#include "ItemToolTipWidget.h"
#include "BaseItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemToolTipWidget::InitTooltip(UBaseItem* itemData)
{
	if (!itemData) return;

	if (IMG_Icon) IMG_Icon->SetBrushFromTexture(itemData->icon);
	if (TXT_Name) TXT_Name->SetText(itemData->itemName);
	if (TXT_Description) TXT_Description->SetText(itemData->itemDescription);
}
