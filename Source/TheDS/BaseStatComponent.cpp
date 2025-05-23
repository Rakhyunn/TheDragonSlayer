#include "BaseStatComponent.h"
#include "Net/UnrealNetwork.h"

UBaseStatComponent::UBaseStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetLevel(1);
}

void UBaseStatComponent::SetLevel(int32 newLevel)
{
	level = newLevel;
	maxHP = 100.f + (level * 20.f);
	maxMP = 50.f + (level * 10.f);
	attack = 10.f + (level * 5.f);
	defense = 5.f + (level * 3.f);
	maxEXP = 100 + (level * 50);

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