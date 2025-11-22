#include "Items/PasswordSafe.h"
#include "EscapeCharacter.h"
#include "Items/ItemBase.h"
#include "TimerManager.h" // Required for timers

APasswordSafe::APasswordSafe()
{
    PrimaryActorTick.bCanEverTick = false;
    SafeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SafeMesh"));
    RootComponent = SafeMesh;

    DigitDisplay = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DigitDisplay"));
    DigitDisplay->SetupAttachment(SafeMesh);
    DigitDisplay->SetRelativeLocation(FVector(51.0f, 0.0f, 0.0f)); // Moved forward slightly
    DigitDisplay->SetTextRenderColor(FColor::Red);
    DigitDisplay->SetText(FText::FromString("0"));
}

void APasswordSafe::OnInteract(AEscapeCharacter* Caller)
{
    if (bIsCracked) return;

    // 1. Reset the timer every time we click (Debounce)
    // This means the check only happens when the player STOPS clicking.
    GetWorldTimerManager().ClearTimer(SubmissionTimer);

    // 2. Cycle Number
    CurrentDialNumber = (CurrentDialNumber + 1) % 10;

    // 3. Update Visuals (White while cycling)
    if (DigitDisplay) DigitDisplay->SetTextRenderColor(FColor::White);
    UpdateDisplay();

    // 4. Start Timer: Check validity in 1.5 seconds
    GetWorldTimerManager().SetTimer(SubmissionTimer, this, &APasswordSafe::CheckSubmission, 1.5f, false);
}

void APasswordSafe::CheckSubmission()
{
    // ALGORITHM: Sequence Validation
    // Convert the current character of the password string to an integer
    if (CurrentSequenceIndex >= TargetSequence.Len()) return;

    // Get the target digit (e.g., '4' from "482")
    FString TargetChar = TargetSequence.Mid(CurrentSequenceIndex, 1);
    int32 TargetDigit = FCString::Atoi(*TargetChar);

    if (CurrentDialNumber == TargetDigit)
    {
        // CORRECT DIGIT
        CurrentSequenceIndex++;

        if (CurrentSequenceIndex >= TargetSequence.Len())
        {
            // --- WIN CONDITION ---
            bIsCracked = true;
            if (DigitDisplay)
            {
                DigitDisplay->SetTextRenderColor(FColor::Green);
                DigitDisplay->SetText(FText::FromString("OPEN"));
            }
            // Spawn Key
            if (ItemToSpawn)
            {
                FVector SpawnLoc = GetActorLocation() + (GetActorForwardVector() * 50.0f);
                GetWorld()->SpawnActor<AItemBase>(ItemToSpawn, SpawnLoc, FRotator::ZeroRotator);
            }
        }
        else
        {
            // --- PARTIAL SUCCESS ---
            // Flash Green, then reset dial to 0 for next number
            if (DigitDisplay) DigitDisplay->SetTextRenderColor(FColor::Green);

            // Reset dial to 0 after a brief delay so user sees the green
            FTimerHandle ResetHandle;
            GetWorld()->GetTimerManager().SetTimer(ResetHandle, [this]()
                {
                    CurrentDialNumber = 0;
                    UpdateDisplay();
                    if (DigitDisplay) DigitDisplay->SetTextRenderColor(FColor::White);
                }, 0.5f, false);
        }
    }
    else
    {
        // --- FAILED ---
        // Wrong number entered. Reset the WHOLE puzzle.
        ResetPuzzle();
    }
}

void APasswordSafe::ResetPuzzle()
{
    CurrentSequenceIndex = 0;
    CurrentDialNumber = 0;
    if (DigitDisplay)
    {
        DigitDisplay->SetTextRenderColor(FColor::Red); // Angry Red
    }

    // Delay reset of text
    FTimerHandle ResetHandle;
    GetWorld()->GetTimerManager().SetTimer(ResetHandle, [this]()
        {
            UpdateDisplay();
        }, 1.0f, false);
}

void APasswordSafe::UpdateDisplay()
{
    if (DigitDisplay)
    {
        FString NumStr = FString::FromInt(CurrentDialNumber);
        DigitDisplay->SetText(FText::FromString(NumStr));
    }
}