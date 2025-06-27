#include "InventoryWidget.h"
#include "InventorySlotWidget.h"
#include "Components/WrapBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (BTN_Equipment) BTN_Equipment->OnClicked.AddDynamic(this, &UInventoryWidget::OnEquipmentBtnClicked);
    if (BTN_Consume) BTN_Consume->OnClicked.AddDynamic(this, &UInventoryWidget::OnConsumeBtnClicked);
    if (BTN_Close) BTN_Close->OnClicked.AddDynamic(this, &UInventoryWidget::OnCloseBtnCLicked);
    player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
    if (player && player->stat)
    {
        if (!player->stat->OnMoneyChangedDelegate.IsBoundToObject(this))
        {
            player->stat->OnMoneyChangedDelegate.AddUObject(this, &UInventoryWidget::UpdateMoney);
        }
        UpdateMoney(player->stat->GetMoney()); // 초기화 시점 UI도 갱신
    }
}

void UInventoryWidget::OnEquipmentBtnClicked()
{
	currentTab = EItemType::IT_equipment;
	RefreshInventory();
}

void UInventoryWidget::OnConsumeBtnClicked()
{
	currentTab = EItemType::IT_comsume;
	RefreshInventory();
}

void UInventoryWidget::OnCloseBtnCLicked()
{
    RemoveFromParent();
}

void UInventoryWidget::UpdateMoney(int32 newMoney)
{
    if (!TXT_Money)return;
    if (!player || !player->stat) return;
    TXT_Money->SetText(FText::AsNumber(player->stat->GetMoney()));
}

void UInventoryWidget::RefreshInventory()
{
    if (!inventory || !WrapBox_Items || !slotWidgetClass) return;
    WrapBox_Items->ClearChildren();
    const TArray<FInventorySlot>& Slots = inventory->GetSlots(currentTab);
    for (int32 i = 0; i < Slots.Num(); i++)
    {
        UInventorySlotWidget* slot = CreateWidget<UInventorySlotWidget>(this, slotWidgetClass);
        if (!slot) continue;
        const FInventorySlot& slotData = Slots[i];
        if (slotData.ItemData)
        {
            FInventorySlot slotCopy = slotData;
            slotCopy.OriginalIndex = i; // 드래그/드롭 인덱스 유지
            slot->Init(slotCopy, inventory);
        }
        else
        {
            FInventorySlot emptySlot;
            emptySlot.OriginalIndex = i; // 빈 슬롯에도 index 지정!
            slot->Init(emptySlot, inventory);
        }
        WrapBox_Items->AddChildToWrapBox(slot);
    }
}

void UInventoryWidget::SetInventoryReference(UInventoryComponent* ref)
{
    inventory = ref;
    if (inventory)
    {
        inventory->linkedInventoryWidget = this;
    }
}
