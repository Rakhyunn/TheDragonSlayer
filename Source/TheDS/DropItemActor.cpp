#include "DropItemActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseItem.h"
#include "BaseCharacter.h"
#include "InventoryComponent.h"

ADropItemActor::ADropItemActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->InitSphereRadius(80.f);
	Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADropItemActor::OnOverlapBegin);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADropItemActor::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeTime);
	UE_LOG(LogTemp, Warning, TEXT("DropItemActor Spawned"));
}

void ADropItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADropItemActor::Init(UBaseItem* itemData, int32 quantity)
{
	ItemData = itemData;
	Quantity = quantity;
	if (ItemData && ItemData->DroppedMesh)
	{
		Mesh->SetStaticMesh(ItemData->DroppedMesh);
	}
}

void ADropItemActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor);
	if (Player && ItemData)
	{
		Player->InventoryComponent->AddItem(ItemData, Quantity);
		Destroy();
	}
}

