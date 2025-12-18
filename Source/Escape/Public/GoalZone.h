#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoalZone.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class ACollectiblesManager;

UCLASS()
class ESCAPE_API AGoalZone : public AActor
{
    GENERATED_BODY()

public:
    AGoalZone();

    // Called by CollectiblesManager when all coins are found
    UFUNCTION()
    void UnlockDoor();

    // Visual Mesh
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* GoalMesh;

    // Trigger Volume
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* TriggerVolume;

    // Level to load when reached (Default: Level3)
    UPROPERTY(EditAnywhere, Category = "Game Flow")
    FName NextLevelName = "Level3";

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    ACollectiblesManager* ManagerRef;

    bool bIsUnlocked = false;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    void FindCollectiblesManager();
};