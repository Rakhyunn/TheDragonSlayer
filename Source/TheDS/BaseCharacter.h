#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	PickUp,
	Talk,
	Portal
};

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

protected:
	UPROPERTY(Replicated)
	float WalkSpeed;

	UPROPERTY(Replicated)
	float RunSpeed;

	UPROPERTY(Replicated)
	float JumpZVelocity;

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
	
	// 상호작용 함수
	void InteractPickUp();
	void InteractNPC();
	void InteractPortal();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	class UBaseStatComponent* Stat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UEquipmentComponent* EquipmentComponent;

public:
	void ReceiveDamage(float Damage);
	
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
	void TryInteract(EInteractionType InteractionType);

	UFUNCTION(Server, Reliable)
	void ServerTryPickup();

	UFUNCTION(Server, Reliable)
	void ServerTryTalk();

	UFUNCTION()
	void ServerDie();
};