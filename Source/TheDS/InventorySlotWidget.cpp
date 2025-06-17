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

void UInventorySlotWidget::Init(const FInventorySlot& inSlotData, UInventoryComponent* inInventory)
{
	slotData = inSlotData;
	owningInventory = inInventory;
	slotIndex = inSlotData.OriginalIndex;
	if (!slotData.ItemData)
	{
		// 빈 슬롯 처리
		IMG_Icon->SetVisibility(ESlateVisibility::Hidden);
		TXT_Quantity->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
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

int32 UInventorySlotWidget::GetSlotIndex() const
{
	return slotIndex;
}

FReply UInventorySlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	if (!owningInventory || slotIndex < 0 || !slotData.ItemData) return FReply::Unhandled();

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return FReply::Unhandled();

	ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(PC->GetPawn());
	if (!TargetCharacter) return FReply::Unhandled();

	owningInventory->UseItem(slotData.ItemData->itemType, slotIndex, TargetCharacter);
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
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && slotData.ItemData)
	{
		UItemSelectMenuWidget* Menu = CreateWidget<UItemSelectMenuWidget>(GetWorld(), ItemSelectMenuWidgetClass);
		if (Menu)
		{
			Menu->Setup(slotData, owningInventory);
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
	if (!slotData.ItemData) return;

	UItemDragDropOperation* DragOperation = NewObject<UItemDragDropOperation>();
	DragOperation->DefaultDragVisual = this; // or a new small icon widget
	DragOperation->Pivot = EDragPivot::MouseDown;
	DragOperation->SourceSlot = this;
	DragOperation->DraggedItemData = slotData;

	OutOperation = DragOperation;
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	UItemDragDropOperation* DragOp = Cast<UItemDragDropOperation>(InOperation);
	if (!DragOp || !owningInventory || !DragOp->DraggedItemData.ItemData)
		return false;
	if (DragOp->SourceSlot == this)
		return false;
	owningInventory->SwapItem(
		DragOp->DraggedItemData.ItemData->itemType,
		DragOp->SourceSlot->GetSlotIndex(),
		this->GetSlotIndex());
	return true;
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (!slotData.ItemData || !ToolTipWidgetClass) return;
	UItemToolTipWidget* TooltipWidget = CreateWidget<UItemToolTipWidget>(GetWorld(), ToolTipWidgetClass);
	if (!TooltipWidget) return;
	TooltipWidget->InitTooltip(slotData.ItemData);
	// UUserWidget은 ToolTipContent로 설정해야 한다.
	SetToolTip(TooltipWidget);
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SetToolTip(nullptr);
}
