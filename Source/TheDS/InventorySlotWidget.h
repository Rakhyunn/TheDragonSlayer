#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "InventorySlotWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;

UCLASS()
class THEDS_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	FInventorySlot slotData;

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* IMG_Icon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_Quantity;

	UPROPERTY()
	class UInventoryComponent* owningInventory;

	int32 slotIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UItemToolTipWidget> ToolTipWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UItemSelectMenuWidget> ItemSelectMenuWidgetClass;

public:
	virtual void NativeConstruct() override;

	// 외부에서 슬롯 정보 설정
	void Init(const FInventorySlot& inSlotData, UInventoryComponent* inInventory);

	int32 GetSlotIndex() const;

	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
