// QueueBridgeManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h" // Required for the Queue Data Structure
#include "QueueBridgeManager.generated.h"

UCLASS()
class ESCAPE_API AQueueBridgeManager : public AActor
{
    GENERATED_BODY()

public:
    AQueueBridgeManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // --- SETTINGS ---
    // The visual mesh for the platform (cube)
    UPROPERTY(EditAnywhere, Category = "Bridge")
    UStaticMesh* PlatformMesh;

    // The colors (Red, Blue, Green)
    UPROPERTY(EditAnywhere, Category = "Bridge")
    TArray<UMaterialInterface*> Materials;

    // How fast new platforms appear (Enqueue)
    UPROPERTY(EditAnywhere, Category = "Bridge")
    float SpawnInterval = 1.0f;

    // How long they last before disappearing (Dequeue)
    UPROPERTY(EditAnywhere, Category = "Bridge")
    float PlatformLifeTime = 3.0f;

    // Distance between steps
    UPROPERTY(EditAnywhere, Category = "Bridge")
    float StepDistance = 250.0f;

    // Wave Height (How tall the curve is). Try 80.0 to 100.0.
    UPROPERTY(EditAnywhere, Category = "Bridge")
    float WaveAmplitude = 80.0f;

    // Wave Speed (How tight the curve is). Try 0.005.
    UPROPERTY(EditAnywhere, Category = "Bridge")
    float WaveFrequency = 0.005f;

    UFUNCTION(BlueprintCallable, Category = "Bridge")
    void ResetBridge();

private:
    // --- INTERNAL LOGIC ---
    void SpawnPlatform();  // Enqueue
    void DequeuePlatform(); // Dequeue

    // Timers to handle the timing
    FTimerHandle SpawnTimerHandle;
    FTimerHandle DestroyTimerHandle;

    // THE QUEUE DATA STRUCTURE (FIFO)
    // We store references to the spawned platforms here
    TQueue<AActor*> BridgeQueue;

    // Where to spawn the next one
    FVector NextSpawnLocation;
    int32 ColorIndex = 0;

public:
    // ... existing functions ...

    // New Function to start the bridge manually
    UFUNCTION(BlueprintCallable, Category = "Bridge")
    void StartBridgeGeneration();

private:
    // ... existing variables ...

    // Tracks distance for the Sine Wave Algorithm
    float TotalDistanceSpawned = 0.0f;
};
