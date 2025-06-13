#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "BaseItem.h"
#include "Components/Button.h"
#include "BaseCharacter.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_Item) BTN_Item->OnClicked.AddDynamic(this, &UInventorySlotWidget::OnItemClicked);
}

void UInventorySlotWidget::Init(const FInventorySlot& inSlotData, UInventoryComponent* inInventory, int32 inIndex)
{
	slotData = inSlotData;
	slotData = inSlotData;
	owningInventory = inInventory;
	slotIndex = inIndex;
	if (!slotData.ItemData) return;
	if (slotData.ItemData)
	{
		if (IMG_Icon && slotData.ItemData->icon)
		{
			IMG_Icon->SetBrushFromTexture(slotData.ItemData->icon);
		}
		// 장비면 텍스트 숨기기
		if (slotData.ItemData->itemType == EItemType::IT_equipment)
		{
			if (TXT_Quantity) TXT_Quantity->SetVisibility(ESlateVisibility::Collapsed);
		}
		// 소비 아이템 등은 텍스트 표시
		else
		{
			if (TXT_Quantity) TXT_Quantity->SetText(FText::AsNumber(slotData.Quantity));
		}
	}
}

void UInventorySlotWidget::OnItemClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Item Selected"));
	if (!owningInventory || slotIndex < 0) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(PC->GetPawn());
	if (!TargetCharacter) return;
	for (int32 i = 0; i < owningInventory->AllSlots.Num(); ++i)
	{
		if (owningInventory->AllSlots[i].ItemData == slotData.ItemData)
		{
			owningInventory->UseItem(i, TargetCharacter);
			break;
		}
	}
}
