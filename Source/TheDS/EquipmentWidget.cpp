#include "EquipmentWidget.h"
#include "Components/Button.h"
#include "EquipmentComponent.h"
#include "Item_Equipment.h"
#include "EquipmentSlotWidget.h"
#include "BaseCharacter.h"

void UEquipmentWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_Close) BTN_Close->OnClicked.AddDynamic(this, &UEquipmentWidget::OnCloseBtnCLicked);
	if (Slot_Weapon)
	{
		Slot_Weapon->OnUnEquipRequested.AddLambda([this](EEquiptype type)
			{
				if (equipment && ownerCharacter)
				{
					equipment->UnEquip(type, ownerCharacter);
					this->RefreshEquipment(); // UI 갱신
				}
			});
	}
	if (Slot_Armor)
	{
		Slot_Armor->OnUnEquipRequested.AddLambda([this](EEquiptype type)
			{
				if (equipment && ownerCharacter)
				{
					equipment->UnEquip(type, ownerCharacter);
					this->RefreshEquipment();
				}
			});
	}
	if (Slot_Head)
	{
		Slot_Head->OnUnEquipRequested.AddLambda([this](EEquiptype type)
			{
				if (equipment && ownerCharacter)
				{
					equipment->UnEquip(type, ownerCharacter);
					this->RefreshEquipment();
				}
			});
	}
	if (equipment) equipment->OnEquipmentChanged.AddUObject(this, &UEquipmentWidget::RefreshEquipment);
}

void UEquipmentWidget::OnCloseBtnCLicked()
{
	RemoveFromParent();
}

void UEquipmentWidget::SetEquipmentReference(UEquipmentComponent* ref, ABaseCharacter* owner)
{
	equipment = ref;
	ownerCharacter = owner;
}

void UEquipmentWidget::RefreshEquipment()
{
	if (!equipment) return;

	Slot_Weapon->Init(EEquiptype::EQ_weapon, equipment->GetEquipped(EEquiptype::EQ_weapon));
	Slot_Armor->Init(EEquiptype::EQ_armor, equipment->GetEquipped(EEquiptype::EQ_armor));
	Slot_Head->Init(EEquiptype::EQ_head, equipment->GetEquipped(EEquiptype::EQ_head));
}
