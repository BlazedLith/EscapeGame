// ItemBase.cpp

#include "ItemBase.h"
#include "EscapeCharacter.h"
#include "Components/StaticMeshComponent.h"

AItemBase::AItemBase()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    // Enable overlap events so player can detect the item
    Mesh->SetGenerateOverlapEvents(true);
    Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void AItemBase::BeginPlay()
{
    Super::BeginPlay();
}

void AItemBase::OnPickup()
{
    // Hide the item in the world when picked
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);
}

bool AItemBase::OnUse(AEscapeCharacter* UsingCharacter)
{
    if (!UsingCharacter) return false;

    // LOGIC: If this item has text, read it!
    if (!InspectionText.IsEmpty())
    {
        // Call a new function on the character (We will write this in Step 3)
        UsingCharacter->ShowInspectionUI(InspectionText);

        // RETURN FALSE so we DO NOT destroy the item
        return false;
    }

    // Default Behavior (Consumables, etc.)
    UE_LOG(LogTemp, Warning, TEXT("Used Item: %s"), *ItemID.ToString());

    if (bDestroyOnUse)
    {
        Destroy();
    }

    return true;
}

