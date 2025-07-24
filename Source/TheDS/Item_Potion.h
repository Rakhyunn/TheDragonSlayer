#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Item_Potion.generated.h"

UENUM(BlueprintType)
enum class EPotionType : uint8
{
	PO_hp,
	PO_mp,
	PO_all,
};

UCLASS(BlueprintType)
class THEDS_API UItem_Potion : public UBaseItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	EPotionType potionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	float restoreHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	float restoreMP;
};
