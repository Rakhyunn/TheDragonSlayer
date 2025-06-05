#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "BaseStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChangedDelegate, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEXPChangedDelegate, float);

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
    void SetExp(int32 newExp) { maxEXP = newExp; };
    void SetEnemyExp(int32 newExp) { enemyEXP = newExp; };

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
    int32 GetMaxEXP() { return maxEXP; };
    int32 GetCurrentEXP() { return currentEXP; };
    int32 GetEnemyEXP() { return enemyEXP; };

    void AddExperience(int32 amount);
    bool CanLevelUp();
    void LevelUp();

    UFUNCTION()
    void OnRep_ExperienceChanged();

    UFUNCTION()
    void OnRep_HPChanged();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(Replicated)
    int32 level = 1;

    float maxHP = 100.f;
    UPROPERTY(ReplicatedUsing = OnRep_HPChanged)
    float currentHP;

    float maxMP = 50.f;
    UPROPERTY(Replicated)
    float currentMP;

    float attack = 10.f;
    float magic = 10.f;
    float defense = 5.f;

    int32 maxEXP = 0;

    UPROPERTY(ReplicatedUsing = OnRep_ExperienceChanged)
    int32 currentEXP = 100;

    int32 enemyEXP = 10;

public:
    FOnHPChangedDelegate OnHPChangedDelegate;
    FOnEXPChangedDelegate OnEXPChangedDelgate;
};
