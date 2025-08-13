#include "BaseEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "BaseStatComponent.h"
#include "BaseCharacter.h"
#include "HPBarWidget.h"
#include "Components/WidgetComponent.h"
#include "EnemySpawnManager.h"
#include "Kismet/GameplayStatics.h"

ABaseEnemyCharacter::ABaseEnemyCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Stat = CreateDefaultSubobject<UBaseStatComponent>(TEXT("StatComponent"));

	HPWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPWidgetComponent"));
	HPWidgetComponent->SetupAttachment(GetMesh());
	HPWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UHPBarWidget> UW(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/BP_Widget/WBP_HPBar.WBP_HPBar_C'"));
	if (UW.Succeeded())
	{
		HPWidgetComponent->SetWidgetClass(UW.Class);
		HPWidgetComponent->SetDrawSize(FVector2D(200.f, 20.f));
		HPWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	}
}

void ABaseEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetCharacterDefaults();
}

void ABaseEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HPWidgetComponent)
	{
		HPBarWidget = Cast<UHPBarWidget>(HPWidgetComponent->GetUserWidgetObject());
		if (HPBarWidget)
			HPBarWidget->BindHp(Stat);
	}

	if (HasAuthority())
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnManager::StaticClass(), Found);
		if (Found.Num() > 0)
		{
			SpawnManager = Cast<AEnemySpawnManager>(Found[0]);
		}
	}
}

void ABaseEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseEnemyCharacter, WalkSpeed);
	DOREPLIFETIME(ABaseEnemyCharacter, RunSpeed);
}

void ABaseEnemyCharacter::SetCharacterDefaults()
{
	WalkSpeed = 500.f;
	RunSpeed = 800.f;
	JumpZVelocity = 420.f;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
}

void ABaseEnemyCharacter::Die(ABaseCharacter* Causer)
{
	DropLoot();
	Causer->Stat->AddExperience(Stat->GetEnemyEXP());
	if (HasAuthority())
	{
		if (SpawnManager)
		{
			SpawnManager->NotifyEnemyDied(GetClass());
		}
		Destroy();
	}
}

void ABaseEnemyCharacter::ReceiveDamage(class ABaseCharacter* Causer, float Damage)
{
	Stat->GetDamage(Damage);
	UE_LOG(LogTemp, Warning, TEXT("Remain HP: %f"), Stat->GetCurrentHP());
	if (Stat->GetCurrentHP() <= 0.f)
	{
		Die(Causer);
	}
}