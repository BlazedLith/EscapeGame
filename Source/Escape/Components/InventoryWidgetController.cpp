#include "InventoryWidgetController.h"
#include "InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Border.h"

void UInventoryWidgetController::Initialize(UInventoryComponent* InInventoryComp, UUserWidget* InWidget)
{
    InventoryComp = InInventoryComp;
    Widget = InWidget;

    if (!InventoryComp || !Widget) return;

    // Bind delegates
    InventoryComp->OnSlotChanged.AddDynamic(this, &UInventoryWidgetController::HandleSlotChanged);
    InventoryComp->OnSelectedSlotChanged.AddDynamic(this, &UInventoryWidgetController::HandleSelectionChanged);

    // --- NEW: FORCE INITIAL UPDATE ---
    // We manually run the update loop once so the UI matches the data immediately
    for (int32 i = 0; i < 6; i++)
    {
        HandleSlotChanged(i, InventoryComp->GetItemInSlot(i));
    }
    HandleSelectionChanged(InventoryComp->GetSelectedIndex());
}

void UInventoryWidgetController::HandleSlotChanged(int32 SlotIndex, AItemBase* NewItem)
{
    if (!Widget) return;

    UImage* SlotImage = GetSlotImage(SlotIndex);
    if (!SlotImage) return;

    if (NewItem)
    {
        // Item exists: Set Texture and Opacity 1.0
        SlotImage->SetBrushFromTexture(NewItem->GetIcon());
        SlotImage->SetOpacity(1.0f);
    }
    else
    {
        // Slot empty: Set Texture null and Opacity 0 (Invisible)
        // This prevents "White Box" syndrome on the icon itself
        SlotImage->SetBrushFromTexture(nullptr);
        SlotImage->SetOpacity(0.0f);
    }
}

void UInventoryWidgetController::HandleSelectionChanged(int32 NewIndex)
{
    if (!Widget) return;

    for (int i = 0; i < 6; i++)
    {
        UWidget* Highlight = GetSlotHighlight(i);
        if (!Highlight) continue;

        // We assume the Highlight is a Border or Image overlay
        if (i == NewIndex)
        {
            // Show Highlight
            // Note: Use HitTestInvisible so it doesn't block clicks
            Highlight->SetVisibility(ESlateVisibility::HitTestInvisible);
            Cast<UBorder>(Highlight)->SetBrushColor(FLinearColor(1, 0.8, 0, 0.5)); // Semi-transparent Orange
        }
        else
        {
            // Hide Highlight
            Highlight->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

UImage* UInventoryWidgetController::GetSlotImage(int32 Index)
{
    if (!Widget) return nullptr;

    FString Name = FString::Printf(TEXT("Slot_%d_Image"), Index);
    return Cast<UImage>(Widget->GetWidgetFromName(FName(*Name)));
}

UWidget* UInventoryWidgetController::GetSlotHighlight(int32 Index)
{
    if (!Widget) return nullptr;

    FString Name = FString::Printf(TEXT("Slot_%d_Highlight"), Index);
    return Widget->GetWidgetFromName(FName(*Name));
}
