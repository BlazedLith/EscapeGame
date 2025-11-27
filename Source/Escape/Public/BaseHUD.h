#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

class UMainGameWidget;
class ACollectiblesManager;

UCLASS()
class ESCAPE_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// The Manager calls this to update the UI
	void UpdateUI(int32 Shards, int32 MaxShards, int32 Time);
	void ShowMessage(FString Msg);

protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UMainGameWidget> MainWidgetClass;

private:
	UPROPERTY()
	UMainGameWidget* MainWidget;
};