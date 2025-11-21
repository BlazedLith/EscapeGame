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

void UInventoryComponent::UseSelectedItem(AEscapeCharacter* Character)
{
	if (!Character)
		return;

	AItemBase* Item = GetSelectedItem();
	if (!Item)
		return;

	bool bResult = Item->OnUse(Character);

	if (bResult)
	{
		// If the item destroys itself, or is marked for removal
		if (!IsValid(Item) || Item->IsActorBeingDestroyed())
		{
			RemoveItem(SelectedIndex);
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
