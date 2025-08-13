#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "BaseStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEXPChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMPChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelChangedDelegate, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttackChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMagicChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDefenseChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMoneyChangedDelegate, int32);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEDS_API UBaseStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UBaseStatComponent();

    void SetLevel(int32 NewLevel);
    void SetHP(float NewHP) { MaxHP = NewHP; };
    void SetMP(float NewMP) { MaxMP = NewMP; };
    void SetAttack(float NewAttack) { Attack = NewAttack; };
    void SetMagic(float NewMagic) { Magic = NewMagic; };
    void SetDefense(float newDefense) { Defense = newDefense; };
    void SetExp(float NewExp) { MaxEXP = NewExp; };
    void SetEnemyExp(float NewExp) { EnemyEXP = NewExp; };

    void GetDamage(float DamageAmount);
    void RestoreHP(float Amount);
    void RestoreMP(float Amount);

    float GetMaxHP() { return MaxHP; };
    float GetCurrentHP() { return CurrentHP; };
    float GetMaxMP() { return MaxMP; };
    float GetCurrentMP() { return CurrentMP; };
    float GetAttack() { return Attack; };
    float GetMagic() { return Magic; };
    float GetDefense() { return Defense; };
    int32 GetLevel() { return Level; };
    float GetMaxEXP() { return MaxEXP; };
    float GetCurrentEXP() { return CurrentEXP; };
    float GetEnemyEXP() { return EnemyEXP; };
    int32 GetMoney() { return CurrentMoney; };

    void AddExperience(int32 Amount);
    bool CanLevelUp();
    void LevelUp();

    void AddAttack(float PlusAttack);
    void AddDefense(float PlusDefense);

    void AddMoney(int32 Money);
    void SpendMoney(int32 Money);
    bool CheckMoney(int32 Money);

    UFUNCTION()
    void OnRep_EXPChanged();

    UFUNCTION()
    void OnRep_HPChanged();

    UFUNCTION()
    void OnRep_MPChanged();

    UFUNCTION()
    void OnRep_LevelChanged();

    UFUNCTION()
    void OnRep_AttackChanged();

    UFUNCTION()
    void OnRep_MagicChanged();

    UFUNCTION()
    void OnRep_DefenseChanged();

    UFUNCTION()
    void OnRep_MoneyChanged();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(ReplicatedUsing = OnRep_LevelChanged)
    int32 Level = 1;

    float MaxHP = 100.f;
    UPROPERTY(ReplicatedUsing = OnRep_HPChanged)
    float CurrentHP;

    float MaxMP = 50.f;
    UPROPERTY(ReplicatedUsing = OnRep_MPChanged)
    float CurrentMP;

    UPROPERTY(ReplicatedUsing = OnRep_AttackChanged)
    float Attack = 10.f;
    UPROPERTY(ReplicatedUsing = OnRep_MagicChanged)
    float Magic = 10.f;
    UPROPERTY(ReplicatedUsing = OnRep_DefenseChanged)
    float Defense = 5.f;

    float MaxEXP = 100.f;
    UPROPERTY(ReplicatedUsing = OnRep_EXPChanged)
    float CurrentEXP = 0.f;

    float EnemyEXP = 10.f;

    UPROPERTY(ReplicatedUsing = OnRep_MoneyChanged)
    int32 CurrentMoney = 1000;

public:
    FOnHPChangedDelegate OnHPChangedDelegate;
    FOnEXPChangedDelegate OnEXPChangedDelegate;
    FOnMPChangedDelegate OnMPChangedDelegate;
    FOnLevelChangedDelegate OnLevelChangedDelegate;
    FOnAttackChangedDelegate OnAttackChangedDelegate;
    FOnMagicChangedDelegate OnMagicChangedDelegate;
    FOnDefenseChangedDelegate OnDefenseChangedDelegate;
    FOnMoneyChangedDelegate OnMoneyChangedDelegate;
};
