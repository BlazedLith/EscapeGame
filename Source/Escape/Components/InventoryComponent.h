// InventoryComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemBase.h"
#include "InventoryComponent.generated.h"

// Delegate for UI updates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotChanged, int32, SlotIndex, AItemBase*, NewItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedSlotChanged, int32, NewIndex);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ESCAPE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetSelectedIndex() const { return SelectedIndex; }

protected:
	virtual void BeginPlay() override;

	// Maximum inventory size (6 slots)
	static const int32 MaxSlots = 6;

	// Fixed-size array of item pointers
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<AItemBase*> Items;

	// Currently selected slot index
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int32 SelectedIndex = 0;

public:

	// Delegates for UI to bind to
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventorySlotChanged OnSlotChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSelectedSlotChanged OnSelectedSlotChanged;

	// Add an item to the first empty slot
	bool AddItem(AItemBase* NewItem);

	// Remove item from slot
	void RemoveItem(int32 SlotIndex);

	// Select a slot (0–5)
	void SelectSlot(int32 NewIndex);

	// Use item in selected slot
	void UseSelectedItem(class AEscapeCharacter* Character);

	// Getter for specific slot
	AItemBase* GetItemInSlot(int32 SlotIndex) const;

	// Getter for selected item
	AItemBase* GetSelectedItem() const;

	// Helper → is inventory full?
	bool IsFull() const;
};
