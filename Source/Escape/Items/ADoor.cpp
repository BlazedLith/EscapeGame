#include "Items/ADoor.h" 
#include "EscapeCharacter.h" 
#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"

ADoor::ADoor()
{
    PrimaryActorTick.bCanEverTick = false;

    DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
    RootComponent = DoorFrame;

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(DoorFrame);

    WireMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WireMesh"));
    WireMesh->SetupAttachment(DoorFrame);
}

void ADoor::OnInteract(AEscapeCharacter* Caller)
{
    if (!Caller) return;
    UInventoryComponent* Inv = Caller->GetInventoryComponent();

    // Helper lambda to check item
    auto CheckHeldItem = [&](FName RequiredID) -> bool
        {
            AItemBase* HeldItem = Inv ? Inv->GetSelectedItem() : nullptr;

            if (!HeldItem)
            {
                Caller->ShowNotification(FText::FromString("I need to hold the right item..."));
                return false;
            }

            if (HeldItem->ItemID != RequiredID)
            {
                FString Msg = FString::Printf(TEXT("This %s won't work here."), *HeldItem->ItemID.ToString());
                Caller->ShowNotification(FText::FromString(Msg));
                return false;
            }
            return true;
        };

    switch (CurrentState)
    {
    case EDoorState::Blocked:
        // 1. Check for Pliers to cut wires
        if (CheckHeldItem(WireItemID))
        {
            Caller->ShowNotification(FText::FromString("Wire cut!"));
            WireMesh->SetVisibility(false);
            WireMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            CurrentState = EDoorState::Locked;
        }
        break;

    case EDoorState::Locked:
        // 2. Check for Key to unlock
        if (CheckHeldItem(KeyItemID))
        {
            // Visually Open
            OpenDoor();
            CurrentState = EDoorState::Open;

            // --- TRANSITION LOGIC ---
            if (!NextLevelName.IsNone())
            {
                // Calls the Character function to Save "NextLevelName" and transition
                Caller->CompleteLevel(NextLevelName);
            }
            else
            {
                Caller->ShowNotification(FText::FromString("Door Unlocked."));
            }
        }
        break;

    case EDoorState::Open:
        // Already open, do nothing
        break;
    }
}

void ADoor::OpenDoor()
{
    FRotator NewRot = DoorMesh->GetRelativeRotation();
    NewRot.Yaw += OpenYaw;
    DoorMesh->SetRelativeRotation(NewRot);
}