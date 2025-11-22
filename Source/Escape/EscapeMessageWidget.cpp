#include "EscapeMessageWidget.h"

void UEscapeMessageWidget::ShowMessage(FText Message)
{
    if (MessageText)
    {
        MessageText->SetText(Message);

        // Ensure it is visible
        SetVisibility(ESlateVisibility::Visible);
    }
}