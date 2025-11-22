#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "EscapeMessageWidget.generated.h"

UCLASS()
class ESCAPE_API UEscapeMessageWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Function to update the text on screen
    void ShowMessage(FText Message);

protected:
    // Bind this to a Text Block in your Blueprint named "MessageText"
    UPROPERTY(meta = (BindWidget))
    UTextBlock* MessageText;

    // Animation to fade it out (Optional, but nice)
    UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
    UWidgetAnimation* FadeOutAnim;
};