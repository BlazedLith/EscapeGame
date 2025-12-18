#include "MenuGameMode.h"
#include "Kismet/GameplayStatics.h"

void AMenuGameMode::StartPlay()
{
	Super::StartPlay();

	// 1. Create and Show the Widget
	if (MainMenuWidgetClass)
	{
		UUserWidget* MenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
		}
	}

	// 2. Setup the Mouse Cursor and Input Mode
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->bShowMouseCursor = true;

		// Set Input Mode to UI Only (so keyboard doesn't move a character)
		FInputModeUIOnly InputModeData;
		if (MainMenuWidgetClass)
		{
			// Optional: Focus the widget immediately
			InputModeData.SetWidgetToFocus(nullptr);
		}
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputModeData);
	}
}