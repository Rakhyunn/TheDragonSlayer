#pragma once

#include "CoreMinimal.h"
#include "BossBase.h"
#include "InteractInterface.h"
#include "DragonBoss.generated.h"

UCLASS()
class THEDS_API ADragonBoss : public ABossBase, public IInteractInterface
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, Category = "Projectile")
    TSubclassOf<AEnemyProjectile> FireProjectileClass;

protected:
    UPROPERTY(EditAnywhere, Category = "DotInfo")
    FDotInfo BurnDot { 8.f, 8.f, 1.f, "Burn" };

    UPROPERTY(EditAnywhere, Category = "Phase")
    float Phase2HPPercent = 0.5f;
    bool bPhase2 = false;

    UPROPERTY(ReplicatedUsing = OnRep_Dead, BlueprintReadOnly, Category = "State")
    bool bDead = false;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")
    bool bCanEndingInteract = false;

public:
    ADragonBoss();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    virtual void DoMelee() override;
    virtual void DoRanged() override;
    virtual void DoMoveOrSpecial() override;

    void EnterPhase2();
    void SetFlying(bool bFly);

    virtual void Die(ABaseCharacter* Causer) override;

    UFUNCTION()
    void OnRep_Dead();
    void EnableEndingInteract(bool bEnable);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
public:
    virtual void Interact(class ABaseCharacter* Interactor) override;

    const FDotInfo& GetBurnDot() const { return BurnDot; }

    UFUNCTION(BlueprintCallable)
    bool IsDead() const { return bDead; }
};
