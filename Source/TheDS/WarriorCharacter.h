#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "WarriorCharacter.generated.h"

UCLASS()
class THEDS_API AWarriorCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AWarriorCharacter();

private:
	UPROPERTY()
	class UCharacterAnimInstance* animInstance;

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
};