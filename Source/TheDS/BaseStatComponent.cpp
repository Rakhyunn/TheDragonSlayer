#include "BaseStatComponent.h"
#include "Net/UnrealNetwork.h"

UBaseStatComponent::UBaseStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBaseStatComponent::SetLevel(int32 newLevel)
{
	level = newLevel;
	maxHP = maxHP + (level * 20.f);
	maxMP = maxMP + (level * 10.f);
	attack = attack + (level * 5.f);
	magic = magic + (level * 5.f);
	defense = defense + (level * 3.f);
	maxEXP = maxEXP + (level * 50);

	currentHP = maxHP;
	currentMP = maxMP;
	currentEXP = 0;
}

void UBaseStatComponent::GetDamage(float damageAmount)
{
	float ActualDamage = damageAmount - defense;
	ActualDamage = FMath::Max(ActualDamage, 1.f);
	currentHP = FMath::Clamp(currentHP - ActualDamage, 0.f, maxHP);
}

void UBaseStatComponent::RestoreHP(float amount)
{
	currentHP = FMath::Min(currentHP + amount, maxHP);
}

void UBaseStatComponent::RestoreMP(float amount)
{
	currentMP = FMath::Min(currentMP + amount, maxMP);
}

void UBaseStatComponent::AddExperience(int32 amount)
{
	UE_LOG(LogTemp, Warning, TEXT("Get EXP: %d"), amount);
	currentEXP += amount;
	if (CanLevelUp()) 
	{
		if (level >= 15)
		{
			currentEXP = maxEXP;
			return;
		}
		LevelUp();
	}
	OnRep_ExperienceChanged();
}

bool UBaseStatComponent::CanLevelUp()
{
	return maxEXP <= currentEXP;
}

void UBaseStatComponent::LevelUp()
{
	SetLevel(level + 1);
	UE_LOG(LogTemp, Log, TEXT("레벨 업! 현재 레벨: %d"), level);
}

void UBaseStatComponent::OnRep_ExperienceChanged()
{
	// 경험치 UI 갱신
}

void UBaseStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBaseStatComponent, level);
	DOREPLIFETIME(UBaseStatComponent, currentHP);
	DOREPLIFETIME(UBaseStatComponent, currentMP);
	DOREPLIFETIME(UBaseStatComponent, currentEXP);
}

void UBaseStatComponent::BeginPlay()
{
	Super::BeginPlay();
}