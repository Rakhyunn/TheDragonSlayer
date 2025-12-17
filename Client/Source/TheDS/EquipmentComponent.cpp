#include "EquipmentComponent.h"
#include "BaseCharacter.h"
#include "InventoryComponent.h"
#include "EquipmentWidget.h"

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEquipmentComponent::Equip(UItem_Equipment* NewItem, ABaseCharacter* OwnerCharacter)
{
	if (!NewItem || !OwnerCharacter) return;
	EEquiptype type = NewItem->EquipType;
	if (EquippedItems.Contains(type))
	{
		UnEquip(type, OwnerCharacter);
	}
	switch (type)
	{
	case EEquiptype::EQ_weapon:
		OwnerCharacter->ServerAddAttack(NewItem->AttackPlus);
		break;
	case EEquiptype::EQ_armor:
		OwnerCharacter->ServerAddDefense(NewItem->DefensePlus);
		break;
	case EEquiptype::EQ_head:
		OwnerCharacter->ServerAddDefense(NewItem->DefensePlus);
		break;
	}
	EquippedItems.Add(type, NewItem);
	OnEquipmentChanged.Broadcast();
}

void UEquipmentComponent::UnEquip(EEquiptype EquipType, ABaseCharacter* OwnerCharacter)
{
	if (!EquippedItems.Contains(EquipType) || !OwnerCharacter) return;
	UItem_Equipment* equippedItem = EquippedItems[EquipType];
	switch (EquipType)
	{
	case EEquiptype::EQ_weapon:
		OwnerCharacter->ServerAddAttack(-equippedItem->AttackPlus);
		break;
	case EEquiptype::EQ_armor:
		OwnerCharacter->ServerAddDefense(-equippedItem->DefensePlus);
		break;
	case EEquiptype::EQ_head:
		OwnerCharacter->ServerAddDefense(-equippedItem->DefensePlus);
		break;
	}
	EquippedItems.Remove(EquipType);
	OwnerCharacter->InventoryComponent->AddItem(equippedItem, 1);
}

UItem_Equipment* UEquipmentComponent::GetEquipped(EEquiptype Type) const
{
	return EquippedItems.FindRef(Type);
}
