// EscapeGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EscapeSaveGame.h" // Include your existing Save class
#include "Kismet/GameplayStatics.h"
#include "EscapeGameInstance.generated.h"

UCLASS()
class ESCAPE_API UEscapeGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Function to be called by "New Game" button
	UFUNCTION(BlueprintCallable, Category = "Game Management")
	void StartNewGame();

	// Function to be called by "Load Game" button
	UFUNCTION(BlueprintCallable, Category = "Game Management")
	void LoadSavedGame();

protected:
	// Helper to get our Save Slot name consistently
	FString SaveSlotName = "EscapeSaveSlot";
};