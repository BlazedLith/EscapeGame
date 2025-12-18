// QueueBridgeManager.cpp
#include "QueueBridgeManager.h"
#include "Engine/StaticMeshActor.h" 
#include "TimerManager.h"

AQueueBridgeManager::AQueueBridgeManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AQueueBridgeManager::BeginPlay()
{
    Super::BeginPlay();

    // DO NOTHING HERE. 
    // We wait for the player to trigger us.
}

void AQueueBridgeManager::StartBridgeGeneration()
{
    // 1. Reset the start location to exactly where the icon is NOW
    NextSpawnLocation = GetActorLocation();

    // 2. Start the Timers (Enqueue and Dequeue)
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AQueueBridgeManager::SpawnPlatform, SpawnInterval, true);

    // Start destroying with a delay
    GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AQueueBridgeManager::DequeuePlatform, SpawnInterval, true, PlatformLifeTime);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("BRIDGE ACTIVATED!"));
}

void AQueueBridgeManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AQueueBridgeManager::SpawnPlatform()
{
    if (!PlatformMesh) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // --- ALGORITHM: SINE WAVE ---
    // notice: We deleted "float Amplitude = 300.0f"
    // We now use the variables "WaveAmplitude" and "WaveFrequency" from the Header

    float WaveHeight = WaveAmplitude * FMath::Sin(TotalDistanceSpawned * WaveFrequency);

    // Create new location
    FVector WaveLocation = NextSpawnLocation;
    WaveLocation.Z = GetActorLocation().Z + WaveHeight;
    // ----------------------------

    AStaticMeshActor* NewPlatform = GetWorld()->SpawnActor<AStaticMeshActor>(WaveLocation, FRotator::ZeroRotator, SpawnParams);

    if (NewPlatform)
    {
        // 1. Mobility FIRST
        NewPlatform->SetMobility(EComponentMobility::Movable);

        // 2. Mesh SECOND
        NewPlatform->GetStaticMeshComponent()->SetStaticMesh(PlatformMesh);

        NewPlatform->SetActorScale3D(FVector(1.5f, 1.5f, 0.2f));

        if (Materials.Num() > 0)
        {
            NewPlatform->GetStaticMeshComponent()->SetMaterial(0, Materials[ColorIndex]);
            ColorIndex = (ColorIndex + 1) % Materials.Num();
        }

        BridgeQueue.Enqueue(NewPlatform);

        // Increment Logic
        NextSpawnLocation.X += StepDistance;
        TotalDistanceSpawned += StepDistance;
    }
}
void AQueueBridgeManager::DequeuePlatform()
{
    // Safety check
    if (BridgeQueue.IsEmpty()) return;

    // 1. DEQUEUE: Remove the oldest item from the front
    AActor* OldPlatform = nullptr;
    BridgeQueue.Dequeue(OldPlatform);

    // 2. Destroy it physically
    if (OldPlatform)
    {
        OldPlatform->Destroy();
    }
}

void AQueueBridgeManager::ResetBridge()
{
    // 1. Clear Timers (Stop spawning/destroying)
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
    GetWorldTimerManager().ClearTimer(DestroyTimerHandle);

    // 2. Destroy all existing cubes in the queue
    while (!BridgeQueue.IsEmpty())
    {
        AActor* Platform = nullptr;
        BridgeQueue.Dequeue(Platform);
        if (Platform)
        {
            Platform->Destroy();
        }
    }

    // 3. Reset Variables
    NextSpawnLocation = GetActorLocation();
    TotalDistanceSpawned = 0.0f;
    ColorIndex = 0;

    //if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Bridge Reset!"));
}