#include "BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "BaseStatComponent.h"
#include "InventoryComponent.h"
#include "EquipmentComponent.h"
#include "BaseMerchantNPC.h"
#include "kismet/GameplayStatics.h"
#include "InteractInterface.h"

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

	Stat = CreateDefaultSubobject<UBaseStatComponent>(TEXT("StatComponent"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetCharacterDefaults();
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("equipmentComponent is: %s"), EquipmentComponent ? TEXT("VALID") : TEXT("NULL"));
	InventoryComponent->AddItem(TestSwordDataAsset, 1);
	InventoryComponent->AddItem(TestShieldDataAsset, 1);
	InventoryComponent->AddItem(TestHeadDataAsset, 1);
	InventoryComponent->AddItem(TestPotionDataAsset, 150);
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

	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &ABaseCharacter::InteractPickUp);
	PlayerInputComponent->BindAction("Talk", IE_Pressed, this, &ABaseCharacter::InteractMerchant);
	PlayerInputComponent->BindAction("UsePortal", IE_Pressed, this, &ABaseCharacter::InteractPortal);
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, WalkSpeed);
	DOREPLIFETIME(ABaseCharacter, RunSpeed);
}

void ABaseCharacter::SetCharacterDefaults()
{
	WalkSpeed = 500.f;
	RunSpeed = 800.f;
	JumpZVelocity = 420.f;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
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
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ABaseCharacter::MulticastStopRun_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ABaseCharacter::InteractPickUp()
{
	TryInteract(EInteractionType::PickUp);
}

void ABaseCharacter::InteractMerchant()
{
	TryInteract(EInteractionType::Talk);
}

void ABaseCharacter::InteractPortal()
{
	TryInteract(EInteractionType::Portal);
}

void ABaseCharacter::ReceiveDamage(float damage)
{
	Stat->GetDamage(damage);
	UE_LOG(LogTemp, Warning, TEXT("Remain HP: %f"), Stat->GetCurrentHP());
}

void ABaseCharacter::DontMove()
{
	bIsAttacking = false;
}

void ABaseCharacter::ServerRestoreHP_Implementation(float Amount)
{
	if (Stat) Stat->RestoreHP(Amount);
}

void ABaseCharacter::ServerRestoreMP_Implementation(float Amount)
{
	if (Stat) Stat->RestoreMP(Amount);
}

void ABaseCharacter::ServerAddAttack_Implementation(float Amount)
{
	if (Stat) Stat->AddAttack(Amount);
}

void ABaseCharacter::ServerAddDefense_Implementation(float Amount)
{
	if (Stat) Stat->AddDefense(Amount);
}

void ABaseCharacter::ServerAddMoney_Implementation(int32 Amount)
{
	if (Stat) Stat->AddMoney(Amount);
}

void ABaseCharacter::ServerSpendMoney_Implementation(int32 Amount)
{
	if (Stat)
	{
		if (Stat->CheckMoney(Amount))
		{
			Stat->SpendMoney(Amount);
		}
		else
		{
			//거래 실패(돈 부족 ui 출력?)
		}
	}
}

void ABaseCharacter::TryInteract(EInteractionType InteractionType)
{
	TArray<AActor*> NearbyActors;
	float Range = 300.f;
	AActor* Closest = nullptr;
	float MinDist = TNumericLimits<float>::Max();

	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UInteractInterface::StaticClass(), NearbyActors);

	for (AActor* Actor : NearbyActors)
	{
		float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
		if (Distance <= Range && Distance < MinDist)
		{
			// 필터링
			if (InteractionType == EInteractionType::PickUp && Actor->ActorHasTag("PickUp"))
			{
				Closest = Actor;
				MinDist = Distance;
			}
			else if (InteractionType == EInteractionType::Talk && Actor->ActorHasTag("MerchantNPC"))
			{
				Closest = Actor;
				MinDist = Distance;
			}
			else if (InteractionType == EInteractionType::Portal && Actor->ActorHasTag("Portal"))
			{
				Closest = Actor;
				MinDist = Distance;
			}
		}
	}

	if (Closest)
	{
		IInteractInterface* Interface = Cast<IInteractInterface>(Closest);
		if (Interface)
		{
			Interface->Interact(this);
		}
	}
}