#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item_Equipment.h"
#include "EquipmentComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnEquipmentChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEDS_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<EEquiptype, UItem_Equipment*> equippedItems;

	FOnEquipmentChanged OnEquipmentChanged;

public:	
	UEquipmentComponent();

	void Equip(UItem_Equipment* newItem, class ABaseCharacter* ownerCharacter);

	void UnEquip(EEquiptype equipType, class ABaseCharacter* ownerCharacter);

	UItem_Equipment* GetEquipped(EEquiptype type) const;
};
