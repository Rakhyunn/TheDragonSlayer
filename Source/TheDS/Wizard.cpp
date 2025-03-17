#include "Wizard.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

AWizard::AWizard()
{
	PrimaryActorTick.bCanEverTick = true;
	//Set SkeletalMesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonDekker/Characters/Heroes/Dekker/Meshes/Dekker.Dekker'"));
	if (SM.Succeeded()) {
		GetMesh()->SetSkeletalMesh(SM.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	}

	//Set SpringArm && Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	//SpringArm->bUsePawnControlRotation = true;

	Camera->SetupAttachment(SpringArm);
	//Camera->bUsePawnControlRotation = false;
}

void AWizard::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWizard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWizard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

