#include "InventoryComponent.h"
#include "Item_Potion.h"
#include "Item_Equipment.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"
#include "InventoryWidget.h"
#include "EquipmentComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ConsumeSlots.SetNum(MaxSlotCount);
	EquipmentSlots.SetNum(MaxSlotCount);
}

void UInventoryComponent::AddItem(UBaseItem* item, int32 quantity)
{
	if (!item || quantity <= 0) return;
	TArray<FInventorySlot>& TargetSlots = (item->itemType == EItemType::IT_comsume) ? ConsumeSlots : EquipmentSlots;
	for (FInventorySlot& slot : TargetSlots)
	{
		if (slot.ItemData == item && item->maxStack > 1 && slot.Quantity < item->maxStack)
		{
			int32 spaceLeft = item->maxStack - slot.Quantity;
			int32 toAdd = FMath::Min(quantity, spaceLeft);
			slot.Quantity += toAdd;
			quantity -= toAdd;
			if (quantity <= 0)
				break;
		}
	}
	for (FInventorySlot& slot : TargetSlots)
	{
		if (slot.ItemData == nullptr)
		{
			int32 toAdd = FMath::Min(quantity, item->maxStack);
			slot.ItemData = item;
			slot.Quantity = toAdd;
			quantity -= toAdd;
			if (quantity <= 0)
				break;
		}
	}
	if (linkedInventoryWidget && linkedInventoryWidget->IsInViewport())
	{
		linkedInventoryWidget->RefreshInventory();
	}
}

void UInventoryComponent::UseItem(EItemType type, int32 index, ABaseCharacter* target)
{
	TArray<FInventorySlot>& TargetSlots = (type == EItemType::IT_comsume) ? ConsumeSlots : EquipmentSlots;
	if (!TargetSlots.IsValidIndex(index) || !target) return;
	FInventorySlot& slot = TargetSlots[index];
	if (!slot.ItemData || slot.Quantity <= 0 || !slot.ItemData->bUsable) return;
	if (type == EItemType::IT_comsume)
	{
		UItem_Potion* potion = Cast<UItem_Potion>(slot.ItemData);
		if (!potion) return;

		switch (potion->potionType)
		{
		case EPotionType::PO_hp:
			target->ServerRestoreHP(potion->restoreHP);
			break;
		case EPotionType::PO_mp:
			target->ServerRestoreMP(potion->restoreMP);
			break;
		case EPotionType::PO_all:
			target->ServerRestoreHP(potion->restoreHP);
			target->ServerRestoreMP(potion->restoreMP);
			break;
		}

		slot.Quantity--;
	}
	else if (type == EItemType::IT_equipment)
	{
		UItem_Equipment* equip = Cast<UItem_Equipment>(slot.ItemData);
		if (!equip || slot.bEquipped) return;

		target->EquipmentComponent->Equip(equip, target);
		slot.bEquipped = true;
		slot.Quantity--;
	}
	if (slot.Quantity <= 0)
	{
		slot.ItemData = nullptr;
		slot.Quantity = 0;
		slot.bEquipped = false;
	}
	if (linkedInventoryWidget && linkedInventoryWidget->IsInViewport())
	{
		linkedInventoryWidget->RefreshInventory();
	}
}

void UInventoryComponent::SwapItem(EItemType type, int32 fromIndex, int32 toIndex)
{
	TArray<FInventorySlot>& TargetSlots = (type == EItemType::IT_comsume) ? ConsumeSlots : EquipmentSlots;
	if (!TargetSlots.IsValidIndex(fromIndex) || !TargetSlots.IsValidIndex(toIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Indices"));
		return;
	}
	if (fromIndex == toIndex) return;
	TargetSlots.Swap(fromIndex, toIndex);

	if (linkedInventoryWidget && linkedInventoryWidget->IsInViewport())
	{
		linkedInventoryWidget->RefreshInventory();
	}
}

void UInventoryComponent::RemoveItem(EItemType type, int32 index)
{
	TArray<FInventorySlot>& Slots = (type == EItemType::IT_comsume) ? ConsumeSlots : EquipmentSlots;
	if (!Slots.IsValidIndex(index)) return;

	Slots[index].ItemData = nullptr;
	Slots[index].Quantity = 0;
	Slots[index].bEquipped = false;
}

const TArray<FInventorySlot>& UInventoryComponent::GetSlots(EItemType type) const
{
	if (type == EItemType::IT_comsume)
	{
		return ConsumeSlots;
	}
	else
	{
		return EquipmentSlots;
	}
}
