#include "SortMaster.h"
#include "PressurePlate.h" 
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "EscapeCharacter.h"    
#include "EscapeSaveGame.h"        
#include "Blueprint/UserWidget.h"

ASortMaster::ASortMaster()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASortMaster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bPuzzleSolved) return;
    if (Plates.Num() != 4) return;

    bool bAllPlatesFull = true;

    for (APressurePlate* Plate : Plates)
    {
        if (!Plate) return;

        if (Plate->GetCurrentValue() == 0)
        {
            bAllPlatesFull = false;
            break;
        }
    }

    if (bAllPlatesFull)
    {
        StabilityTimer += DeltaTime;
        if (StabilityTimer > 1.0f && !bHasCheckedAttempt)
        {
            RunSelectionSortCheck();
            bHasCheckedAttempt = true;
        }
    }
    else
    {
        StabilityTimer = 0.0f;
        bHasCheckedAttempt = false;
    }
}

void ASortMaster::RunSelectionSortCheck()
{
    // 1. READ VALUES
    TArray<int32> CurrentValues;
    for (APressurePlate* Plate : Plates)
    {
        CurrentValues.Add(Plate->GetCurrentValue());
    }

    // 2. SORT & COMPARE (Selection Sort)
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

    bool bCorrect = true;
    for (int32 i = 0; i < n; i++)
    {
        if (CurrentValues[i] != SortedValues[i])
        {
            bCorrect = false;
            break;
        }
    }

    // 3. HANDLE RESULT
    if (bCorrect)
    {
        // PUZZLE SOLVED 
        if (TargetGate) TargetGate->Destroy();
        bPuzzleSolved = true;

        // Clear Save Data (Game Completed requirement)
        AEscapeCharacter* PlayerChar = Cast<AEscapeCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        if (PlayerChar)
        {
            PlayerChar->ClearSaveGameData();
        }

        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("SUCCESS! Gate Opened."));
    }
    else
    {
        //  PUZZLE FAILED 
        StrikeCount++;

        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("WRONG ORDER! Strike %d / 3"), StrikeCount));

        // If 3 strikes, PENALTY: Send back to Level 1
        if (StrikeCount >= 3)
        {
            if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GAME OVER - Returning to Level 1..."));

            // We must update the save file so "Load Game" puts them in Level 1, not back here.
            if (UEscapeSaveGame* SaveInst = Cast<UEscapeSaveGame>(UGameplayStatics::CreateSaveGameObject(UEscapeSaveGame::StaticClass())))
            {
                // Reset to Level 1
                // MAKE SURE THIS MATCHES YOUR LEVEL 1 MAP NAME EXACTLY!
                SaveInst->CurrentLevelName = FName("Lvl_FirstPerson");

                // Reset Position
                SaveInst->PlayerLocation = FVector::ZeroVector;

                // Clear Inventory (Penalty: Start Fresh)
                SaveInst->InventoryItems.Empty();

                // Save to Disk
                UGameplayStatics::SaveGameToSlot(SaveInst, "EscapeSaveSlot", 0);
            }

            // Transition
            UGameplayStatics::OpenLevel(this, FName("Lvl_FirstPerson"));
        }
    }
}