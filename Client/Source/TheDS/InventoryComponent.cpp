#include "InventoryComponent.h"
#include "Item_Potion.h"
#include "Item_Equipment.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"
#include "InventoryWidget.h"
#include "EquipmentComponent.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
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
	if (LinkedInventoryWidget && LinkedInventoryWidget->IsInViewport())
	{
		LinkedInventoryWidget->RefreshInventory();
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
	if (LinkedInventoryWidget && LinkedInventoryWidget->IsInViewport())
	{
		LinkedInventoryWidget->RefreshInventory();
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
	FInventorySlot& From = TargetSlots[fromIndex];
	FInventorySlot& To = TargetSlots[toIndex];
	if (!From.ItemData || From.Quantity <= 0) return;

	if (To.ItemData && To.ItemData == From.ItemData && From.ItemData->MaxStack > 1)
	{
		const int32 MaxStack = From.ItemData->MaxStack;
		const int32 Space = MaxStack - To.Quantity;
		if (Space > 0)
		{
			const int32 Moved = FMath::Min(Space, From.Quantity);
			To.Quantity += Moved;
			From.Quantity -= Moved;
			if (From.Quantity <= 0)
			{
				From.ItemData = nullptr;
				From.Quantity = 0;
				From.bEquipped = false;
			}
			if (LinkedInventoryWidget && LinkedInventoryWidget->IsInViewport())
			{
				LinkedInventoryWidget->RefreshInventory();
			}
			return;
		}
	}
	if (!To.ItemData)
	{
		To = From;
		From.ItemData = nullptr;
		From.Quantity = 0;
		From.bEquipped = false;
	}
	else
	{
		TargetSlots.Swap(fromIndex, toIndex);
	}
	if (LinkedInventoryWidget && LinkedInventoryWidget->IsInViewport())
	{
		LinkedInventoryWidget->RefreshInventory();
	}
}

void UInventoryComponent::RemoveItem(EItemType type, int32 index)
{
	TArray<FInventorySlot>& Slots = (type == EItemType::IT_comsume) ? ConsumeSlots : EquipmentSlots;
	if (!Slots.IsValidIndex(index)) return;

	Slots[index].ItemData = nullptr;
	Slots[index].Quantity = 0;
	Slots[index].bEquipped = false;

	if (LinkedInventoryWidget && LinkedInventoryWidget->IsInViewport())
	{
		LinkedInventoryWidget->RefreshInventory();
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

bool UInventoryComponent::HasItem(UBaseItem* Item, int32 NeedAmount) const
{
	if (!Item) return false;
	int32 Sum = 0;
	for (const FInventorySlot& S : ConsumeSlots)   if (S.ItemData == Item) Sum += S.Quantity;
	for (const FInventorySlot& S : EquipmentSlots) if (S.ItemData == Item) Sum += S.Quantity;
	return Sum >= NeedAmount;
}

void UInventoryComponent::OnRep_ConsumeSlots()
{
	if (LinkedInventoryWidget && LinkedInventoryWidget->IsInViewport())
	{
		LinkedInventoryWidget->RefreshInventory();
	}
}

void UInventoryComponent::OnRep_EquipmentSlots()
{
	if (LinkedInventoryWidget && LinkedInventoryWidget->IsInViewport())
	{
		LinkedInventoryWidget->RefreshInventory();
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UInventoryComponent, ConsumeSlots, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UInventoryComponent, EquipmentSlots, COND_OwnerOnly)
}
