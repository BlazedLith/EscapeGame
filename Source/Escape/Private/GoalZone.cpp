#include "GoalZone.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CollectiblesManager.h" 
#include "EscapeCharacter.h" 

AGoalZone::AGoalZone()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    RootComponent = TriggerVolume;
    TriggerVolume->SetBoxExtent(FVector(150.0f, 150.0f, 150.0f));
    TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
    TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AGoalZone::OnOverlapBegin);

    GoalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GoalMesh"));
    GoalMesh->SetupAttachment(RootComponent);

    // Default: Block the player so they can't leave yet
    GoalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GoalMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AGoalZone::BeginPlay()
{
    Super::BeginPlay();
    FindCollectiblesManager();

    // Ensure locked at start
    bIsUnlocked = false;
}

void AGoalZone::FindCollectiblesManager()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACollectiblesManager::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        ManagerRef = Cast<ACollectiblesManager>(FoundActors[0]);
    }
}

void AGoalZone::UnlockDoor()
{
    // 1. Mark as Unlocked
    bIsUnlocked = true;

    // 2. Disable collision so player can walk through
    GoalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGoalZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 1. Check if it is the Player Character
    AEscapeCharacter* MyChar = Cast<AEscapeCharacter>(OtherActor);

    if (MyChar)
    {
        // 2. Notify Manager (Good for UI updates like "Player at Goal")
        if (ManagerRef)
        {
            ManagerRef->ReachedGoal();
        }
        if (bIsUnlocked)
        {
            if (!NextLevelName.IsNone())
            {
                MyChar->CompleteLevel(NextLevelName);
            }
        }
    }
}