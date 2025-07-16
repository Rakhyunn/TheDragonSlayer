#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyCharacter.h"
#include "BasicEnemy_1.generated.h"

UCLASS()
class THEDS_API ABasicEnemy_1 : public ABaseEnemyCharacter
{
	GENERATED_BODY()
	
public:
	ABasicEnemy_1();

private:
	UPROPERTY()
	class UCharacterAnimInstance* AnimInstance;

private:
	virtual void BeginPlay() override;

	virtual void SetCharacterDefaults() override;

	virtual void Attack() override;		// 공격 상속 정의

	// 서버, 클라이언트 공격 분리
	UFUNCTION(Server, Reliable)
	void ServerAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack();
	// 몽타지 종료 시 호출
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	virtual void Die(class ABaseCharacter* Causer) override;

	virtual void DropLoot() override;
};
