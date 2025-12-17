#include "ItemToolTipWidget.h"
#include "BaseItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemToolTipWidget::InitTooltip(UBaseItem* ItemData)
{
	if (!ItemData) return;

	if (IMG_Icon) IMG_Icon->SetBrushFromTexture(ItemData->Icon);
	if (TXT_Name) TXT_Name->SetText(ItemData->ItemName);
	if (TXT_Description) TXT_Description->SetText(ItemData->ItemDescription);
}
