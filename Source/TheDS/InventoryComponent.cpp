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
}

void UInventoryComponent::AddItem(UBaseItem* item, int32 quantity)
{
	if (!item || quantity <= 0) return;
	UE_LOG(LogTemp, Warning, TEXT("Get Item %s"), *item->itemName.ToString());
	for (FInventorySlot& slot : AllSlots)
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
	while (quantity > 0)
	{
		int32 toAdd = FMath::Min(quantity, item->maxStack);
		FInventorySlot NewSlot;
		NewSlot.ItemData = item;
		NewSlot.Quantity = toAdd;
		AllSlots.Add(NewSlot);
		quantity -= toAdd;
	}
	if (linkedInventoryWidget && linkedInventoryWidget->IsInViewport())
	{
		linkedInventoryWidget->RefreshInventory();
	}
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
		target->EquipmentComponent->Equip(equip, target);
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
	for (int32 i = 0; i < AllSlots.Num(); ++i)
	{
		const FInventorySlot& slot = AllSlots[i];
		if (slot.ItemData && slot.ItemData->itemType == filterType)
		{
			FInventorySlot copy = slot;
			copy.OriginalIndex = i;
			result.Add(copy);
		}
	}
	return result;
}