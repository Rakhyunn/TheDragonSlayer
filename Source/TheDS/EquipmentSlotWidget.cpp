#include "EquipmentSlotWidget.h"
#include "Components/Image.h"

void UEquipmentSlotWidget::Init(EEquiptype inSlotType, UItem_Equipment* equippedItem)
{
	slotType = inSlotType;
	currentItem = equippedItem;

	if (IMG_ItemIcon && IMG_BasicIcon)
	{
		if (equippedItem && equippedItem->icon)
		{
			IMG_ItemIcon->SetBrushFromTexture(equippedItem->icon);
			IMG_ItemIcon->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
			IMG_BasicIcon->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
		}
		else
		{
			IMG_ItemIcon->SetBrushFromTexture(nullptr);
			IMG_ItemIcon->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
			IMG_BasicIcon->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.3f));
		}
	}
}

FReply UEquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnUnEquipRequested.Broadcast(slotType);
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
