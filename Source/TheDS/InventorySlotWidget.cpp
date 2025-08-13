#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "BaseItem.h"
#include "Components/Button.h"
#include "BaseCharacter.h"
#include "ItemDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ItemToolTipWidget.h"
#include "ItemSelectMenuWidget.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventorySlotWidget::Init(const FInventorySlot& InSlotData, UInventoryComponent* InInventory)
{
	SlotData = InSlotData;
	OwningInventory = InInventory;
	SlotIndex = InSlotData.OriginalIndex;
	if (!SlotData.ItemData)
	{
		// 빈 슬롯 처리
		IMG_Icon->SetVisibility(ESlateVisibility::Hidden);
		TXT_Quantity->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	if (SlotData.ItemData)
	{
		if (IMG_Icon && SlotData.ItemData->Icon)
		{
			IMG_Icon->SetBrushFromTexture(SlotData.ItemData->Icon);
		}
		// 장비면 텍스트 숨기기
		if (SlotData.ItemData->ItemType == EItemType::IT_equipment)
		{
			if (TXT_Quantity) TXT_Quantity->SetVisibility(ESlateVisibility::Collapsed);
		}
		// 소비 아이템 등은 텍스트 표시
		else
		{
			if (TXT_Quantity) TXT_Quantity->SetText(FText::AsNumber(SlotData.Quantity));
		}
	}
}

int32 UInventorySlotWidget::GetSlotIndex() const
{
	return SlotIndex;
}

FReply UInventorySlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	if (!OwningInventory || SlotIndex < 0 || !SlotData.ItemData) return FReply::Unhandled();

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return FReply::Unhandled();

	ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(PC->GetPawn());
	if (!TargetCharacter) return FReply::Unhandled();

	OwningInventory->UseItem(SlotData.ItemData->ItemType, SlotIndex, TargetCharacter);
	return FReply::Handled();
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		UE_LOG(LogTemp, Warning, TEXT("MouseButtonDown detected"));
		Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && SlotData.ItemData)
	{
		UItemSelectMenuWidget* Menu = CreateWidget<UItemSelectMenuWidget>(GetWorld(), ItemSelectMenuWidgetClass);
		if (Menu)
		{
			Menu->Setup(SlotData, OwningInventory);
			Menu->AddToViewport();
			// 위치 조정 (커서 위치에 띄우기)
			Menu->SetPositionInViewport(InMouseEvent.GetScreenSpacePosition(), false);
		}
		return FReply::Handled();
	}
	return Reply;
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	if (!SlotData.ItemData) return;

	UItemDragDropOperation* DragOperation = NewObject<UItemDragDropOperation>();
	DragOperation->DefaultDragVisual = this; // or a new small icon widget
	DragOperation->Pivot = EDragPivot::MouseDown;
	DragOperation->SourceSlot = this;
	DragOperation->DraggedItemData = SlotData;

	OutOperation = DragOperation;
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	UItemDragDropOperation* DragOp = Cast<UItemDragDropOperation>(InOperation);
	if (!DragOp || !OwningInventory || !DragOp->DraggedItemData.ItemData)
		return false;
	if (DragOp->SourceSlot == this)
		return false;
	OwningInventory->SwapItem(
		DragOp->DraggedItemData.ItemData->ItemType,
		DragOp->SourceSlot->GetSlotIndex(),
		this->GetSlotIndex());
	return true;
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (!SlotData.ItemData || !ToolTipWidgetClass) return;
	UItemToolTipWidget* TooltipWidget = CreateWidget<UItemToolTipWidget>(GetWorld(), ToolTipWidgetClass);
	if (!TooltipWidget) return;
	TooltipWidget->InitTooltip(SlotData.ItemData);
	// UUserWidget은 ToolTipContent로 설정해야 한다.
	SetToolTip(TooltipWidget);
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SetToolTip(nullptr);
}
