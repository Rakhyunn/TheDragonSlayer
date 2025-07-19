#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropItemActor.generated.h"

UCLASS()
class THEDS_API ADropItemActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USphereComponent* Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY()
	class UBaseItem* ItemData;

	UPROPERTY()
	int32 Quantity;

	UPROPERTY(EditDefaultsOnly, Category = "Drop")
	float LifeTime = 30.f; // 자동 소멸 시간

public:
	ADropItemActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 아이템 설정 함수
	void Init(class UBaseItem* itemData, int32 quantity = 1);

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
