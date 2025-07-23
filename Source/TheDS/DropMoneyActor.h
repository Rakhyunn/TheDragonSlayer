#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h"
#include "DropMoneyActor.generated.h"

UCLASS()
class THEDS_API ADropMoneyActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USphereComponent* Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY()
	int32 MoneyAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Drop")
	float LifeTime = 30.f; // 자동 소멸 시간

public:
	ADropMoneyActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 아이템 설정 함수
	void Init(int32 money = 0);

	virtual void Interact(class ABaseCharacter* Interactor) override;
};
