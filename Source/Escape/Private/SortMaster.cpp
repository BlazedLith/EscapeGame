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

    if (bPuzzleSolved) return;
    if (Plates.Num() != 4) return;

    bool bAllPlatesFull = true;

    for (APressurePlate* Plate : Plates)
    {
        if (!Plate) return;

        if (Plate->GetCurrentValue() == 0)
        {
            bAllPlatesFull = false;
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

    // Selection Sort Algo
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

    if (bCorrect)
    {
        if (TargetGate) TargetGate->Destroy();
        bPuzzleSolved = true;
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("SUCCESS! Gate Opened."));
    }
    else
    {
        StrikeCount++;

        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("WRONG ORDER! Strike %d / 3"), StrikeCount));

        if (StrikeCount >= 3)
        {
            if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GAME OVER - Restarting Level..."));

            UGameplayStatics::OpenLevel(this, FName("Lvl_FirstPerson"));
        }
    }
}