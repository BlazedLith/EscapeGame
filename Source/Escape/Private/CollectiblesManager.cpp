#include "CollectiblesManager.h"
#include "Kismet/GameplayStatics.h"
#include "BaseHUD.h" 
#include "EscapeSaveGame.h"

// Sets default values
ACollectiblesManager::ACollectiblesManager()
{
	// Enable Tick to allow for timer countdown.
	PrimaryActorTick.bCanEverTick = true;
}

void ACollectiblesManager::BeginPlay()
{
	Super::BeginPlay();

	// 1. Find and cache the HUD reference
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		HUDRef = Cast<ABaseHUD>(PC->GetHUD());
	}

	// 2. Initialize Game State
	ShardsRemaining = RequiredShards;
	CurrentTimeRemaining = MaxLevelTime;
	bLevelActive = true;

	// 3. Initial UI Update
	UpdateHUD();
	if (HUDRef)
	{
		HUDRef->ShowMessage("Collect all shards to unlock the Goal!");
	}
}

void ACollectiblesManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bLevelActive)
	{
		CurrentTimeRemaining -= DeltaTime;

		// Update UI with new time
		UpdateHUD();

		// Check for Time Out
		if (CurrentTimeRemaining <= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Timer hit 0. Level Restart."));
			RestartLevel();
		}
	}
}

void ACollectiblesManager::UpdateHUD()
{
	// Send the raw data to the HUD, which passes it to the Widget
	if (HUDRef)
	{
		HUDRef->UpdateUI(ShardsRemaining, RequiredShards, FMath::CeilToInt(CurrentTimeRemaining));
	}
}

void ACollectiblesManager::CollectShard()
{
	if (!bLevelActive) return;

	ShardsRemaining--;

	// Update the counter on screen
	UpdateHUD();

	// Provide Feedback
	if (ShardsRemaining <= 0)
	{
		if (HUDRef) HUDRef->ShowMessage("GOAL UNLOCKED! Find the exit.");
	}
}

void ACollectiblesManager::ReachedGoal()
{
	if (!bLevelActive) return;

	// Only finish if shards are collected
	if (ShardsRemaining <= 0)
	{
		FinishLevel();
	}
	else
	{
		// Feedback if player tries to leave too early
		if (HUDRef) HUDRef->ShowMessage("Goal Locked! You need more shards.");
	}
}

void ACollectiblesManager::PlayerHitByEnemy()
{
	if (!bLevelActive) return;

	UE_LOG(LogTemp, Warning, TEXT("Player Hit by Enemy! Level Restart."));

	if (HUDRef) HUDRef->ShowMessage("Caught by Enemy!");

	RestartLevel();
}

void ACollectiblesManager::FinishLevel()
{
	bLevelActive = false;

	UE_LOG(LogTemp, Warning, TEXT("Level 2 Complete. Saving and Loading Level 3..."));

	// 1. Save Game Logic (Persist data between levels)
	UEscapeSaveGame* SaveInst = Cast<UEscapeSaveGame>(UGameplayStatics::CreateSaveGameObject(UEscapeSaveGame::StaticClass()));
	if (SaveInst)
	{
		SaveInst->CurrentLevelName = NextLevelName;
		UGameplayStatics::SaveGameToSlot(SaveInst, SaveInst->SaveSlotName, SaveInst->UserIndex);
	}

	// 2. Direct Transition
	if (!NextLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, NextLevelName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NextLevelName is not set in CollectiblesManager Details Panel!"));
	}
}

void ACollectiblesManager::RestartLevel()
{
	bLevelActive = false;

	// Optional: Tell HUD to show Game Over if you want, 
	// or just let the restart happen.
	if (HUDRef) HUDRef->ShowMessage("GAME OVER");

	UE_LOG(LogTemp, Warning, TEXT("Restarting Level in %f seconds."), RestartDelay);

	GetWorldTimerManager().SetTimer(RestartTimerHandle, [this]()
		{
			const FName CurrentLevelName = FName(*UGameplayStatics::GetCurrentLevelName(this));
			UGameplayStatics::OpenLevel(this, CurrentLevelName);
		}, RestartDelay, false);
}