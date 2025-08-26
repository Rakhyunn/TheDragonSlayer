#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEnemyCharacter.generated.h"

UCLASS()
class THEDS_API ABaseEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseEnemyCharacter();

public:
	bool bIsAttacking = true;	// 공격 가능 여부

public:
	// 공격 함수 가상화 -> 상속받은 캐릭터에서 구현
	virtual void Attack() { };

protected:
	UPROPERTY(Replicated)
	float WalkSpeed;

	UPROPERTY(Replicated)
	float RunSpeed;

	UPROPERTY(Replicated)
	float JumpZVelocity;

	FTimerHandle AttackResetTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	class UBaseStatComponent* Stat;

	UPROPERTY(BlueprintReadOnly)
	class UWidgetComponent* HPWidgetComponent;

	UPROPERTY(BlueprintReadOnly)
	class UHPBarWidget* HPBarWidget;

	UPROPERTY(EditAnywhere, Category = "DropItem")
	FName DropRowName;

	UPROPERTY(EditAnywhere, Category = "DropItem")
	UDataTable* DropTable;

	UPROPERTY()
	class AEnemySpawnManager* SpawnManager;

	UPROPERTY()
	APlayerController* LastKillerPC = nullptr;

protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 이동속도, 점프 능력치 조정 함수
	virtual void SetCharacterDefaults();

	virtual void Die(class ABaseCharacter* Causer);

	virtual void DropLoot() { };

public:
	void ReceiveDamage(class ABaseCharacter* Causer, float Damage);
};
