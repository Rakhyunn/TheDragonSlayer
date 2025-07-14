#include "ShopItemSlotWidget.h"
#include "BaseItem.h"
#include "BaseCharacter.h"
#include "BaseMerchantNPC.h"
#include "InventoryComponent.h"
#include "BaseStatComponent.h"
#include "NPCShopWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ItemToolTipWidget.h"

void UShopItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Buy)
	{
		BTN_Buy->OnClicked.AddDynamic(this, &UShopItemSlotWidget::OnBuyButtonClicked);
	}
}

void UShopItemSlotWidget::Init(UBaseItem* item, ABaseCharacter* player, ABaseMerchantNPC* merchant, UNPCShopWidget* shopWidget)
{
	slotItem = item;
	playerRef = player;
	merchantRef = merchant;
	shopWidgetRef = shopWidget;
	if (slotItem)
	{
		if (IMG_Icon)
		{
			IMG_Icon->SetBrushFromTexture(slotItem->icon);
		}
		if (TXT_Name)
		{
			TXT_Name->SetText(slotItem->itemName);
		}
		if (TXT_Price)
		{
			TXT_Price->SetText(FText::AsNumber(slotItem->purchaseMoney));
		}
	}
}

void UShopItemSlotWidget::OnBuyButtonClicked()
{
	if (!playerRef || !slotItem || !merchantRef) return;

	int32 cost = slotItem->purchaseMoney;
	int32 quantity = FCString::Atoi(*ET_Quantity->GetText().ToString());
	if (quantity <= 0)
		return;	// 추후에 UI 팝업으로 구매 실패?
	int32 totalCost = cost * quantity;
	if (playerRef->stat->GetMoney() >= totalCost)
	{
		playerRef->stat->AddMoney(-totalCost);
		playerRef->InventoryComponent->AddItem(slotItem, quantity);
		shopWidgetRef->UpdateMoney();
		UE_LOG(LogTemp, Log, TEXT("아이템 구매: %s"), *slotItem->itemName.ToString());
	}
	else
	{
		// 구매 실패 UI 팝업?
		UE_LOG(LogTemp, Warning, TEXT("소지금 부족"));
	}
}

void UShopItemSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (!slotItem || !ToolTipWidgetClass) return;
	UItemToolTipWidget* TooltipWidget = CreateWidget<UItemToolTipWidget>(GetWorld(), ToolTipWidgetClass);
	if (!TooltipWidget) return;
	TooltipWidget->InitTooltip(slotItem);
	SetToolTip(TooltipWidget);
}

void UShopItemSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SetToolTip(nullptr);
}
