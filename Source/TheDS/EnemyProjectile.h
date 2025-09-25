#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossBase.h"
#include "EnemyProjectile.generated.h"

UCLASS()
class THEDS_API AEnemyProjectile : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDotInfo Dot;

protected:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Collision = nullptr;
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* Movement = nullptr;
	
public:	
	AEnemyProjectile();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
