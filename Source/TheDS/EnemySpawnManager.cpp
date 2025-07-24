#include "EnemySpawnManager.h"
#include "BaseEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

AEnemySpawnManager::AEnemySpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
}

void AEnemySpawnManager::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) return;
	// 초기화
	for (const FEnemySpawnData& Config : EnemySpawnConfigs)
	{
		CurrentEnemyCounts.Add(Config.EnemyClass, 0);
		for (int32 i = 0; i < Config.MaxCount; ++i)
		{
			SpawnEnemy(Config);
		}
	}
	// 주기적 리스폰 체크 시작
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AEnemySpawnManager::CheckAndRespawnAllEnemies, RespawnDelay, true);
}

void AEnemySpawnManager::NotifyEnemyDied(TSubclassOf<ABaseEnemyCharacter> EnemyClass)
{
	if (CurrentEnemyCounts.Contains(EnemyClass))
	{
		CurrentEnemyCounts[EnemyClass]--;
	}
}

void AEnemySpawnManager::CheckAndRespawnAllEnemies()
{
	for (const FEnemySpawnData& Config : EnemySpawnConfigs)
	{
		int32& CurrentCount = CurrentEnemyCounts.FindOrAdd(Config.EnemyClass);
		int32 ToSpawn = Config.MaxCount - CurrentCount;
		for (int32 i = 0; i < ToSpawn; ++i)
		{
			SpawnEnemy(Config);
		}
	}
}

void AEnemySpawnManager::SpawnEnemy(const FEnemySpawnData& Config)
{
	if (!HasAuthority()) return;
	if (Config.SpawnPoints.Num() == 0) return;

	AActor* SpawnPoint = Config.SpawnPoints[FMath::RandRange(0, Config.SpawnPoints.Num() - 1)];
	FVector Location = SpawnPoint->GetActorLocation();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABaseEnemyCharacter* Enemy = GetWorld()->SpawnActor<ABaseEnemyCharacter>(Config.EnemyClass, Location, FRotator::ZeroRotator, Params);
	if (Enemy)
	{
		CurrentEnemyCounts[Config.EnemyClass]++;
	}
}