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

void UInventoryComponent::AddItem(UBaseItem* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0) return;
	TArray<FInventorySlot>& TargetSlots = (Item->ItemType == EItemType::IT_comsume) ? ConsumeSlots : EquipmentSlots;
	for (FInventorySlot& Slot : TargetSlots)
	{
		if (Slot.ItemData == Item && Item->MaxStack > 1 && Slot.Quantity < Item->MaxStack)
		{
			int32 spaceLeft = Item->MaxStack - Slot.Quantity;
			int32 toAdd = FMath::Min(Quantity, spaceLeft);
			Slot.Quantity += toAdd;
			Quantity -= toAdd;
			if (Quantity <= 0)
				break;
		}
	}
	if (Quantity > 0)
	{
		for (FInventorySlot& Slot : TargetSlots)
		{
			if (Slot.ItemData == nullptr)
			{
				int32 toAdd = FMath::Min(Quantity, Item->MaxStack);
				Slot.ItemData = Item;
				Slot.Quantity = toAdd;
				Quantity -= toAdd;
				if (Quantity <= 0)
					break;
			}
		}
	}
	if (linkedInventoryWidget && linkedInventoryWidget->IsInViewport())
	{
		linkedInventoryWidget->RefreshInventory();
	}
}

void UInventoryComponent::UseItem(EItemType Type, int32 Index, ABaseCharacter* Target)
{
	TArray<FInventorySlot>& TargetSlots = (Type == EItemType::IT_comsume) ? ConsumeSlots : EquipmentSlots;
	if (!TargetSlots.IsValidIndex(Index) || !Target) return;
	FInventorySlot& Slot = TargetSlots[Index];
	if (!Slot.ItemData || Slot.Quantity <= 0 || !Slot.ItemData->bUsable) return;
	if (Type == EItemType::IT_comsume)
	{
		UItem_Potion* Potion = Cast<UItem_Potion>(Slot.ItemData);
		if (!Potion) return;

		switch (Potion->PotionType)
		{
		case EPotionType::PO_hp:
			Target->ServerRestoreHP(Potion->RestoreHP);
			break;
		case EPotionType::PO_mp:
			Target->ServerRestoreMP(Potion->RestoreMP);
			break;
		case EPotionType::PO_all:
			Target->ServerRestoreHP(Potion->RestoreHP);
			Target->ServerRestoreMP(Potion->RestoreMP);
			break;
		}

		Slot.Quantity--;
	}
	else if (Type == EItemType::IT_equipment)
	{
		UItem_Equipment* Equip = Cast<UItem_Equipment>(Slot.ItemData);
		if (!Equip || Slot.bEquipped) return;

		Target->EquipmentComponent->Equip(Equip, Target);
		Slot.bEquipped = true;
		Slot.Quantity--;
	}
	if (Slot.Quantity <= 0)
	{
		Slot.ItemData = nullptr;
		Slot.Quantity = 0;
		Slot.bEquipped = false;
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

	if (linkedInventoryWidget && linkedInventoryWidget->IsInViewport())
	{
		linkedInventoryWidget->RefreshInventory();
	}
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
