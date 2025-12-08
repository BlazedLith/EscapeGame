#include "BaseHUD.h"
#include "MainGameWidget.h"
#include "Kismet/GameplayStatics.h"

void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidgetClass)
	{
		// Create the ONE and ONLY widget for Level 2
		MainWidget = CreateWidget<UMainGameWidget>(GetWorld(), MainWidgetClass);
		if (MainWidget)
		{
			MainWidget->AddToViewport();
			// Ensure notification text is hidden by default
			MainWidget->ShowNotification("");
		}
	}
}

void ABaseHUD::UpdateUI(int32 Coins, int32 MaxCoins, int32 Time)
{
	if (MainWidget)
	{
		MainWidget->UpdateCoinCount(Coins, MaxCoins);
		MainWidget->UpdateTimer(Time);
	}
}

void ABaseHUD::ShowMessage(FString Msg)
{
	if (MainWidget)
	{
		MainWidget->ShowNotification(Msg);
	}
}

void ABaseHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// CRITICAL: Remove the Level 2 UI from the viewport
	if (MainWidget)
	{
		MainWidget->RemoveFromParent();
		MainWidget = nullptr;
	}
}