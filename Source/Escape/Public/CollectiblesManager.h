#pragma once

#include "Escape.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EscapeSaveGame.h"
#include "CollectiblesManager.generated.h"

// Forward Declaration
class ABaseHUD;

UCLASS()
class ESCAPE_API ACollectiblesManager : public AActor
{
	GENERATED_BODY()

public:
	ACollectiblesManager();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// --- INTERNAL HELPERS ---
	void FinishLevel();
	void RestartLevel();
	void UpdateHUD();

public:
	// --- PUBLIC API FOR GAMEPLAY ACTORS ---

	/** Called by the Shard Actor when overlapped */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void CollectShard();

	/** Called by the GoalZone Actor when overlapped */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ReachedGoal();

	/** Called by EnemyAI when it hits the player */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void PlayerHitByEnemy();

	// --- CONFIGURATION ---

	/** Reference to the HUD to update UI directly */
	UPROPERTY()
	ABaseHUD* HUDRef;

	/** The name of the next level map (e.g. Level3) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Settings")
	FName NextLevelName;

	/** The number of shards the player must collect to win. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Settings")
	int32 RequiredShards = 5;

	/** The maximum time (in seconds) allowed for the level. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Settings")
	float MaxLevelTime = 120.0f;

	/** Time delay (in seconds) before restarting the level. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Settings")
	float RestartDelay = 3.0f;

private:
	// --- STATE VARIABLES ---
	int32 ShardsRemaining;
	float CurrentTimeRemaining;
	bool bLevelActive = false;
	FTimerHandle RestartTimerHandle;
};