#include "CollectiblesManager.h"
#include "Kismet/GameplayStatics.h"
#include "BaseHUD.h" 
#include "EscapeSaveGame.h"
#include "GoalZone.h"

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
	// Use IsValid()! A simple (HUDRef) check passes even if the object is about to be deleted.
	if (IsValid(HUDRef))
	{
		HUDRef->UpdateUI(ShardsRemaining, RequiredShards, FMath::CeilToInt(CurrentTimeRemaining));
	}
}
void ACollectiblesManager::CollectShard()
{
	if (!bLevelActive) return;

	ShardsRemaining--;
	UpdateHUD();

	if (ShardsRemaining <= 0)
	{
		// 1. Show the Unlock Message
		if (HUDRef) HUDRef->ShowMessage("DOOR UNLOCKED! Find the exit.");

		// 2. Find the GoalZone and PHYSICALLY unlock it
		TArray<AActor*> GoalActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGoalZone::StaticClass(), GoalActors);

		// Loop through results just in case there is more than one
		for (AActor* Actor : GoalActors)
		{
			AGoalZone* TheGoal = Cast<AGoalZone>(Actor);
			if (TheGoal)
			{
				TheGoal->UnlockDoor();
			}
		}
	}
}

void ACollectiblesManager::ReachedGoal()
{
	if (!bLevelActive) return;

	// Only finish if shards are collected
	if (ShardsRemaining <= 0)
	{
		// New Message Logic
		if (HUDRef) HUDRef->ShowMessage("Welcome to Level 3!");

		// Short delay to read the message before loading
		FTimerHandle WaitHandle;
		GetWorldTimerManager().SetTimer(WaitHandle, this, &ACollectiblesManager::FinishLevel, 2.0f, false);
	}
	else
	{
		// Feedback if they touch the trigger (physically blocked usually preventing this, but good safety)
		if (HUDRef) HUDRef->ShowMessage("Door Locked! You need more shards.");
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
	// 1. Stop Logic
	bLevelActive = false;
	SetActorTickEnabled(false); // Stop Ticking immediately

	// 2. Kill all timers (Prevents the "Zombie" crash)
	GetWorldTimerManager().ClearAllTimersForObject(this);

	UE_LOG(LogTemp, Warning, TEXT("Level Complete. Transitioning..."));

	// 3. Save Game
	UEscapeSaveGame* SaveInst = Cast<UEscapeSaveGame>(UGameplayStatics::CreateSaveGameObject(UEscapeSaveGame::StaticClass()));
	if (SaveInst)
	{
		SaveInst->CurrentLevelName = NextLevelName;
		UGameplayStatics::SaveGameToSlot(SaveInst, SaveInst->SaveSlotName, SaveInst->UserIndex);
	}

	// 4. Remove HUD Widget immediately (Double safety)
	if (HUDRef)
	{
		// We can't call RemoveFromParent on the HUD Actor, 
		// but we can tell the HUD to clean up its widget if we added a function for it.
		// For now, the BaseHUD::EndPlay will handle it, but we NULL the reference here.
		HUDRef = nullptr;
	}

	// 5. Open Level
	if (!NextLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, NextLevelName);
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

void ACollectiblesManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	// Ensure no timers try to fire after the level closes
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}