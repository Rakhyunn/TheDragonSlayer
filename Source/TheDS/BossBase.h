#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyCharacter.h"
#include "BossBase.generated.h"

class UBaseItem;
class AEnemyProjectile;
class ABaseCharacter;

UENUM(BlueprintType)
enum class EBossAction : uint8
{
	Idle,			// 기본 상태
	Melee,			// 근접 공격
	Ranged,			// 원거리 공격
	Teleport,		// 텔레포트(마녀)
	TakeOff,		// 이륙(드래곤)
	Land,			// 착륙(드래곤)
	Die				// 사망
};

USTRUCT(BlueprintType)
struct FDotInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DPS = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Duration = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Tick = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Type;		// 마녀-독, 드래곤-화상
};

USTRUCT(BlueprintType)
struct FThreatEntry 
{
	GENERATED_BODY()

public:
	float AccDamage = 0.f;
	float LastHitTime = 0.f;
};


UCLASS()
class THEDS_API ABossBase : public ABaseEnemyCharacter
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MeleeRange = 200.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float RangedRange = 1000.f;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_ActiveAction)
	EBossAction ActiveAction = EBossAction::Idle;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ThinkInterval = 1.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MeleeCooldown = 2.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float RangedCooldown = 3.f;

	float NextMeleeTime = 0.f;
	float NextRangedTime = 0.f;

	FTimerHandle ThinkTimer;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* MeleeMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* RangedMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* TeleportMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* TakeOffMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* LandMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* DieMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "BT")
	bool bUseBehaviorTree;

	UPROPERTY()
	TMap<TWeakObjectPtr<AController>, FThreatEntry> ThreatMap;

public:
	ABossBase();

	UFUNCTION()
	void OnRep_ActiveAction();

	UFUNCTION(Server, Reliable)
	void ServerStartAction(EBossAction Action, FName Section);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAction(EBossAction Action, FName Section);

	void ApplyDot(ABaseCharacter* Target, const FDotInfo& Dot);

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void Think();

	virtual void DoMelee() PURE_VIRTUAL(ABossBase::MeleeAttack, );
	virtual void DoRanged() PURE_VIRTUAL(ABossBase::RangedAttack, );
	virtual void DoMoveOrSpecial() PURE_VIRTUAL(ABossBase::MoveOrSpecial, );

public:
	UFUNCTION()
	ABaseCharacter* FindNearestPlayer(float Range) const;

	UFUNCTION(Server, Reliable)
	void ServerReportThreat(AController* AttackController, float Damage);

	ABaseCharacter* PickBestThreatTarget(float Now);
};
