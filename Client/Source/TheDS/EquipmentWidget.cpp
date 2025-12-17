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
				if (Equipment && OwnerCharacter)
				{
					Equipment->UnEquip(type, OwnerCharacter);
					this->RefreshEquipment(); // UI 갱신
				}
			});
	}
	if (Slot_Armor)
	{
		Slot_Armor->OnUnEquipRequested.AddLambda([this](EEquiptype type)
			{
				if (Equipment && OwnerCharacter)
				{
					Equipment->UnEquip(type, OwnerCharacter);
					this->RefreshEquipment();
				}
			});
	}
	if (Slot_Head)
	{
		Slot_Head->OnUnEquipRequested.AddLambda([this](EEquiptype type)
			{
				if (Equipment && OwnerCharacter)
				{
					Equipment->UnEquip(type, OwnerCharacter);
					this->RefreshEquipment();
				}
			});
	}
	if (Equipment) Equipment->OnEquipmentChanged.AddUObject(this, &UEquipmentWidget::RefreshEquipment);
}

void UEquipmentWidget::OnCloseBtnCLicked()
{
	RemoveFromParent();
}

void UEquipmentWidget::SetEquipmentReference(UEquipmentComponent* Ref, ABaseCharacter* Owner)
{
	Equipment = Ref;
	OwnerCharacter = Owner;
}

void UEquipmentWidget::RefreshEquipment()
{
	if (!Equipment) return;

	Slot_Weapon->Init(EEquiptype::EQ_weapon, Equipment->GetEquipped(EEquiptype::EQ_weapon));
	Slot_Armor->Init(EEquiptype::EQ_armor, Equipment->GetEquipped(EEquiptype::EQ_armor));
	Slot_Head->Init(EEquiptype::EQ_head, Equipment->GetEquipped(EEquiptype::EQ_head));
}
