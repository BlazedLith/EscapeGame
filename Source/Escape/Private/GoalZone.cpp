// Fill out your copyright notice in the Description page of Project Settings.

#include "GoalZone.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CollectiblesManager.h" // Needed to call ReachedGoal()

// Sets default values
AGoalZone::AGoalZone()
{
	PrimaryActorTick.bCanEverTick = false;

	// Trigger Volume setup remains the same...
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;
	TriggerVolume->SetBoxExtent(FVector(150.0f, 150.0f, 150.0f));
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AGoalZone::OnOverlapBegin);

	// 2. CHANGE MESH SETTINGS HERE
	GoalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GoalMesh"));
	GoalMesh->SetupAttachment(RootComponent);

	// CHANGE FROM NoCollision TO BlockAll
	GoalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GoalMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

// Called when the game starts or when spawned
void AGoalZone::BeginPlay()
{
	Super::BeginPlay();

	// Immediately find and cache the manager reference on start
	FindCollectiblesManager();
}

void AGoalZone::FindCollectiblesManager()
{
	// Find the single instance of the ACollectiblesManager in the level
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACollectiblesManager::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		ManagerRef = Cast<ACollectiblesManager>(FoundActors[0]);
		if (!ManagerRef)
		{
			UE_LOG(LogTemp, Error, TEXT("GoalZone: Found an actor but failed to cast to ACollectiblesManager. Check class type."));
		}
	}
	else
	{
		// Log an error if the Manager is missing, as the win condition will fail.
		UE_LOG(LogTemp, Error, TEXT("GoalZone: ACollectiblesManager was not found in the level! The win condition will not work."));
	}
}

void AGoalZone::UnlockDoor()
{
	// Turn off collision so the player can walk through
	GoalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Optional: Make it invisible or change color to show it's open
	// GoalMesh->SetVisibility(false); 
}

void AGoalZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. Check if the overlapping actor is the Player Character
	// FIX: Cast OtherActor to APawn first, then check if that Pawn is player-controlled.
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (PlayerPawn && PlayerPawn->IsPlayerControlled())
	{
		UE_LOG(LogTemp, Log, TEXT("Player entered the Goal Zone!"));

		// 2. Notify the Manager
		if (ManagerRef)
		{
			// Call the public function that marks the player as being at the goal
			ManagerRef->ReachedGoal();
		}
		else
		{
			// Try finding it again (good practice for runtime resilience)
			FindCollectiblesManager();
			if (ManagerRef)
			{
				ManagerRef->ReachedGoal();
			}
		}
	}
}