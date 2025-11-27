// Fill out your copyright notice in the Description page of Project Settings.

#include "GoalZone.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CollectiblesManager.h" // Needed to call ReachedGoal()

// Sets default values
AGoalZone::AGoalZone()
{
	// This actor doesn't need to update every frame
	PrimaryActorTick.bCanEverTick = false;

	// 1. Setup the Trigger Volume (Root Component)
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;

	// Configure the trigger volume size and type
	TriggerVolume->SetBoxExtent(FVector(150.0f, 150.0f, 150.0f)); // Make it a large, visible trigger area
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger")); // Set to the standard Trigger profile

	// Bind the function to the overlap event
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AGoalZone::OnOverlapBegin);

	// 2. Setup the Visual Mesh (Child Component)
	GoalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GoalMesh"));
	GoalMesh->SetupAttachment(RootComponent);
	GoalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Visual mesh should not interfere with physics
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