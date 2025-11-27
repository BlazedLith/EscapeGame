// Fill out your copyright notice in the Description page of Project Settings.

#include "MainGameWidget.h"

void UMainGameWidget::UpdateShardCount(int32 CurrentShards, int32 MaxShards)
{
	if (ShardDisplay)
	{
		// Format: "3 / 5"
		int32 Collected = MaxShards - CurrentShards;
		FString Text = FString::Printf(TEXT("%d / %d"), Collected, MaxShards);
		ShardDisplay->SetText(FText::FromString(Text));
	}
}

void UMainGameWidget::UpdateTimer(int32 TimeRemaining)
{
	if (TimeDisplay)
	{
		// Format: "01:20"
		int32 Min = TimeRemaining / 60;
		int32 Sec = TimeRemaining % 60;
		FString Text = FString::Printf(TEXT("%02d:%02d"), Min, Sec);
		TimeDisplay->SetText(FText::FromString(Text));
	}
}

void UMainGameWidget::ShowNotification(FString Message)
{
	if (NotificationDisplay)
	{
		NotificationDisplay->SetText(FText::FromString(Message));

		// Make it visible
		NotificationDisplay->SetVisibility(ESlateVisibility::Visible);

		// Auto-hide after 3 seconds using a Timer
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, [this]()
			{
				// Check if 'this' is still valid before accessing members
				if (this && this->NotificationDisplay)
				{
					this->NotificationDisplay->SetVisibility(ESlateVisibility::Hidden);
				}
			}, 3.0f, false);
	}
}