#include "EnemySpawnManager.h"
#include "BaseEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"

AEnemySpawnManager::AEnemySpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
}

void AEnemySpawnManager::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(VisibilityTickHandle, this, &AEnemySpawnManager::TickLevelVisibility, 0.2f, true);
	}
}

void AEnemySpawnManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(VisibilityTickHandle);
		StopRespawnTimer();
	}
	Super::EndPlay(EndPlayReason);
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
	if (!HasAuthority() || !bSpawning) return;

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

	TArray<AActor*> SpawnPoints;
	ULevel* MyLevel = GetLevel();
	for (AActor* P : Config.SpawnPoints)
	{
		if (IsValid(P) && P->GetLevel() == MyLevel)
			SpawnPoints.Add(P);
	}
	if (SpawnPoints.Num() == 0) return;

	AActor* SpawnPoint = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];

	const FVector  Location = SpawnPoint->GetActorLocation();
	const FRotator Rotation = SpawnPoint->GetActorRotation();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABaseEnemyCharacter* Enemy = GetWorld()->SpawnActor<ABaseEnemyCharacter>(Config.EnemyClass, Location, FRotator::ZeroRotator, Params);
	if (Enemy)
	{
		CurrentEnemyCounts[Config.EnemyClass]++;
	}
}

void AEnemySpawnManager::TickLevelVisibility()
{
	if (!HasAuthority()) return;

	const ULevel* MyLevel = GetLevel();
	const bool bVisible = (MyLevel && MyLevel->bIsVisible);
	if (bVisible && !bSpawning)
	{
		ActivateSpawning();
	}
	else if (!bVisible && bSpawning)
	{
		DeactivateSpawning();
	}
}

void AEnemySpawnManager::TrySpawnTick()
{
	if (!bSpawning || !HasAuthority()) return;
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

void AEnemySpawnManager::StopRespawnTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
}

void AEnemySpawnManager::ActivateSpawning()
{
	bSpawning = true;
	TrySpawnTick();
}

void AEnemySpawnManager::DeactivateSpawning()
{
	bSpawning = false;
	StopRespawnTimer();
}
