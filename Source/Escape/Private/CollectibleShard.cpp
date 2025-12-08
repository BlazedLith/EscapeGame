// Fill out your copyright notice in the Description page of Project Settings.

#include "CollectibleShard.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CollectiblesManager.h" // Include the manager header

// Sets default values
ACollectibleShard::ACollectibleShard()
{
	// Shards don't need to update every frame
	PrimaryActorTick.bCanEverTick = false;

	// Setup the collision root component
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;
	CollisionComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ACollectibleShard::OnOverlapBegin);

	// Setup the mesh component (visual only)
	ShardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShardMesh"));
	ShardMesh->SetupAttachment(RootComponent);
	ShardMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ACollectibleShard::BeginPlay()
{
	Super::BeginPlay();
}

void ACollectibleShard::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapping actor is the player character
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn && Pawn->IsPlayerControlled())
	{
		NotifyManagerOfCollection();
		// The shard is collected, destroy it immediately
		Destroy();
	}
}

void ACollectibleShard::NotifyManagerOfCollection()
{
	// Find the single instance of the ACollectiblesManager in the level
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACollectiblesManager::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		ACollectiblesManager* Manager = Cast<ACollectiblesManager>(FoundActors[0]);
		if (Manager)
		{
			Manager->CollectCoin();
		}
	}
}