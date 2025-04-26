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
	UPROPERTY(EditAnywhere, Category = "Attack")
	class UAnimMontage* AttackMontage;

private: 
	virtual void BeginPlay() override;

	virtual void SetCharacterDefaults() override;

	virtual void Attack() override;

	UFUNCTION(Server, Reliable)
	void ServerAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack();
};
