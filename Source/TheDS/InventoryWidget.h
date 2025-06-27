#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseItem.h"
#include "InventoryComponent.h"
#include "InventoryWidget.generated.h"

UCLASS()
class THEDS_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Equipment;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Consume;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Close;

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* WrapBox_Items;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_Money;

	UPROPERTY()
	UInventoryComponent* inventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UInventorySlotWidget> slotWidgetClass;

	EItemType currentTab = EItemType::IT_comsume;

	UPROPERTY()
	class ABaseCharacter* player;

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnEquipmentBtnClicked();

	UFUNCTION()
	void OnConsumeBtnClicked();

	UFUNCTION()
	void OnCloseBtnCLicked();

	UFUNCTION()
	void UpdateMoney(int32 newMoney);

	void RefreshInventory();

	void SetInventoryReference(UInventoryComponent* ref);
};
