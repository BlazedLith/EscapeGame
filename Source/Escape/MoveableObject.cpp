#include "MoveableObject.h"
#include "EscapeCharacter.h"
#include "Components/InventoryComponent.h"

AMoveableObject::AMoveableObject()
{
    PrimaryActorTick.bCanEverTick = true; // Enable ticking for animation

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
}

void AMoveableObject::BeginPlay()
{
    Super::BeginPlay();

    // Cache the starting location
    ClosedPosition = GetActorLocation();

    // Calculate target based on enum
    FVector Offset = FVector::ZeroVector;
    switch (Direction)
    {
    case EMoveDirection::X_Axis: Offset.X = MoveDistance; break;
    case EMoveDirection::Y_Axis: Offset.Y = MoveDistance; break;
    case EMoveDirection::Z_Axis: Offset.Z = MoveDistance; break;
    }
    TargetPosition = ClosedPosition + Offset;
}

void AMoveableObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ALGORITHM: Linear Interpolation for smooth physics movement
    FVector CurrentPos = GetActorLocation();
    FVector Destination = bIsOpen ? TargetPosition : ClosedPosition;

    // If we are not at the destination, move towards it
    if (!CurrentPos.Equals(Destination, 0.1f))
    {
        FVector NewPos = FMath::VInterpConstantTo(CurrentPos, Destination, DeltaTime, MoveSpeed * 100.0f);
        SetActorLocation(NewPos);
    }
}

// MoveableObject.cpp

void AMoveableObject::OnInteract(AEscapeCharacter* Caller)
{
    if (!bIsOpen)
    {
        if (bIsLocked && Caller)
        {
            UInventoryComponent* Inv = Caller->GetInventoryComponent();

            // --- NEW: Checks ONLY the selected slot ---
            AItemBase* HeldItem = Inv ? Inv->GetSelectedItem() : nullptr;

            // Check if we are holding ANYTHING, and if it matches the ID
            if (!HeldItem || HeldItem->ItemID != RequiredItemID)
            {
                UE_LOG(LogTemp, Warning, TEXT("Locked! You must hold the: %s"), *RequiredItemID.ToString());
                // Optional: Play a "Locked" sound here
                return;
            }
        }
        // If we get here, we are holding the correct item!
        bIsOpen = true;
    }
}