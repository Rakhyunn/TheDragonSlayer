#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_equipment,
	IT_comsume
};

UCLASS(BlueprintType)
class THEDS_API UBaseItem : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Item")
	EItemType ItemType;
	UPROPERTY(EditAnywhere, Category = "Item")
	FText ItemName;
	UPROPERTY(EditAnywhere, Category = "Item")
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere, Category = "Item")
	FText ItemDescription;
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 MaxStack = 1;
	UPROPERTY(EditAnywhere, Category = "Item")
	bool bUsable = false;
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 PurchaseMoney;
	UPROPERTY(EditAnywhere, Category = "Item")
	int32 SaleMoney;
	UPROPERTY(EditAnywhere, Category = "Visual")
	UStaticMesh* DroppedMesh;
};
