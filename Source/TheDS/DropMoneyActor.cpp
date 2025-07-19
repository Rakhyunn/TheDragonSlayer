#include "DropMoneyActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseItem.h"
#include "BaseCharacter.h"
#include "BaseStatComponent.h"

ADropMoneyActor::ADropMoneyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->InitSphereRadius(80.f);
	Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADropMoneyActor::OnOverlapBegin);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/AncientTreasures/Meshes/SM_Coin_Pile_01a.SM_Coin_Pile_01a'"));
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
	}
}

void ADropMoneyActor::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeTime);
	UE_LOG(LogTemp, Warning, TEXT("DropMoneyActor Spawned"));
}

void ADropMoneyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADropMoneyActor::Init(int32 money)
{
	MoneyAmount = money;
}

void ADropMoneyActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor);
	if (Player)
	{
		Player->stat->AddMoney(MoneyAmount);
		Destroy();
	}
}
