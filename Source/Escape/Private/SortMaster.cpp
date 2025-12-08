#include "SortMaster.h"
#include "PressurePlate.h" 
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

ASortMaster::ASortMaster()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASortMaster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bPuzzleSolved)
    {
        RunSelectionSortCheck();
    }
}

void ASortMaster::RunSelectionSortCheck()
{
    // 1. SAFETY CHECKS
    if (TargetGate == nullptr) return;
    if (bPuzzleSolved) return;
    if (Plates.Num() != 4) return;

    // 2. READ ALL PLATES
    TArray<int32> CurrentValues;
    for (APressurePlate* Plate : Plates)
    {
        if (!Plate) return;
        int32 Val = Plate->GetCurrentValue();

        // CRITICAL CHECK: If ANY plate is empty, we are NOT ready.
        if (Val == 0)
        {
            bHasCheckedAttempt = false; // Reset the lock so we can try again later
            return; // STOP HERE. Do not judge the player yet.
        }
        CurrentValues.Add(Val);
    }

    // 3. LOCK MECHANISM (Prevent Spamming)
    // If we are here, ALL 4 plates have cubes on them.

    // If we already judged this attempt, do nothing.
    if (bHasCheckedAttempt) return;

    // MARK AS CHECKED: We are judging this attempt now.
    bHasCheckedAttempt = true;

    // 4. SORTING ALGORITHM (Selection Sort)
    TArray<int32> SortedValues = CurrentValues;
    int32 n = SortedValues.Num();
    for (int32 i = 0; i < n - 1; i++)
    {
        int32 MinIdx = i;
        for (int32 j = i + 1; j < n; j++)
        {
            if (SortedValues[j] < SortedValues[MinIdx]) MinIdx = j;
        }
        if (MinIdx != i)
        {
            int32 Temp = SortedValues[MinIdx];
            SortedValues[MinIdx] = SortedValues[i];
            SortedValues[i] = Temp;
        }
    }

    // 5. COMPARE ORDER
    bool bCorrect = true;
    for (int32 i = 0; i < n; i++)
    {
        if (CurrentValues[i] != SortedValues[i])
        {
            bCorrect = false;
            break;
        }
    }

    // 6. WIN or STRIKE
    if (bCorrect)
    {
        // --- SUCCESS ---
        if (TargetGate) TargetGate->Destroy();
        bPuzzleSolved = true;
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("CORRECT! Gate Opened."));
    }
    else
    {
        // --- FAILURE ---
        StrikeCount++;

        // Visual Feedback
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("WRONG ORDER! Strike %d / 3"), StrikeCount));

        // CHECK LOSE CONDITION
        if (StrikeCount >= 3)
        {
            if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GAME OVER - Restarting Level..."));

            // SEND PLAYER BACK TO LEVEL 1
            UGameplayStatics::OpenLevel(this, FName("Lvl_FirstPerson"));
        }
    }
}