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
	UButton* BTN_Item;

	UPROPERTY(meta = (BindWidget))
	UImage* IMG_Icon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_Quantity;

	UPROPERTY()
	class UInventoryComponent* owningInventory;

	int32 slotIndex = -1;

public:
	virtual void NativeConstruct() override;

	// 외부에서 슬롯 정보 설정
	void Init(const FInventorySlot& inSlotData, UInventoryComponent* inInventory, int32 inIndex);

	UFUNCTION()
	void OnItemClicked();
};
