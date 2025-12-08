#include "PressurePlate.h"

APressurePlate::APressurePlate()
{
    // 1. The Visual Base
    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    RootComponent = BaseMesh;

    // 2. The Invisible Sensor
    TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
    TriggerZone->SetupAttachment(BaseMesh);
    TriggerZone->SetBoxExtent(FVector(40, 40, 50));
}

int32 APressurePlate::GetCurrentValue()
{
    TArray<AActor*> OverlappingActors;
    TriggerZone->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        // Check if the thing on top is a DataCube
        if (ADataCube* Cube = Cast<ADataCube>(Actor))
        {
            return Cube->SortValue;
        }
    }
    return 0; // Nothing found
}