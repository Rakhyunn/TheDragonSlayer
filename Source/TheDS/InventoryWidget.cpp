#include "InventoryWidget.h"
#include "InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
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
    if (!inventory || !inventoryGrid || !slotWidgetClass) return;
    // 기존 슬롯 위젯 제거
    inventoryGrid->ClearChildren();
    // 필터링된 슬롯 가져오기
    TArray<FInventorySlot> filteredSlots = inventory->GetFilteredSlots(currentTab);
    int32 col = 0;
    int32 row = 0;
    for (int32 i = 0; i < filteredSlots.Num(); i++)
    {
        const FInventorySlot& slotData = filteredSlots[i];

        UInventorySlotWidget* slotWidget = CreateWidget<UInventorySlotWidget>(this, slotWidgetClass);
        if (slotWidget)
        {
            slotWidget->Init(slotData, inventory, i); // 슬롯 데이터 전달
            inventoryGrid->AddChildToUniformGrid(slotWidget, row, col);
            col++;
            if (col >= 5) // 5개씩 한 줄에
            {
                col = 0;
                row++;
            }
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
