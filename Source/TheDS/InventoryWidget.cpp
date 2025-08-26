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
    Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());
    if (Player && Player->Stat)
    {
        if (!Player->Stat->OnMoneyChangedDelegate.IsBoundToObject(this))
        {
            Player->Stat->OnMoneyChangedDelegate.AddUObject(this, &UInventoryWidget::UpdateMoney);
        }
        UpdateMoney(Player->Stat->GetMoney()); // 초기화 시점 UI도 갱신
    }
}

void UInventoryWidget::OnEquipmentBtnClicked()
{
	CurrentTab = EItemType::IT_equipment;
	RefreshInventory();
}

void UInventoryWidget::OnConsumeBtnClicked()
{
	CurrentTab = EItemType::IT_comsume;
	RefreshInventory();
}

void UInventoryWidget::OnCloseBtnCLicked()
{
    RemoveFromParent();
}

void UInventoryWidget::UpdateMoney(int32 NewMoney)
{
    if (!TXT_Money)return;
    if (!Player || !Player->Stat) return;
    TXT_Money->SetText(FText::AsNumber(Player->Stat->GetMoney()));
}

void UInventoryWidget::RefreshInventory()
{
    if (!Inventory || !WrapBox_Items || !SlotWidgetClass) return;
    WrapBox_Items->ClearChildren();
    const TArray<FInventorySlot>& Slots = Inventory->GetSlots(CurrentTab);
    for (int32 i = 0; i < Slots.Num(); i++)
    {
        UInventorySlotWidget* slot = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
        if (!slot) continue;
        const FInventorySlot& slotData = Slots[i];
        if (slotData.ItemData)
        {
            FInventorySlot slotCopy = slotData;
            slotCopy.OriginalIndex = i; // 드래그/드롭 인덱스 유지
            slot->Init(slotCopy, Inventory);
        }
        else
        {
            FInventorySlot emptySlot;
            emptySlot.OriginalIndex = i; // 빈 슬롯에도 index 지정!
            slot->Init(emptySlot, Inventory);
        }
        WrapBox_Items->AddChildToWrapBox(slot);
    }
}

void UInventoryWidget::SetInventoryReference(UInventoryComponent* Ref)
{
    Inventory = Ref;
    if (Inventory)
    {
        Inventory->LinkedInventoryWidget = this;
    }
}
