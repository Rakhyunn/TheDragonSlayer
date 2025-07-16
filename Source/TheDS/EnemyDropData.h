#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BaseItem.h"
#include "EnemyDropData.generated.h"

USTRUCT(BlueprintType)
struct FDropItemInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBaseItem* Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DropRate = 1.0f;  // 이 아이템이 드랍될 확률 (0~1)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1;
};

USTRUCT(BlueprintType)
struct FEnemyDropData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDropItemInfo> DropItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoneyDropRate = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinMoney = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxMoney = 50;
};