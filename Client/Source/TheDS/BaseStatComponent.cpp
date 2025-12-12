#include "BaseStatComponent.h"
#include "Net/UnrealNetwork.h"

UBaseStatComponent::UBaseStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UBaseStatComponent::SetLevel(int32 newLevel)
{
	Level = newLevel;
	MaxHP = MaxHP + (Level * 20.f);
	MaxMP = MaxMP + (Level * 10.f);
	Attack = Attack + (Level * 5.f);
	Magic = Magic + (Level * 5.f);
	Defense = Defense + (Level * 3.f);
	MaxEXP = MaxEXP + (Level * 50.f);

	CurrentHP = MaxHP;
	CurrentMP = MaxMP;
	CurrentEXP = 0.f;
	OnRep_EXPChanged();
	OnRep_HPChanged();
	OnRep_MPChanged();
	OnRep_LevelChanged();
	OnRep_AttackChanged();
	OnRep_MagicChanged();
	OnRep_DefenseChanged();
}

void UBaseStatComponent::GetDamage(float DamageAmount)
{
	float ActualDamage = DamageAmount - Defense;
	ActualDamage = FMath::Max(ActualDamage, 1.f);
	CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.f, MaxHP);
	OnRep_HPChanged();
	if (CurrentHP <= 0.f)
	{
		ClearAllDots();
		OnDiedDelegate.Broadcast();
	}
}

void UBaseStatComponent::RestoreHP(float Amount)
{
	CurrentHP = FMath::Min(CurrentHP + Amount, MaxHP);
	UE_LOG(LogTemp, Warning, TEXT("HP: %f"), CurrentHP);
	OnRep_HPChanged();
}

void UBaseStatComponent::RestoreMP(float Amount)
{
	CurrentMP = FMath::Min(CurrentMP + Amount, MaxMP);
	UE_LOG(LogTemp, Warning, TEXT("MP: %f"), CurrentMP);
	OnRep_MPChanged();
}

void UBaseStatComponent::FullRestore()
{
	CurrentHP = MaxHP;
	CurrentMP = MaxMP;
	OnRep_HPChanged();
	OnRep_MPChanged();
}

void UBaseStatComponent::AddExperience(int32 Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("Get EXP: %d"), Amount);
	CurrentEXP += Amount;
	if (CanLevelUp()) 
	{
		if (Level >= 15)
		{
			CurrentEXP = MaxEXP;
			return;
		}
		LevelUp();
	}
	OnRep_EXPChanged();
}

bool UBaseStatComponent::CanLevelUp()
{
	return MaxEXP <= CurrentEXP;
}

void UBaseStatComponent::LevelUp()
{
	SetLevel(Level + 1);
	UE_LOG(LogTemp, Log, TEXT("레벨 업! 현재 레벨: %d"), Level);
}

void UBaseStatComponent::ApplyDiedPenalty()
{
	float Loss = CurrentEXP * 0.1;
	CurrentEXP = FMath::Max(0.f, CurrentEXP - Loss);
	OnRep_EXPChanged();
}

void UBaseStatComponent::AddAttack(float PlusAttack)
{
	if (Attack + PlusAttack <= 0)
		Attack = 0;
	else
		Attack += PlusAttack;
	OnRep_AttackChanged();
}

void UBaseStatComponent::AddMagic(float PlusMagic)
{
	if (Magic + PlusMagic <= 0)
		Magic = 0;
	else
		Magic += PlusMagic;
	OnRep_MagicChanged();
}

void UBaseStatComponent::AddDefense(float PlusDefense)
{
	if (Defense + PlusDefense <= 0)
		Defense = 0;
	else
		Defense += PlusDefense;
	OnRep_DefenseChanged();
}

void UBaseStatComponent::AddMoney(int32 Money)
{
	CurrentMoney += Money;
	OnRep_MoneyChanged();
}

void UBaseStatComponent::SpendMoney(int32 Money)
{
	if (CurrentMoney >= Money)
	{
		CurrentMoney -= Money;
		OnRep_MoneyChanged();
	}
	else
		return;
}

bool UBaseStatComponent::CheckMoney(int32 Money)
{
	if (CurrentMoney >= Money) return true;
	else return false;
}

void UBaseStatComponent::OnRep_EXPChanged()
{
	// 경험치 UI 갱신
	float Percent = CurrentEXP / MaxEXP;
	OnEXPChangedDelegate.Broadcast(Percent);
}

void UBaseStatComponent::OnRep_HPChanged()
{
	// HP UI 갱신
	float Percent = CurrentHP / MaxHP;
	OnHPChangedDelegate.Broadcast(Percent);
}

void UBaseStatComponent::OnRep_MPChanged()
{
	// MP UI 갱신
	float Percent = CurrentMP / MaxMP;
	OnMPChangedDelegate.Broadcast(Percent);
}

void UBaseStatComponent::OnRep_LevelChanged()
{
	// Level UI 갱신
	OnLevelChangedDelegate.Broadcast(Level);
}

void UBaseStatComponent::OnRep_AttackChanged()
{
	OnAttackChangedDelegate.Broadcast(Attack);
}

void UBaseStatComponent::OnRep_MagicChanged()
{
	OnMagicChangedDelegate.Broadcast(Magic);
}

void UBaseStatComponent::OnRep_DefenseChanged()
{
	OnDefenseChangedDelegate.Broadcast(Defense);
}

void UBaseStatComponent::OnRep_MoneyChanged()
{
	OnMoneyChangedDelegate.Broadcast(CurrentMoney);
}

void UBaseStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBaseStatComponent, Level);
	DOREPLIFETIME(UBaseStatComponent, CurrentHP);
	DOREPLIFETIME(UBaseStatComponent, CurrentMP);
	DOREPLIFETIME(UBaseStatComponent, CurrentEXP);
	DOREPLIFETIME(UBaseStatComponent, Attack);
	DOREPLIFETIME(UBaseStatComponent, Magic);
	DOREPLIFETIME(UBaseStatComponent, Defense);
	DOREPLIFETIME(UBaseStatComponent, CurrentMoney);
}

void UBaseStatComponent::ApplyDot(const FDotInfo& Info)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (Info.DPS <= 0.f || Info.Tick <= 0.f || Info.Duration <= 0.f) return;
	const int32 Ticks = FMath::CeilToInt(Info.Duration / Info.Tick);
	if (FActiveDot* Found = ActiveDots.Find(Info.Type))
	{
		Found->Spec = Info;
		Found->LeftTick = Ticks;
		Found->DamagePerTick = Info.DPS * Info.Tick;
		GetWorld()->GetTimerManager().ClearTimer(Found->Timer);
		FTimerDelegate Del;
		Del.BindUObject(this, &UBaseStatComponent::TickOneDot, Info.Type);
		GetWorld()->GetTimerManager().SetTimer(Found->Timer, Del, Info.Tick, true);
	}
	else
	{
		FActiveDot NewDot;
		NewDot.Spec = Info;
		NewDot.LeftTick = Ticks;
		NewDot.DamagePerTick = Info.DPS * Info.Tick;
		ActiveDots.Add(Info.Type, NewDot);
		FActiveDot& Ref = ActiveDots[Info.Type];
		FTimerDelegate Del;
		Del.BindUObject(this, &UBaseStatComponent::TickOneDot, Info.Type);
		GetWorld()->GetTimerManager().SetTimer(Ref.Timer, Del, Info.Tick, true);
	}
}

bool UBaseStatComponent::RemoveDotById(FName Type)
{
	if (FActiveDot* Dot = ActiveDots.Find(Type))
	{
		GetWorld()->GetTimerManager().ClearTimer(Dot->Timer);
		ActiveDots.Remove(Type);
		return true;
	}
	return false;
}

void UBaseStatComponent::ClearAllDots()
{
	for (auto& P : ActiveDots)
		GetWorld()->GetTimerManager().ClearTimer(P.Value.Timer);
	ActiveDots.Empty();
}

void UBaseStatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBaseStatComponent::TickOneDot(FName Type)
{
	FActiveDot* Dot = ActiveDots.Find(Type);
	if (!Dot) return;
	if (!GetOwner() || CurrentHP <= 0.f)
	{
		ClearAllDots();
		return;
	}
	GetDamage(Dot->DamagePerTick);
	if (--Dot->LeftTick <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(Dot->Timer);
		ActiveDots.Remove(Type);
	}
}
