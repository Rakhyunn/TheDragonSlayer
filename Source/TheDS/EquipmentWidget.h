#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentWidget.generated.h"

UCLASS()
class THEDS_API UEquipmentWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Close;

	UPROPERTY(meta = (BindWidget))
	class UEquipmentSlotWidget* Slot_Weapon;

	UPROPERTY(meta = (BindWidget))
	class UEquipmentSlotWidget* Slot_Armor;

	UPROPERTY(meta = (BindWidget))
	class UEquipmentSlotWidget* Slot_Head;

	UPROPERTY()
	class UEquipmentComponent* Equipment;

	UPROPERTY()
	class ABaseCharacter* OwnerCharacter;

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnCloseBtnCLicked();

	void SetEquipmentReference(class UEquipmentComponent* Ref, ABaseCharacter* Owner);

	void RefreshEquipment();
};
