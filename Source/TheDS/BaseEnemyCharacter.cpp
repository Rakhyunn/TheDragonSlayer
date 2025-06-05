#include "BaseEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "BaseStatComponent.h"
#include "BaseCharacter.h"
#include "HPBarWidget.h"
#include "Components/WidgetComponent.h"

ABaseEnemyCharacter::ABaseEnemyCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	stat = CreateDefaultSubobject<UBaseStatComponent>(TEXT("StatComponent"));

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
			HPBarWidget->BindHp(stat);
	}
}

void ABaseEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseEnemyCharacter, walkSpeed);
	DOREPLIFETIME(ABaseEnemyCharacter, runSpeed);
}

void ABaseEnemyCharacter::SetCharacterDefaults()
{
	walkSpeed = 500.f;
	runSpeed = 800.f;
	jumpZVelocity = 420.f;

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	GetCharacterMovement()->JumpZVelocity = jumpZVelocity;
}

void ABaseEnemyCharacter::ReceiveDamage(class ABaseCharacter* Causer, float Damage)
{
	stat->GetDamage(Damage);
	UE_LOG(LogTemp, Warning, TEXT("Remain HP: %f"), stat->GetCurrentHP());
	if (stat->GetCurrentHP() <= 0.f)
	{
		Die(Causer);
	}
}