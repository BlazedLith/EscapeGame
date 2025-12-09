#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h" // Required for UI
#include "MenuGameMode.generated.h"

UCLASS()
class ESCAPE_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;

protected:
	// This allows you to select "WBP_MainMenu" in the Blueprint editor
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
};