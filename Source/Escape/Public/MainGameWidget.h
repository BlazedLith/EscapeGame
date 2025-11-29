// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MainGameWidget.generated.h"

/**
 * Simplified Widget: Only handles Time, Shards, and Notification text.
 */
UCLASS()
class ESCAPE_API UMainGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Update functions called by the HUD
	void UpdateCoinCount(int32 CurrentCoins, int32 MaxCoins);
	void UpdateTimer(int32 TimeRemaining);
	void ShowNotification(FString Message);

protected:
	// These names MUST match the Hierarchy in your Widget Blueprint.

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CoinDisplay;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeDisplay;

	// Note: We are using "NotificationDisplay" now, NOT "MainMessageDisplay"
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NotificationDisplay;
};