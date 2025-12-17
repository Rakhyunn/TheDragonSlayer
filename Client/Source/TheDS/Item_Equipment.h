#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Item_Equipment.generated.h"

UENUM(BlueprintType)
enum class EEquiptype : uint8
{
	EQ_weapon,
	EQ_armor,
	EQ_head,
};

UCLASS(BlueprintType)
class THEDS_API UItem_Equipment : public UBaseItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EEquiptype EquipType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	float AttackPlus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	float DefensePlus;
};
