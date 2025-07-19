#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_equipment,
	IT_comsume,
};

UCLASS(BlueprintType)
class THEDS_API UBaseItem : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Item")
	EItemType itemType;
	UPROPERTY(EditAnywhere, Category = "Item")
	FText itemName;
	UPROPERTY(EditAnywhere, Category = "Item")
	UTexture2D* icon;
	UPROPERTY(EditAnywhere, Category = "Item")
	FText itemDescription;
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 maxStack = 1;
	UPROPERTY(EditAnywhere, Category = "Item")
	bool bUsable = false;
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 purchaseMoney;
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 saleMoney;
	UPROPERTY(EditAnywhere, Category = "Visual")
	UStaticMesh* DroppedMesh;
};
