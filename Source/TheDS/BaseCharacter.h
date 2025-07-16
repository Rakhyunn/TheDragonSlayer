#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class THEDS_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;
	
	UPROPERTY(EditAnywhere)
	class UBaseItem* TestSwordDataAsset;
	UPROPERTY(EditAnywhere)
	class UBaseItem* TestShieldDataAsset;
	UPROPERTY(EditAnywhere)
	class UBaseItem* TestHeadDataAsset;
	UPROPERTY(EditAnywhere)
	class UBaseItem* TestPotionDataAsset;

protected:
	UPROPERTY(Replicated)
	float walkSpeed;

	UPROPERTY(Replicated)
	float runSpeed;

	UPROPERTY(Replicated)
	float jumpZVelocity;

	bool bIsAttacking = true;	// 공격 가능 여부
	FTimerHandle AttackResetTimerHandle;

protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 이동속도, 점프 능력치 조정 함수
	virtual void SetCharacterDefaults();

	// 공통 이동 관련 함수
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);

	// 달리기 함수 -> 서버에서 관리해 동기화될 수 있도록 함
	UFUNCTION(Server, Reliable)
	void ServerStartRun();
	UFUNCTION(Server, Reliable)
	void ServerStopRun();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartRun();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopRun();

	// 공격 함수 가상화 -> 상속받은 캐릭터에서 구현
	virtual void Attack() { };

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	class UBaseStatComponent* stat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEquipmentComponent* EquipmentComponent;

public:
	void ReceiveDamage(float damage);
	
	virtual void DontMove();

	UFUNCTION(Server, Reliable)
	void ServerRestoreHP(float Amount);

	UFUNCTION(Server, Reliable)
	void ServerRestoreMP(float Amount);

	UFUNCTION(Server, Reliable)
	void ServerAddAttack(float Amount);

	UFUNCTION(Server, Reliable)
	void ServerAddDefense(float Amount);

	UFUNCTION(Server, Reliable)
	void ServerAddMoney(int32 Amount);

	UFUNCTION(Server, Reliable)
	void ServerSpendMoney(int32 Amount);

	UFUNCTION()
	void TryInteract();
};