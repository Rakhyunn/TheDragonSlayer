#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseStatComponent.generated.h"


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

    float GetCurrentHP() { return currentHP; };
    float GetCurrentMP() { return currentMP; };
    float GetAttack() { return attack; };
    float GetMagic() { return magic; };
    float GetDefense() { return defense; };
    int32 GetLevel() { return level; };
    int32 GetCurrentEXP() { return currentEXP; };
    int32 GetEnemyEXP() { return enemyEXP; };

    void AddExperience(int32 amount);
    bool CanLevelUp();
    void LevelUp();

    UFUNCTION()
    void OnRep_ExperienceChanged();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

private:
    int32 level = 1;

    float maxHP = 100.f;
    float currentHP;

    float maxMP = 50.f;
    float currentMP;

    float attack = 10.f;
    float magic = 10.f;
    float defense = 5.f;

    UPROPERTY(Replicated)
    int32 maxEXP = 0;

    UPROPERTY(ReplicatedUsing = OnRep_ExperienceChanged)
    int32 currentEXP = 100;

    int32 enemyEXP = 10;
};
