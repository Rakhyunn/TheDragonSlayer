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

void UNPCShopWidget::InitShop(ABaseMerchantNPC* Merchant)
{
	MerchantRef = Merchant;
	PlayerRef = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	RefreshShopItems();
	UpdateMoney();
}

void UNPCShopWidget::RefreshShopItems()
{
	if (!WB_ShopItems || !MerchantRef || !ShopSlotWidgetClass) return;
	WB_ShopItems->ClearChildren();
	for (UBaseItem* Item : MerchantRef->GetItemsForSale())
	{
		UShopItemSlotWidget* Slot_ = CreateWidget< UShopItemSlotWidget>(this, ShopSlotWidgetClass);
		if (Slot_)
		{
			Slot_->Init(Item, PlayerRef, MerchantRef, this);
			WB_ShopItems->AddChildToWrapBox(Slot_);
		}
	}
}

void UNPCShopWidget::UpdateMoney()
{
	if (PlayerRef && PlayerRef->Stat && TXT_PlayerMoney)
	{
		TXT_PlayerMoney->SetText(FText::AsNumber(PlayerRef->Stat->GetMoney()));
	}
}

void UNPCShopWidget::OnCloseBtnCLicked()
{
	RemoveFromParent();
}
