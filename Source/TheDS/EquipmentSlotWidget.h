#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item_Equipment.h"
#include "EquipmentSlotWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnEquipRequested, EEquiptype);

UCLASS()
class THEDS_API UEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(EEquiptype inSlotType, UItem_Equipment* equippedItem);

	FOnUnEquipRequested OnUnEquipRequested;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* IMG_ItemIcon;

	UPROPERTY(meta = (BindWidget))
	class UImage* IMG_BasicIcon;

private:
	EEquiptype slotType;
	UItem_Equipment* currentItem;
};
