// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAI.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CollectiblesManager.h" 
#include "NavigationSystem.h" 
#include "DrawDebugHelpers.h" 

// Sets default values
AEnemyAI::AEnemyAI()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemyAI::OnHitPlayer);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

	PathIndex = 0;
	TargetPlayer = nullptr;
	ManagerRef = nullptr;
}

void AEnemyAI::BeginPlay()
{
	Super::BeginPlay();

	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACollectiblesManager::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		ManagerRef = Cast<ACollectiblesManager>(FoundActors[0]);
	}

	if (TargetPlayer)
	{
		StartPathfinding(TargetPlayer->GetActorLocation());
	}
}

void AEnemyAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Re-evaluate path periodically 
	static float PathUpdateTimer = 0.0f;
	PathUpdateTimer += DeltaTime;

	if (PathUpdateTimer >= 0.5f && TargetPlayer)
	{
		StartPathfinding(TargetPlayer->GetActorLocation());
		PathUpdateTimer = 0.0f;
	}

	MoveAlongPath(DeltaTime);

	// === NEW: MANUAL CAPTURE CHECK ===
	// Since we disabled Sweep, we manually check if we are touching the player.
	if (TargetPlayer)
	{
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetPlayer->GetActorLocation());

		// 80.0f is slightly larger than (EnemyRadius 34 + PlayerRadius 34)
		// This ensures we catch the player even if physics is pushing us apart.
		if (DistanceToPlayer < 80.0f)
		{
			NotifyManagerOfPlayerHit();
		}
	}
	// =================================

	/* DEBUG: Draw Path
	//for (int i = 0; i < CurrentPath.Num() - 1; ++i)
	{
		DrawDebugLine(GetWorld(), CurrentPath[i], CurrentPath[i + 1], FColor::Cyan, false, 0.0f, 0, 5.0f);
	}

	if (CurrentPath.Num() == 0)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Red, TEXT("AI STATUS: NO PATH FOUND"));
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Green, TEXT("AI STATUS: MOVING"));
	}*/
}

void AEnemyAI::StartPathfinding(const FVector& TargetLocation)
{
	CurrentPath.Empty();
	PathIndex = 0;
	CurrentPath = FindPathAStar(GetActorLocation(), TargetLocation);
}

void AEnemyAI::MoveAlongPath(float DeltaTime)
{
	if (CurrentPath.Num() == 0 || PathIndex >= CurrentPath.Num()) return;

	FVector TargetPoint = CurrentPath[PathIndex];
	FVector CurrentLocation = GetActorLocation();

	FVector Direction = (TargetPoint - CurrentLocation);
	Direction.Z = 0.0f;
	float DistanceToTarget = Direction.Size();
	Direction.Normalize();

	// Tolerance check: Must be significantly smaller than GridSize to avoid skipping nodes
	if (DistanceToTarget < 10.0f)
	{
		PathIndex++;
	}
	else
	{
		FVector NewLocation = CurrentLocation + Direction * MovementSpeed * DeltaTime;

		// Disable sweep to prevent getting stuck on floor seams
		SetActorLocation(NewLocation, false);

		if (!Direction.IsNearlyZero())
		{
			FRotator NewRotation = Direction.Rotation();
			NewRotation.Pitch = 0.0f;
			NewRotation.Roll = 0.0f;
			SetActorRotation(NewRotation);
		}
	}
}

TArray<FVector> AEnemyAI::FindPathAStar(const FVector& StartWorldPos, const FVector& TargetWorldPos)
{
	// 1. Calculate Grid Positions (Lifted +50z to avoid floor clipping issues)
	float PathHeight = StartWorldPos.Z + 50.0f;

	FVector StartGridPos = FVector(
		FMath::RoundToFloat(StartWorldPos.X / GridSize) * GridSize,
		FMath::RoundToFloat(StartWorldPos.Y / GridSize) * GridSize,
		PathHeight
	);

	FVector TargetGridPos = FVector(
		FMath::RoundToFloat(TargetWorldPos.X / GridSize) * GridSize,
		FMath::RoundToFloat(TargetWorldPos.Y / GridSize) * GridSize,
		PathHeight
	);

	// Basic Validation: If start or end are unwalkable, return empty path immediately
	if (!IsWalkable(StartGridPos) || !IsWalkable(TargetGridPos))
	{
		return TArray<FVector>();
	}

	// --- A* ALGORITHM EXECUTION ---
	TArray<FNode> OpenList;
	TSet<FNode> ClosedSet;
	TMap<FVector, FNode*> VisitedNodes;

	auto CleanupMemory = [&VisitedNodes]() {
		for (auto It : VisitedNodes) delete It.Value;
		VisitedNodes.Empty();
		};

	float HScoreStart = CalculateHScore(StartGridPos, TargetGridPos);
	FNode StartNode(StartGridPos, 0.0f, HScoreStart, nullptr);

	FNode* HeapStartNode = new FNode(StartNode);
	OpenList.Add(*HeapStartNode);
	VisitedNodes.Add(StartGridPos, HeapStartNode);

	while (OpenList.Num() > 0)
	{
		// Sort by lowest F Cost
		OpenList.Sort([](const FNode& A, const FNode& B) { return A.F() < B.F(); });
		FNode CurrentNode = OpenList[0];
		OpenList.RemoveAt(0);

		FNode* CurrentHeapNode = VisitedNodes.FindRef(CurrentNode.Position);

		// Check if we reached the target (Approximate match)
		if (CurrentNode.Position.Equals(TargetGridPos, GridSize / 2.0f))
		{
			TArray<FVector> Path;
			FNode* PathNode = CurrentHeapNode;
			while (PathNode)
			{
				Path.Insert(PathNode->Position, 0);
				PathNode = PathNode->Parent;
			}
			CleanupMemory();
			return Path;
		}

		ClosedSet.Add(CurrentNode);

		TArray<FNode> Neighbors;
		GetNeighbors(CurrentNode, Neighbors, TargetGridPos);

		for (const FNode& Neighbor : Neighbors)
		{
			if (ClosedSet.Contains(Neighbor) || !IsWalkable(Neighbor.Position)) continue;

			float NewG = CurrentNode.G + GridSize;
			int32 IndexInOpenList = OpenList.IndexOfByPredicate([&Neighbor](const FNode& N) { return N == Neighbor; });

			if (IndexInOpenList == INDEX_NONE)
			{
				float HScore = CalculateHScore(Neighbor.Position, TargetGridPos);
				FNode* NewNeighborNode = new FNode(Neighbor.Position, NewG, HScore, CurrentHeapNode);
				OpenList.Add(*NewNeighborNode);
				VisitedNodes.Add(Neighbor.Position, NewNeighborNode);
			}
			else if (NewG < OpenList[IndexInOpenList].G)
			{
				FNode& ExistingNodeInList = OpenList[IndexInOpenList];
				ExistingNodeInList.G = NewG;
				FNode* HeapNode = VisitedNodes.FindRef(ExistingNodeInList.Position);
				if (HeapNode)
				{
					HeapNode->G = NewG;
					HeapNode->Parent = CurrentHeapNode;
				}
			}
		}
	}

	CleanupMemory();
	return TArray<FVector>();
}

float AEnemyAI::CalculateHScore(const FVector& A, const FVector& B) const
{
	return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y);
}

void AEnemyAI::GetNeighbors(const FNode& CurrentNode, TArray<FNode>& Neighbors, const FVector& TargetWorldPos)
{
	const FVector Pos = CurrentNode.Position;
	FVector Directions[] = {
		FVector(GridSize, 0, 0), FVector(-GridSize, 0, 0),
		FVector(0, GridSize, 0), FVector(0, -GridSize, 0)
	};

	for (const FVector& Direction : Directions)
	{
		FVector NeighborPos = Pos + Direction;
		FNode Neighbor(NeighborPos, 0.0f, CalculateHScore(NeighborPos, TargetWorldPos), nullptr);
		Neighbors.Add(Neighbor);
	}
}

bool AEnemyAI::IsWalkable(const FVector& Location) const
{
	UWorld* World = GetWorld();
	if (!World) return false;

	// 1. Setup Collision Params (Ignore Player & Enemy to prevent blocking path)
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (TargetPlayer) Params.AddIgnoredActor(TargetPlayer);

	// 2. Define Dimensions
	FVector TestLocation = Location + FVector(0, 0, 60.0f); // Waist height
	float CheckRadius = 30.0f; // Skinny check

	// 3. OBSTACLE CHECK (Sphere Trace against WorldStatic)
	FHitResult HitResult;
	bool bHitWall = World->SweepSingleByChannel(
		HitResult, TestLocation, TestLocation + FVector(0, 0, 1),
		FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeSphere(CheckRadius), Params
	);

	if (bHitWall) return false;

	// 4. FLOOR CHECK (Deep Trace using Visibility)
	// We trace down significantly to handle uneven terrain
	FVector TraceEnd = TestLocation - FVector(0, 0, 400.0f);

	bool bHitFloor = World->LineTraceSingleByChannel(
		HitResult, TestLocation, TraceEnd, ECC_Visibility, Params
	);

	return bHitFloor;
}

void AEnemyAI::OnHitPlayer(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn && Pawn->IsPlayerControlled()) NotifyManagerOfPlayerHit();
}

void AEnemyAI::NotifyManagerOfPlayerHit()
{
	if (ManagerRef) ManagerRef->PlayerHitByEnemy();
}