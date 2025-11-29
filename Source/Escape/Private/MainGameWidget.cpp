// Fill out your copyright notice in the Description page of Project Settings.

#include "MainGameWidget.h"

void UMainGameWidget::UpdateCoinCount(int32 CurrentCoins, int32 MaxCoins)
{
    if (CoinDisplay)
    {
        int32 Collected = MaxCoins - CurrentCoins;
        // CHANGED TEXT HERE
        FString Text = FString::Printf(TEXT("Coins: %d / %d"), Collected, MaxCoins);
        CoinDisplay->SetText(FText::FromString(Text));
    }
}

void UMainGameWidget::UpdateTimer(int32 TimeRemaining)
{
    if (TimeDisplay)
    {
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
		NotificationDisplay->SetVisibility(ESlateVisibility::Visible);

		TWeakObjectPtr<UMainGameWidget> WeakThis(this);
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, [WeakThis]()
			{
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