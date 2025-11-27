// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectibleShard.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class ESCAPE_API ACollectibleShard : public AActor
{
	GENERATED_BODY()

public:
	ACollectibleShard();

	// === COMPONENTS ===

	// Visual representation of the shard
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* ShardMesh;

	// The component that handles the collision/overlap with the player
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionComp;

protected:
	virtual void BeginPlay() override;

private:
	// === COLLISION HANDLER ===

	// Called when something overlaps the CollisionComp
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Helper function to find and call the Manager
	void NotifyManagerOfCollection();
};