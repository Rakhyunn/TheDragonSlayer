#include "BaseStatComponent.h"
#include "Net/UnrealNetwork.h"

UBaseStatComponent::UBaseStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UBaseStatComponent::SetLevel(int32 newLevel)
{
	level = newLevel;
	maxHP = maxHP + (level * 20.f);
	maxMP = maxMP + (level * 10.f);
	attack = attack + (level * 5.f);
	magic = magic + (level * 5.f);
	defense = defense + (level * 3.f);
	maxEXP = maxEXP + (level * 50.f);

	currentHP = maxHP;
	currentMP = maxMP;
	currentEXP = 0.f;
	OnRep_EXPChanged();
	OnRep_HPChanged();
	OnRep_MPChanged();
	OnRep_LevelChanged();
	OnRep_AttackChanged();
	OnRep_MagicChanged();
	OnRep_DefenseChanged();
}

void UBaseStatComponent::GetDamage(float damageAmount)
{
	float ActualDamage = damageAmount - defense;
	ActualDamage = FMath::Max(ActualDamage, 1.f);
	currentHP = FMath::Clamp(currentHP - ActualDamage, 0.f, maxHP);
	OnRep_HPChanged();
}

void UBaseStatComponent::RestoreHP(float amount)
{
	currentHP = FMath::Min(currentHP + amount, maxHP);
	UE_LOG(LogTemp, Warning, TEXT("HP: %f"), currentHP);
	OnRep_HPChanged();
}

void UBaseStatComponent::RestoreMP(float amount)
{
	currentMP = FMath::Min(currentMP + amount, maxMP);
	UE_LOG(LogTemp, Warning, TEXT("MP: %f"), currentMP);
	OnRep_MPChanged();
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
	OnRep_EXPChanged();
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

void UBaseStatComponent::AddAttack(float plusAttack)
{
	if (attack + plusAttack <= 0)
		attack = 0;
	else
		attack += plusAttack;
	OnRep_AttackChanged();
}

void UBaseStatComponent::AddDefense(float plusDefense)
{
	if (defense + plusDefense <= 0)
		defense = 0;
	else
		defense += plusDefense;
	OnRep_DefenseChanged();
}

void UBaseStatComponent::OnRep_EXPChanged()
{
	// 경험치 UI 갱신
	float percent = currentEXP / maxEXP;
	OnEXPChangedDelegate.Broadcast(percent);
}

void UBaseStatComponent::OnRep_HPChanged()
{
	// HP UI 갱신
	float percent = currentHP / maxHP;
	OnHPChangedDelegate.Broadcast(percent);
}

void UBaseStatComponent::OnRep_MPChanged()
{
	// MP UI 갱신
	float percent = currentMP / maxMP;
	OnMPChangedDelegate.Broadcast(percent);
}

void UBaseStatComponent::OnRep_LevelChanged()
{
	// Level UI 갱신
	OnLevelChangedDelegate.Broadcast(level);
}

void UBaseStatComponent::OnRep_AttackChanged()
{
	OnAttackChangedDelegate.Broadcast(attack);
}

void UBaseStatComponent::OnRep_MagicChanged()
{
	OnMagicChangedDelegate.Broadcast(magic);
}

void UBaseStatComponent::OnRep_DefenseChanged()
{
	OnDefenseChangedDelegate.Broadcast(defense);
}

void UBaseStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBaseStatComponent, level);
	DOREPLIFETIME(UBaseStatComponent, currentHP);
	DOREPLIFETIME(UBaseStatComponent, currentMP);
	DOREPLIFETIME(UBaseStatComponent, currentEXP);
	DOREPLIFETIME(UBaseStatComponent, attack);
	DOREPLIFETIME(UBaseStatComponent, magic);
	DOREPLIFETIME(UBaseStatComponent, defense);
}

void UBaseStatComponent::BeginPlay()
{
	Super::BeginPlay();
}