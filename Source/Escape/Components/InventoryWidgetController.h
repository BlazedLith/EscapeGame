#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/Widget.h"
#include "InventoryWidgetController.generated.h"

// Forward declarations
class UInventoryComponent;
class UUserWidget;
class UImage;
class AItemBase;

UCLASS()
class ESCAPE_API UInventoryWidgetController : public UObject
{
    GENERATED_BODY()

public:
    // Called after creating the controller to initialize it
    void Initialize(UInventoryComponent* InInventoryComp, UUserWidget* InWidget);

protected:

    // References
    UPROPERTY()
    UInventoryComponent* InventoryComp;

    UPROPERTY()
    UUserWidget* Widget;

    // UI update handlers
    UFUNCTION()
    void HandleSlotChanged(int32 SlotIndex, AItemBase* NewItem);

    UFUNCTION()
    void HandleSelectionChanged(int32 NewIndex);

    // Helper to get an image widget by name inside the UI
    UImage* GetSlotImage(int32 Index);

    // Helper to get the highlight border (if you use Border instead of Image, change class)
    UWidget* GetSlotHighlight(int32 Index);
};
