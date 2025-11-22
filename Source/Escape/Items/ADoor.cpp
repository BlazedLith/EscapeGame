// Door.cpp
#include "Items/ADoor.h" 
#include "EscapeCharacter.h"
#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"
#include "Kismet/GameplayStatics.h" 
#include "TimerManager.h"           

ADoor::ADoor()
{
    PrimaryActorTick.bCanEverTick = true;

    DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
    RootComponent = DoorFrame;

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(DoorFrame);

    WireMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WireMesh"));
    WireMesh->SetupAttachment(DoorFrame);
}

void ADoor::OnInteract(AEscapeCharacter* Caller)
{
    if (bIsTransitioning) return;

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
        if (CheckHeldItem(WireItemID))
        {
            Caller->ShowNotification(FText::FromString("Wire cut!"));
            WireMesh->SetVisibility(false);
            WireMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            CurrentState = EDoorState::Locked;
        }
        break;

    case EDoorState::Locked:
        if (CheckHeldItem(KeyItemID))
        {
            bIsTransitioning = true;
            // --- NEW LOGIC START ---
            Caller->ShowNotification(FText::FromString("Door Unlocked. Escaping..."));

            // 1. Save the Game (So we keep our items in the next level)
            Caller->SaveGame();

            // 2. Check if we should transition
            if (!NextLevelName.IsNone())
            {
                UE_LOG(LogTemp, Warning, TEXT("Door Unlocked! Loading %s"), *NextLevelName.ToString());

                // Visual: Open the door immediately
                OpenDoor();

                // Logic: Load level after 1.0 second (so player sees the door move)
                FTimerHandle TimerHandle;
                GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
                    {
                        UGameplayStatics::OpenLevel(this, NextLevelName);
                    }, 1.0f, false);
            }
            else
            {
                // If no level name is set, just open normally
                OpenDoor();
                bIsTransitioning = false;
            }
            // --- NEW LOGIC END ---

            CurrentState = EDoorState::Open;
        }
        break;

    case EDoorState::Open:
        break;
    }
}

void ADoor::OpenDoor()
{
    FRotator NewRot = DoorMesh->GetRelativeRotation();
    NewRot.Yaw += OpenYaw;
    DoorMesh->SetRelativeRotation(NewRot);
}