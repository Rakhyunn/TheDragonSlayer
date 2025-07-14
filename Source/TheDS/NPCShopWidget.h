#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCShopWidget.generated.h"

UCLASS()
class THEDS_API UNPCShopWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget))
	class UWrapBox* WB_ShopItems;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_PlayerMoney;

	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Close;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UShopItemSlotWidget> ShopSlotWidgetClass;

	UPROPERTY()
	class ABaseMerchantNPC* MerchantRef;

	UPROPERTY()
	class ABaseCharacter* PlayerRef;
	
public:
	virtual void NativeConstruct() override;

	void InitShop(class ABaseMerchantNPC* merchant);

	void RefreshShopItems();
	
	void UpdateMoney();

	UFUNCTION()
	void OnCloseBtnCLicked();
};
