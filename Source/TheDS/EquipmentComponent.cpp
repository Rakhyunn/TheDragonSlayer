#include "EquipmentComponent.h"
#include "BaseCharacter.h"
#include "InventoryComponent.h"
#include "EquipmentWidget.h"

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEquipmentComponent::Equip(UItem_Equipment* newItem, ABaseCharacter* ownerCharacter)
{
	if (!newItem || !ownerCharacter) return;
	EEquiptype type = newItem->equipType;
	if (equippedItems.Contains(type))
	{
		UnEquip(type, ownerCharacter);
	}
	switch (type)
	{
	case EEquiptype::EQ_weapon:
		ownerCharacter->ServerAddAttack(newItem->attackPlus);
		break;
	case EEquiptype::EQ_armor:
		ownerCharacter->ServerAddDefense(newItem->defensePlus);
		break;
	case EEquiptype::EQ_head:
		ownerCharacter->ServerAddDefense(newItem->defensePlus);
		break;
	}
	equippedItems.Add(type, newItem);
	OnEquipmentChanged.Broadcast();
}

void UEquipmentComponent::UnEquip(EEquiptype equipType, ABaseCharacter* ownerCharacter)
{
	if (!equippedItems.Contains(equipType) || !ownerCharacter) return;
	UItem_Equipment* equippedItem = equippedItems[equipType];
	switch (equipType)
	{
	case EEquiptype::EQ_weapon:
		ownerCharacter->ServerAddAttack(-equippedItem->attackPlus);
		break;
	case EEquiptype::EQ_armor:
		ownerCharacter->ServerAddDefense(-equippedItem->defensePlus);
		break;
	case EEquiptype::EQ_head:
		ownerCharacter->ServerAddDefense(-equippedItem->defensePlus);
		break;
	}
	equippedItems.Remove(equipType);
	ownerCharacter->InventoryComponent->AddItem(equippedItem, 1);
}

UItem_Equipment* UEquipmentComponent::GetEquipped(EEquiptype type) const
{
	return equippedItems.FindRef(type);
}
