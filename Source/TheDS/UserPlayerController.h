#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UserPlayerController.generated.h"

class UPlayerInfoWidget;
class UBaseStatComponent;
class UInventoryWidget;
class UEquipmentWidget;
class UNPCShopWidget

UCLASS()
class THEDS_API AUserPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AUserPlayerController();

	void ToggleInventory();

	void ToggleEquipment();

	void OpenShop(class ABaseMerchantNPC* Merchant);
	
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerInfoWidget> PlayerInfoWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UEquipmentWidget> EquipmentWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Merchant")
	TSubclassOf<UNPCShopWidget> ShopWidgetClass;

private:
	UPROPERTY()
	UPlayerInfoWidget* PlayerInfoWidgetInstance;

	UPROPERTY()
	UInventoryWidget* InventoryWidgetInstance;

	UPROPERTY()
	UEquipmentWidget* EquipmentWidgetInstance;

	UPROPERTY()
	UNPCShopWidget* ShopWidgetInstance;
};
