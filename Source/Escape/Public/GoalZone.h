// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoalZone.generated.h"

// Forward declarations for components and the manager
class UBoxComponent;
class UStaticMeshComponent;
class ACollectiblesManager;

UCLASS()
class ESCAPE_API AGoalZone : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGoalZone();

	UFUNCTION()
	void UnlockDoor();

	// === COMPONENTS ===

	// A simple cube/mesh to represent the goal visually (optional, but good for placement)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* GoalMesh;

	// The large trigger volume that detects the player's entry
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* TriggerVolume;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// === STATE AND REFERENCES ===

	// Cached reference to the Collectibles Manager in the level
	UPROPERTY()
	ACollectiblesManager* ManagerRef;

	// === COLLISION HANDLER ===

	/**
	 * @brief Handles the overlap event when an actor enters the TriggerVolume.
	 * @param OverlappedComp The component that was overlapped (TriggerVolume).
	 * @param OtherActor The actor that triggered the overlap (hopefully the player).
	 * ... other parameters
	 */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Helper function to find and cache the Manager
	void FindCollectiblesManager();
};