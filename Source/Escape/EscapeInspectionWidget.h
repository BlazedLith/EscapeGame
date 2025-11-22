#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "EscapeInspectionWidget.generated.h"

UCLASS()
class ESCAPE_API UEscapeInspectionWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetNoteContent(FText NewText);

protected:
    // Bind to a Text Block in the Designer
    UPROPERTY(meta = (BindWidget))
    UTextBlock* NoteText;
};