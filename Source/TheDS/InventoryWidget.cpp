#include "InventoryWidget.h"
#include "InventorySlotWidget.h"
#include "Components/WrapBox.h"
#include "Components/Button.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (BTN_Equipment) BTN_Equipment->OnClicked.AddDynamic(this, &UInventoryWidget::OnEquipmentBtnClicked);
    if (BTN_Consume) BTN_Consume->OnClicked.AddDynamic(this, &UInventoryWidget::OnConsumeBtnClicked);
    if (BTN_Close) BTN_Close->OnClicked.AddDynamic(this, &UInventoryWidget::OnCloseBtnCLicked);
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

void UInventoryWidget::RefreshInventory()
{
    if (!inventory || !WrapBox_Items || !slotWidgetClass) return;
    WrapBox_Items->ClearChildren();
    TArray<FInventorySlot> filteredSlots = inventory->GetFilteredSlots(currentTab);
    UE_LOG(LogTemp, Warning, TEXT("Slot: %d"), filteredSlots.Num());
    for (int32 i = 0; i < filteredSlots.Num(); i++)
    {
        UInventorySlotWidget* slot = CreateWidget<UInventorySlotWidget>(this, slotWidgetClass);
        if (slot)
        {
            slot->Init(filteredSlots[i], inventory);
            UE_LOG(LogTemp, Warning, TEXT("%d"), i);
            WrapBox_Items->AddChildToWrapBox(slot);
        }
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
