// InventoryComponent.cpp

#include "InventoryComponent.h"
#include "EscapeCharacter.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize array with 6 nullptr slots
	Items.Init(nullptr, MaxSlots);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UInventoryComponent::AddItem(AItemBase* NewItem)
{
	if (!NewItem || IsFull())
		return false;

	for (int32 i = 0; i < MaxSlots; i++)
	{
		if (Items[i] == nullptr)
		{
			Items[i] = NewItem;

			// Tell UI to update slot
			OnSlotChanged.Broadcast(i, NewItem);

			return true;
		}
	}
	return false;
}

void UInventoryComponent::RemoveItem(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= MaxSlots)
		return;

	Items[SlotIndex] = nullptr;

	// UI update
	OnSlotChanged.Broadcast(SlotIndex, nullptr);
}

void UInventoryComponent::SelectSlot(int32 NewIndex)
{
	if (NewIndex < 0 || NewIndex >= MaxSlots)
		return;

	SelectedIndex = NewIndex;

	// UI update
	OnSelectedSlotChanged.Broadcast(NewIndex);
}

void UInventoryComponent::UseSelectedItem(class AEscapeCharacter* Character)
{
	// Get the item currently selected
	if (AItemBase* Item = GetSelectedItem())
	{
		// Try to use it
		if (Item->OnUse(Character))
		{
			if (!IsValid(Item))
			{
				RemoveItem(SelectedIndex);
			}
			if (Item->ShouldDestroyOnUse())
			{
				RemoveItem(SelectedIndex);
			}
		}
	}
}



AItemBase* UInventoryComponent::GetItemInSlot(int32 SlotIndex) const
{
	if (SlotIndex < 0 || SlotIndex >= MaxSlots)
		return nullptr;

	return Items[SlotIndex];
}

AItemBase* UInventoryComponent::GetSelectedItem() const
{
	return Items[SelectedIndex];
}

bool UInventoryComponent::IsFull() const
{
	for (AItemBase* Item : Items)
	{
		if (Item == nullptr)
			return false;
	}
	return true;
}

bool UInventoryComponent::HasItem(FName TargetItemID) const
{
	for (AItemBase* Item : Items)
	{
		// Check if item exists AND matches the ID we need
		if (Item && Item->ItemID == TargetItemID)
		{
			return true;
		}
	}
	return false;
}