#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "ADoor.generated.h"

UENUM(BlueprintType)
enum class EDoorState : uint8
{
    Blocked, // Needs Pliers
    Locked,  // Needs Key
    Open     // Done
};

UCLASS()
class ESCAPE_API ADoor : public AActor, public IInteractionInterface
{
    GENERATED_BODY()

public:
    ADoor();

    // Interface function
    virtual void OnInteract(class AEscapeCharacter* Caller) override;

protected:
    // --- COMPONENTS ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* DoorFrame;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* DoorMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* WireMesh;

    // --- SETTINGS ---
    UPROPERTY(EditAnywhere, Category = "Lock")
    EDoorState CurrentState = EDoorState::Blocked;

    UPROPERTY(EditAnywhere, Category = "Lock")
    FName WireItemID = "Pliers";

    UPROPERTY(EditAnywhere, Category = "Lock")
    FName KeyItemID = "MainKey";

    UPROPERTY(EditAnywhere, Category = "Lock")
    float OpenYaw = 90.0f;

    // --- NEW: Level Transition ---
    // Name of the level to load (e.g. "Level2"). Leave None to just open.
    UPROPERTY(EditAnywhere, Category = "Game Flow")
    FName NextLevelName;

    bool bIsTransitioning = false;

    void OpenDoor();
};