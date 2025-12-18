#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataCube.generated.h"

UCLASS()
class ESCAPE_API ADataCube : public AActor
{
    GENERATED_BODY()

public:
    ADataCube();

    // The Secret Weight (1, 2, 3, 4...)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
    int32 SortValue = 1;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComp;

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

private:
    FVector StartLocation;
};