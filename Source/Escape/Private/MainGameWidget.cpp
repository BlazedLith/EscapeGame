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
		int32 Min = TimeRemaining / 60;
		int32 Sec = TimeRemaining % 60;

		// CHANGED: Added "Timer: " prefix
		FString Text = FString::Printf(TEXT("Timer: %02d:%02d"), Min, Sec);

		TimeDisplay->SetText(FText::FromString(Text));
	}
}

void UMainGameWidget::ShowNotification(FString Message)
{
    if (NotificationDisplay)
    {
        NotificationDisplay->SetText(FText::FromString(Message));
        NotificationDisplay->SetVisibility(ESlateVisibility::Visible);

        // SAFER: Check if the World exists before setting a timer
        if (UWorld* World = GetWorld())
        {
            FTimerHandle Handle;
            World->GetTimerManager().SetTimer(Handle, [this]()
                {
                    // Verify the object is still valid (IsValid is an Unreal check)
                    if (IsValid(this) && NotificationDisplay)
                    {
                        NotificationDisplay->SetVisibility(ESlateVisibility::Hidden);
                    }
                }, 3.0f, false);
        }
    }
}