#include "BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "BaseStatComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SocketOffset = FVector(0.f, 120.f, 200.f);
	SpringArm->bUsePawnControlRotation = true;

	Camera->SetupAttachment(SpringArm);
	Camera->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
	Camera->bUsePawnControlRotation = false;

	stat = CreateDefaultSubobject<UBaseStatComponent>(TEXT("StatComponent"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterDefaults();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 공통 조작
	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &ABaseCharacter::Turn);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ABaseCharacter::ServerStartRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ABaseCharacter::ServerStopRun);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ABaseCharacter::Attack);
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, walkSpeed);
	DOREPLIFETIME(ABaseCharacter, runSpeed);
}

void ABaseCharacter::SetCharacterDefaults()
{
	walkSpeed = 500.f;
	runSpeed = 800.f;
	jumpZVelocity = 420.f;

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	GetCharacterMovement()->JumpZVelocity = jumpZVelocity;
}

void ABaseCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::Turn(float Value)
{
	if (Controller)
	{
		AddControllerYawInput(Value);
	}
}

void ABaseCharacter::ServerStartRun_Implementation()
{
	if (!HasAuthority()) return;
	MulticastStartRun();
}

void ABaseCharacter::ServerStopRun_Implementation()
{
	if (!HasAuthority()) return;
	MulticastStopRun();
}

void ABaseCharacter::MulticastStartRun_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = runSpeed;
}

void ABaseCharacter::MulticastStopRun_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
}

void ABaseCharacter::ReceiveDamage(float damage)
{
	stat->GetDamage(damage);
	UE_LOG(LogTemp, Warning, TEXT("Remain HP: %f"), stat->GetCurrentHP());
}