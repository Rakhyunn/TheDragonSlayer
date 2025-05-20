#include "CharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UCharacterAnimInstance::UCharacterAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("/Script/Engine.AnimMontage'/Game/ParagonGreystone/Characters/Heroes/Greystone/Animations/Attack_PrimaryA_Montage.Attack_PrimaryA_Montage'"));
	if (AM.Succeeded())
	{
		AttackMontage = AM.Object;
	}
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn))
	{
		Character = Cast<ACharacter>(Pawn);
		if (IsValid(Character))
		{
			MovementComponent = Character->GetCharacterMovement();
		}
	}
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(Character))
	{
		// 애니매이션 변수 설정
		Velocity = MovementComponent->Velocity;
		GroundSpeed = Velocity.Size2D();
		ShouldMove = GroundSpeed > 3.0f;
	}
}

void UCharacterAnimInstance::AnimNotify_Hit()
{
	// Hit Notify 등록
	OnAttackHit.Broadcast();
}

void UCharacterAnimInstance::PlayAttackMontage()
{
	// 몽타지 재생
	Montage_Play(AttackMontage, 1.f);
}