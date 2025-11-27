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

	// DEBUG: Draw Path
	for (int i = 0; i < CurrentPath.Num() - 1; ++i)
	{
		DrawDebugLine(GetWorld(), CurrentPath[i], CurrentPath[i + 1], FColor::Cyan, false, 0.0f, 0, 5.0f);
	}
	// DEBUG: Draw Status
	if (CurrentPath.Num() == 0)
	{
		// RED = Pathfinding Failed (Check Z-Axis or Obstacles)
		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Red, TEXT("AI STATUS: NO PATH FOUND"));
	}
	else
	{
		// GREEN = Path Found (Should be moving)
		if (GEngine) GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Green, TEXT("AI STATUS: MOVING"));
	}
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

	// === CRITICAL FIX 1: MOVEMENT TOLERANCE ===
	// Must be SMALLER than Grid Size (e.g. 10.0f). 
	// If this is 100.0f, the enemy thinks it has already arrived and skips steps.
	if (DistanceToTarget < 10.0f)
	{
		PathIndex++;
	}
	else
	{
		FVector NewLocation = CurrentLocation + Direction * MovementSpeed * DeltaTime;

		// === CRITICAL FIX 2: DISABLE SWEEP ===
		// Use 'false' for sweep. If 'true', the enemy stops if it touches the floor even slightly.
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
	// 1. Grid Alignment
	FVector StartGridPos = FVector(FMath::RoundToFloat(StartWorldPos.X / GridSize) * GridSize,
		FMath::RoundToFloat(StartWorldPos.Y / GridSize) * GridSize,
		StartWorldPos.Z);

	// === CRITICAL FIX 3: Z-AXIS FLATTENING ===
	// We use StartGridPos.Z instead of TargetWorldPos.Z.
	// This forces the "Goal" to be on the same height level as the Enemy.
	FVector TargetGridPos = FVector(FMath::RoundToFloat(TargetWorldPos.X / GridSize) * GridSize,
		FMath::RoundToFloat(TargetWorldPos.Y / GridSize) * GridSize,
		StartGridPos.Z);

	// Visual Debug
	DrawDebugSphere(GetWorld(), StartGridPos, 50.0f, 12, FColor::Green, false, 0.0f, 0, 5.0f);
	DrawDebugSphere(GetWorld(), TargetGridPos, 50.0f, 12, FColor::Red, false, 0.0f, 0, 5.0f);

	TArray<FNode> OpenList;
	TSet<FNode> ClosedSet;
	TMap<FVector, FNode*> VisitedNodes;

	auto CleanupMemory = [&VisitedNodes]()
		{
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
		OpenList.Sort([](const FNode& A, const FNode& B) { return A.F() < B.F(); });
		FNode CurrentNode = OpenList[0];
		OpenList.RemoveAt(0);

		FNode* CurrentHeapNode = VisitedNodes.FindRef(CurrentNode.Position);

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
	FHitResult HitResult;

	FCollisionQueryParams Params;

	Params.AddIgnoredActor(this);



	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();



	// Trace Start: Top of capsule

	FVector TraceStart = Location + FVector(0, 0, CapsuleHalfHeight);

	// Trace End: Stop 15 units ABOVE the floor to avoid hitting it

	FVector TraceEnd = Location - FVector(0, 0, CapsuleHalfHeight - 15.0f);



	// === CRITICAL FIX 4: CHECK RADIUS ===

	// Use a small radius (10.0f) instead of full capsule size.

	// This ensures we only block if the center of the path is truly blocked.

	FCollisionShape SphereShape = FCollisionShape::MakeSphere(10.0f);

	// Draw PURPLE DOTS where the AI is trying to walk

	DrawDebugPoint(GetWorld(), Location, 10.0f, FColor::Purple, false, 0.1f);



	if (GetWorld()->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, FQuat::Identity, ECC_WorldStatic, SphereShape, Params))

	{

		return false; // Wall hit

	}

	return true; // Walkable
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