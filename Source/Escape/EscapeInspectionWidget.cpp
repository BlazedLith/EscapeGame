#include "EscapeInspectionWidget.h"

void UEscapeInspectionWidget::SetNoteContent(FText NewText)
{
    if (NoteText)
    {
        NoteText->SetText(NewText);
    }
}