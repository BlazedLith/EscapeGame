#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SortMaster.generated.h"

class APressurePlate;

UCLASS()
class ESCAPE_API ASortMaster : public AActor
{
    GENERATED_BODY()

public:
    ASortMaster();
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditInstanceOnly, Category = "Algorithm")
    TArray<APressurePlate*> Plates;

    UPROPERTY(EditInstanceOnly, Category = "Algorithm")
    AActor* TargetGate;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> VictoryWidgetClass;

private:
    void RunSelectionSortCheck();
    int32 StrikeCount = 0;
    bool bHasCheckedAttempt = false;
    bool bPuzzleSolved = false;

protected:
    float StabilityTimer = 0.0f;
};