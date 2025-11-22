#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "MoveableObject.generated.h"

UENUM(BlueprintType)
enum class EMoveDirection : uint8
{
    X_Axis,
    Y_Axis,
    Z_Axis
};

UCLASS()
class ESCAPE_API AMoveableObject : public AActor, public IInteractionInterface
{
    GENERATED_BODY()

public:
    AMoveableObject();
    virtual void Tick(float DeltaTime) override;

    // --- INTERFACE ---
    virtual void OnInteract(class AEscapeCharacter* Caller) override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComp;

    // --- SETTINGS ---
    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveDistance = 50.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    EMoveDirection Direction = EMoveDirection::Y_Axis;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveSpeed = 5.0f;

    // If true, needs an item (like Screwdriver) to move
    UPROPERTY(EditAnywhere, Category = "Lock")
    bool bIsLocked = false;

    UPROPERTY(EditAnywhere, Category = "Lock", meta = (EditCondition = "bIsLocked"))
    FName RequiredItemID;

    // --- STATE ---
    bool bIsOpen = false;
    FVector ClosedPosition;
    FVector TargetPosition;
};