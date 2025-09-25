#pragma once

#include "CoreMinimal.h"
#include "BossBase.h"
#include "WitchBoss.generated.h"

UCLASS()
class THEDS_API AWitchBoss : public ABossBase
{
	GENERATED_BODY()

public:
    // 원거리 공격 정보
    UPROPERTY(EditAnywhere, Category = "Projectile")
    TSubclassOf<AEnemyProjectile> PoisonProjectileClass;

protected:
    UPROPERTY(EditAnywhere, Category = "DotInfo")
    FDotInfo PoisonDot { 3.f, 6.f, 1.f, "Poison" };

    // 순간이동
    UPROPERTY(EditAnywhere, Category = "Move")
    float TeleportMin = 500.f;
    UPROPERTY(EditAnywhere, Category = "Move")
    float TeleportMax = 900.f;

    // 드래곤의 진주
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    UBaseItem* DragonPearlDataAsset = nullptr;
    
    UPROPERTY(ReplicatedUsing = OnRep_Dead, BlueprintReadOnly, Category = "State")
    bool bDead = false;

public:
    AWitchBoss();

protected:
    void ServerTeleportNear(ABaseCharacter* Target);

    virtual void Die(ABaseCharacter* Causer) override;

    UFUNCTION()
    void OnRep_Dead();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    const FDotInfo& GetPoisonDot() const { return PoisonDot; }

    UFUNCTION(Server, Reliable)
    void ServerTeleportToNearestTarget(float SearchRange = 2500.f);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastSetHidden(bool bShow);

    UFUNCTION(BlueprintCallable)
    bool IsDead() const { return bDead; }
};
