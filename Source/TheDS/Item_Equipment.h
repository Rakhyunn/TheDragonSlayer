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
	EEquiptype equipType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	float attackPlus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	float defensePlus;
};
