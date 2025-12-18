#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "DataCube.h" // Includes DataCube so we can detect it
#include "PressurePlate.generated.h"

UCLASS()
class ESCAPE_API APressurePlate : public AActor
{
    GENERATED_BODY()

public:
    APressurePlate();

    // Which number is this? (1st, 2nd, 3rd, 4th)
    UPROPERTY(EditAnywhere, Category = "Algorithm")
    int32 PlateIndex = 0;

    // Helper to get the value of the cube on top
    int32 GetCurrentValue();

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BaseMesh;

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* TriggerZone;
};