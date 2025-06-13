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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEDS_API UBaseStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UBaseStatComponent();

    void SetLevel(int32 newLevel);
    void SetHP(float newHP) { maxHP = newHP; };
    void SetMP(float newMP) { maxMP = newMP; };
    void SetAttack(float newAttack) { attack = newAttack; };
    void SetMagic(float newMagic) { magic = newMagic; };
    void SetDefense(float newDefense) { defense = newDefense; };
    void SetExp(float newExp) { maxEXP = newExp; };
    void SetEnemyExp(float newExp) { enemyEXP = newExp; };

    void GetDamage(float damageAmount);
    void RestoreHP(float amount);
    void RestoreMP(float amount);

    float GetMaxHP() { return maxHP; };
    float GetCurrentHP() { return currentHP; };
    float GetMaxMP() { return maxMP; };
    float GetCurrentMP() { return currentMP; };
    float GetAttack() { return attack; };
    float GetMagic() { return magic; };
    float GetDefense() { return defense; };
    int32 GetLevel() { return level; };
    float GetMaxEXP() { return maxEXP; };
    float GetCurrentEXP() { return currentEXP; };
    float GetEnemyEXP() { return enemyEXP; };

    void AddExperience(int32 amount);
    bool CanLevelUp();
    void LevelUp();

    void AddAttack(float plusAttack);
    void AddDefense(float plusDefense);

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

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(ReplicatedUsing = OnRep_LevelChanged)
    int32 level = 1;

    float maxHP = 100.f;
    UPROPERTY(ReplicatedUsing = OnRep_HPChanged)
    float currentHP;

    float maxMP = 50.f;
    UPROPERTY(ReplicatedUsing = OnRep_MPChanged)
    float currentMP;

    UPROPERTY(ReplicatedUsing = OnRep_AttackChanged)
    float attack = 10.f;
    UPROPERTY(ReplicatedUsing = OnRep_MagicChanged)
    float magic = 10.f;
    UPROPERTY(ReplicatedUsing = OnRep_DefenseChanged)
    float defense = 5.f;

    float maxEXP = 100.f;

    UPROPERTY(ReplicatedUsing = OnRep_EXPChanged)
    float currentEXP = 0.f;

    float enemyEXP = 10.f;

public:
    FOnHPChangedDelegate OnHPChangedDelegate;
    FOnEXPChangedDelegate OnEXPChangedDelegate;
    FOnMPChangedDelegate OnMPChangedDelegate;
    FOnLevelChangedDelegate OnLevelChangedDelegate;
    FOnAttackChangedDelegate OnAttackChangedDelegate;
    FOnMagicChangedDelegate OnMagicChangedDelegate;
    FOnDefenseChangedDelegate OnDefenseChangedDelegate;
};
