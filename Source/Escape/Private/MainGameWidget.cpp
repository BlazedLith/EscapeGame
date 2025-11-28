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

        // 1. Create a "Weak Pointer" to self. This is safe to check later.
        TWeakObjectPtr<UMainGameWidget> WeakThis(this);

        FTimerHandle Handle;
        // 2. Capture the Weak Pointer, NOT 'this'
        GetWorld()->GetTimerManager().SetTimer(Handle, [WeakThis]()
            {
                // 3. Check if the widget is still alive
                if (UMainGameWidget* StrongThis = WeakThis.Get())
                {
                    if (StrongThis->NotificationDisplay)
                    {
                        StrongThis->NotificationDisplay->SetVisibility(ESlateVisibility::Hidden);
                    }
                }
            }, 3.0f, false);
    }
}