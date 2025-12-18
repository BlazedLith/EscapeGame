#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h" 
#include "EscapeMessageWidget.h"
#include "EscapeMainMenuWidget.generated.h"

UCLASS()
class ESCAPE_API UEscapeMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// NativeConstruct is the "BeginPlay" for Widgets
	virtual void NativeConstruct() override;

protected:
	// The names here MUST match the button names in your Blueprint Designer EXACTLY
	UPROPERTY(meta = (BindWidget))
	UButton* NewGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LoadGameButton;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UEscapeMessageWidget> MessageWidgetClass;

private:
	// Functions to handle the clicks
	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnLoadGameClicked();

	// Internal helper to spawn your widget
	void ShowMenuNotification(FText Message);
};