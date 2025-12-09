#include "EscapeMainMenuWidget.h"
#include "EscapeGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UEscapeMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (NewGameButton)
        NewGameButton->OnClicked.AddDynamic(this, &UEscapeMainMenuWidget::OnNewGameClicked);

    if (LoadGameButton)
        LoadGameButton->OnClicked.AddDynamic(this, &UEscapeMainMenuWidget::OnLoadGameClicked);
}

void UEscapeMainMenuWidget::OnNewGameClicked()
{
    if (UEscapeGameInstance* GI = Cast<UEscapeGameInstance>(GetGameInstance()))
    {
        GI->StartNewGame();
    }
}

void UEscapeMainMenuWidget::OnLoadGameClicked()
{
    // 1. Check if Save Exists
    if (UGameplayStatics::DoesSaveGameExist("EscapeSaveSlot", 0))
    {
        if (UEscapeGameInstance* GI = Cast<UEscapeGameInstance>(GetGameInstance()))
        {
            GI->LoadSavedGame();
        }
    }
    else
    {
        ShowMenuNotification(FText::FromString("No Saved Game Found!"));
    }
}

void UEscapeMainMenuWidget::ShowMenuNotification(FText Message)
{
    if (MessageWidgetClass)
    {
        // Create and show your standard message widget
        UEscapeMessageWidget* MsgWidget = CreateWidget<UEscapeMessageWidget>(GetWorld(), MessageWidgetClass);
        if (MsgWidget)
        {
            MsgWidget->AddToViewport(100); // High Z-Order so it appears on top
            MsgWidget->ShowMessage(Message);

            // Optional: Remove it after 3 seconds to keep memory clean
            FTimerHandle Handle;
            GetWorld()->GetTimerManager().SetTimer(Handle, [MsgWidget]()
                {
                    if (MsgWidget) MsgWidget->RemoveFromParent();
                }, 3.0f, false);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("MessageWidgetClass is not selected in WBP_MainMenu Details Panel!"));
    }
}