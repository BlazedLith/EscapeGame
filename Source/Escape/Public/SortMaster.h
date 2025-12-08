#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SortMaster.generated.h"

// Forward declaration (Tells Unreal this class exists)
class APressurePlate;

UCLASS()
class ESCAPE_API ASortMaster : public AActor
{
    GENERATED_BODY()

public:
    ASortMaster();
    virtual void Tick(float DeltaTime) override;

    // --- CONFIGURATION ---

    // The List of Plates (Order Matters: 1, 2, 3, 4)
    UPROPERTY(EditInstanceOnly, Category = "Algorithm")
    TArray<APressurePlate*> Plates;

    // The Gate to destroy when you win
    UPROPERTY(EditInstanceOnly, Category = "Algorithm")
    AActor* TargetGate;

private:
    // The Sorting Algorithm logic
    void RunSelectionSortCheck();
    int32 StrikeCount = 0;
    bool bHasCheckedAttempt = false;
    bool bPuzzleSolved = false;


protected:
    float StabilityTimer = 0.0f;
};