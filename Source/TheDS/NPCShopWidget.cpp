#include "NPCShopWidget.h"
#include "BaseMerchantNPC.h"
#include "BaseCharacter.h"
#include "BaseItem.h"
#include "BaseStatComponent.h"
#include "ShopItemSlotWidget.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"

void UNPCShopWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BTN_Close) BTN_Close->OnClicked.AddDynamic(this, &UNPCShopWidget::OnCloseBtnCLicked);
}

void UNPCShopWidget::InitShop(ABaseMerchantNPC* merchant)
{
	MerchantRef = merchant;
	PlayerRef = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	RefreshShopItems();
	UpdateMoney();
}

void UNPCShopWidget::RefreshShopItems()
{
	if (!WB_ShopItems || !MerchantRef || !ShopSlotWidgetClass) return;
	WB_ShopItems->ClearChildren();
	for (UBaseItem* item : MerchantRef->GetItemsForSale())
	{
		UShopItemSlotWidget* slot = CreateWidget< UShopItemSlotWidget>(this, ShopSlotWidgetClass);
		if (slot)
		{
			slot->Init(item, PlayerRef, MerchantRef);
			WB_ShopItems->AddChildToWrapBox(slot);
		}
	}
}

void UNPCShopWidget::UpdateMoney()
{
	if (PlayerRef && PlayerRef->stat && TXT_PlayerMoney)
	{
		TXT_PlayerMoney->SetText(FText::AsNumber(PlayerRef->stat->GetMoney()));
	}
}

void UNPCShopWidget::OnCloseBtnCLicked()
{
	RemoveFromParent();
}
