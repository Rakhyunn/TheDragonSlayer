#include "InventoryComponent.h"
#include "Item_Potion.h"
#include "Item_Equipment.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"
#include "InventoryWidget.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::AddItem(UBaseItem* item, int32 quantity)
{
	if (!item || quantity <= 0) return;
	UE_LOG(LogTemp, Warning, TEXT("Get Item %s"), *item->itemName.ToString());
	for (FInventorySlot& slot : AllSlots)
	{
		if (slot.ItemData == item && item->maxStack > 1)
		{
			slot.Quantity += quantity;
			return;
		}
	}
	// »õ ½½·Ô Ãß°¡
	FInventorySlot NewSlot;
	NewSlot.ItemData = item;
	NewSlot.Quantity = quantity;
	AllSlots.Add(NewSlot);
}

void UInventoryComponent::UseItem(int32 index, ABaseCharacter* target)
{
	if (!AllSlots.IsValidIndex(index) || !target) return;
	UE_LOG(LogTemp, Warning, TEXT("Use Item"));
	FInventorySlot& slot = AllSlots[index];
	if (!slot.ItemData || slot.Quantity <= 0 || !slot.ItemData->bUsable) return;
	if (slot.ItemData->itemType == EItemType::IT_comsume) 
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
		if (slot.Quantity <= 0)
		{
			AllSlots.RemoveAt(index);
		}
	}
	else if (slot.ItemData->itemType == EItemType::IT_equipment) 
	{
		UItem_Equipment* equip = Cast<UItem_Equipment>(slot.ItemData);
		if (!equip || slot.bEquipped) return;

		switch (equip->equipType)
		{
		case EEquiptype::EQ_weapon:
			target->ServerAddAttack(equip->attackPlus);
			break;
		case EEquiptype::EQ_armor:
			target->ServerAddAttack(equip->defensePlus);
			break;
		case EEquiptype::EQ_head:
			target->ServerAddAttack(equip->defensePlus);
			break;
		}
		slot.bEquipped = true;
		AllSlots.RemoveAt(index);
	}
	if (linkedInventoryWidget && linkedInventoryWidget->IsInViewport())
	{
		linkedInventoryWidget->RefreshInventory();
	}
}

TArray<FInventorySlot> UInventoryComponent::GetFilteredSlots(EItemType filterType) const
{
	TArray<FInventorySlot> result;
	for (const FInventorySlot& slot : AllSlots)
	{
		if (slot.ItemData && slot.ItemData->itemType == filterType)
		{
			result.Add(slot);
		}
	}
	return result;
}