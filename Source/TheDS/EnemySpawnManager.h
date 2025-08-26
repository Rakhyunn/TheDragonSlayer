#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnManager.generated.h"

USTRUCT(BlueprintType)
struct FEnemySpawnData
{
    GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class ABaseEnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 MaxCount;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<AActor*> SpawnPoints;
};

UCLASS()
class THEDS_API AEnemySpawnManager : public AActor
{
	GENERATED_BODY()

protected:
	// 리스폰 주기 타이머
	FTimerHandle RespawnTimerHandle;

	// 현재 적 수 추적용
	UPROPERTY()
	TMap<TSubclassOf<ABaseEnemyCharacter>, int32> CurrentEnemyCounts;

public:
	UPROPERTY(EditAnywhere, Category = "Spawn Config")
	TArray<FEnemySpawnData> EnemySpawnConfigs;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float RespawnDelay = 10.f;

public:	
	AEnemySpawnManager();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 적 사망 시 호출
	void NotifyEnemyDied(TSubclassOf<ABaseEnemyCharacter> EnemyClass);

	// 주기적으로 호출
	void CheckAndRespawnAllEnemies();

	void SpawnEnemy(const FEnemySpawnData& Config);

private:
	FTimerHandle VisibilityTickHandle;

	bool bSpawning = false;

private:
	void TickLevelVisibility();

	void TrySpawnTick();

	void StopRespawnTimer();

public:
	UFUNCTION(BlueprintCallable)
	void ActivateSpawning();

	UFUNCTION(BlueprintCallable)
	void DeactivateSpawning();
};
