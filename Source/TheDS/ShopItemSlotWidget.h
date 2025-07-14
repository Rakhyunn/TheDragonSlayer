#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopItemSlotWidget.generated.h"

UCLASS()
class THEDS_API UShopItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	class UBaseItem* slotItem;

	class ABaseCharacter* playerRef;

	class ABaseMerchantNPC* merchantRef;

	class UNPCShopWidget* shopWidgetRef;

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* IMG_Icon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_Name;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_Price;

	UPROPERTY(meta = (BindWidget))
	class UEditableText* ET_Quantity;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Buy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UItemToolTipWidget> ToolTipWidgetClass;
	
public:
	virtual void NativeConstruct() override;

	void Init(class UBaseItem* item, class ABaseCharacter* player, class ABaseMerchantNPC* merchant, UNPCShopWidget* shopWidget);

	UFUNCTION()
	void OnBuyButtonClicked();

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
