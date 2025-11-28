// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h" // Required for the GetCapsuleComponent() functions in .cpp
#include "EnemyAI.generated.h"

// Forward Declaration for the Manager
class ACollectiblesManager;

// === A* PATHFINDING NODE STRUCTURE ===
// This structure represents a grid cell or point in the maze
USTRUCT()
struct FNode
{
	GENERATED_BODY()

	// Location in the world (or grid coordinates)
	FVector Position;

	// G Score: Cost from the start node
	float G;

	// H Score: Heuristic (estimated cost to the end node)
	float H;

	// F Score: Total cost (F = G + H)
	float F() const { return G + H; }

	// Pointer to the previous node to reconstruct the path
	// NOTE: Pointers require careful memory management (handled in .cpp).
	FNode* Parent;

	// Default Constructor
	FNode() : G(0.0f), H(0.0f), Parent(nullptr) {}

	// Custom Constructor
	FNode(FVector Pos, float GScore, float HScore, FNode* ParentNode)
		: Position(Pos), G(GScore), H(HScore), Parent(ParentNode) {
	}

	// Comparison operator for use in TSet/TMap/sorting
	bool operator==(const FNode& Other) const
	{
		// FIX: Reduce tolerance to 1.0f. 
		// We only want to match if they are practically the SAME point.
		return Position.Equals(Other.Position, 1.0f);
	}
};

// Custom Hash function for FNode to be used in THashSet (TSet)
// The FORCEINLINE is often required for custom hash functions.
FORCEINLINE uint32 GetTypeHash(const FNode& Node)
{
	// Combine the hashes of the components of the FVector
	return HashCombine(GetTypeHash(Node.Position.X), HashCombine(GetTypeHash(Node.Position.Y), GetTypeHash(Node.Position.Z)));
}
// === END A* PATHFINDING NODE STRUCTURE ===


UCLASS()
class ESCAPE_API AEnemyAI : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyAI();

	virtual void Tick(float DeltaTime) override;

	// Starts the pathfinding process to the target location.
	void StartPathfinding(const FVector& TargetLocation);

protected:
	virtual void BeginPlay() override;

private:
	// ... (rest of the private members) ...

	UPROPERTY()
	ACharacter* TargetPlayer;

	UPROPERTY(EditAnywhere, Category = "AI|Pathfinding")
	float GridSize = 200.0f;

	UPROPERTY(EditAnywhere, Category = "AI|Movement")
	float MovementSpeed = 300.0f;

	TArray<FVector> CurrentPath;

	int32 PathIndex;

	UPROPERTY()
	ACollectiblesManager* ManagerRef;

	// === COLLISION HANDLER ===

	UFUNCTION()
	void OnHitPlayer(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void NotifyManagerOfPlayerHit();

	// === A* ALGORITHM IMPLEMENTATION ===

	TArray<FVector> FindPathAStar(const FVector& StartWorldPos, const FVector& TargetWorldPos);

	float CalculateHScore(const FVector& A, const FVector& B) const;

	void GetNeighbors(const FNode& CurrentNode, TArray<FNode>& Neighbors, const FVector& TargetWorldPos);

	bool IsWalkable(const FVector& Location) const;

	// === MOVEMENT LOGIC ===

	void MoveAlongPath(float DeltaTime);
};