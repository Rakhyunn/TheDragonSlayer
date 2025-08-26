#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RespawnDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FRespawnData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Respawn")
	FName Field;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Respawn")
	FName Village;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Respawn")
	FTransform VillageSpawn;
};

UCLASS()
class THEDS_API URespawnDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Respawn")
	TArray<FRespawnData> Map;

public:
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	bool FindVillageByField(FName InField, FName& FindVillage, FTransform& FindSpawn) const
	{
		for (const FRespawnData& Data : Map)
		{
			if (Data.Field == InField)
			{
				FindVillage = Data.Village;
				FindSpawn = Data.VillageSpawn;
				return true;
			}
		}
		return false;
	}
};
