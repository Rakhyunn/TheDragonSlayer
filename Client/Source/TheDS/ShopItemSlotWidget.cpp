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

void UShopItemSlotWidget::Init(UBaseItem* Item, ABaseCharacter* Player, ABaseMerchantNPC* Merchant, UNPCShopWidget* ShopWidget)
{
	SlotItem = Item;
	PlayerRef = Player;
	MerchantRef = Merchant;
	ShopWidgetRef = ShopWidget;
	if (SlotItem)
	{
		if (IMG_Icon)
		{
			IMG_Icon->SetBrushFromTexture(SlotItem->Icon);
		}
		if (TXT_Name)
		{
			TXT_Name->SetText(SlotItem->ItemName);
		}
		if (TXT_Price)
		{
			TXT_Price->SetText(FText::AsNumber(SlotItem->PurchaseMoney));
		}
	}
}

void UShopItemSlotWidget::OnBuyButtonClicked()
{
	if (!PlayerRef || !SlotItem || !MerchantRef) return;

	int32 Cost = SlotItem->PurchaseMoney;
	int32 Quantity = FCString::Atoi(*ET_Quantity->GetText().ToString());
	if (Quantity <= 0) return;
	int32 TotalCost = Cost * Quantity;
	if (PlayerRef->Stat->GetMoney() >= TotalCost)
	{
		PlayerRef->Stat->AddMoney(-TotalCost);
		PlayerRef->InventoryComponent->AddItem(SlotItem, Quantity);
		ShopWidgetRef->UpdateMoney();
		UE_LOG(LogTemp, Log, TEXT("아이템 구매: %s"), *SlotItem->ItemName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("소지금 부족"));
	}
}

void UShopItemSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (!SlotItem || !ToolTipWidgetClass) return;
	UItemToolTipWidget* TooltipWidget = CreateWidget<UItemToolTipWidget>(GetWorld(), ToolTipWidgetClass);
	if (!TooltipWidget) return;
	TooltipWidget->InitTooltip(SlotItem);
	SetToolTip(TooltipWidget);
}

void UShopItemSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SetToolTip(nullptr);
}
