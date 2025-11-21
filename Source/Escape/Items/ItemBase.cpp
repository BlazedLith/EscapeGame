// ItemBase.cpp

#include "ItemBase.h"
#include "Components/StaticMeshComponent.h"

AItemBase::AItemBase()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    // Enable overlap events so player can detect the item
    Mesh->SetGenerateOverlapEvents(true);
    Mesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
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
    // Default behavior: item does nothing
    // Override in child classes like KeyItem or PuzzlePieceItem

    if (bDestroyOnUse)
    {
        Destroy();
    }

    return true;
}

