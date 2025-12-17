#include "BossBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "BaseCharacter.h"
#include "TimerManager.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BaseStatComponent.h"

ABossBase::ABossBase()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void ABossBase::OnRep_ActiveAction()
{
    UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
    if (!Anim) return;
    switch (ActiveAction)
    {
    case EBossAction::Idle:
        break;
    case EBossAction::Melee:
        if (MeleeMontage) Anim->Montage_Play(MeleeMontage);
        break;
    case EBossAction::Ranged:
        if (RangedMontage) Anim->Montage_Play(RangedMontage);
        break;
    case EBossAction::Teleport:
        if (TeleportMontage) Anim->Montage_Play(TeleportMontage);
        break;
    case EBossAction::TakeOff:
        if (TakeOffMontage) Anim->Montage_Play(TakeOffMontage);
        break;
    case EBossAction::Land:
        if (LandMontage) Anim->Montage_Play(LandMontage);
        break;
    case EBossAction::Phase:
        if (PhaseMontage) Anim->Montage_Play(PhaseMontage);
    }
}

void ABossBase::ServerStartAction_Implementation(EBossAction Action, FName Section)
{
    ActiveAction = Action;
    OnRep_ActiveAction();
    MulticastPlayAction(Action, Section);
}

void ABossBase::MulticastPlayAction_Implementation(EBossAction Action, FName Section)
{
    OnRep_ActiveAction();
    if (Section.IsNone()) return;
    UAnimInstance* Anim = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
    if (!Anim) return;
    UAnimMontage* Montage = nullptr;
    switch (Action)
    {
    case EBossAction::Idle:
        break;
    case EBossAction::Melee:
        if (MeleeMontage) Montage = MeleeMontage;
        break;
    case EBossAction::Ranged:
        if (RangedMontage) Montage = RangedMontage;
        break;
    case EBossAction::Teleport:
        if (TeleportMontage) Montage = TeleportMontage;
        break;
    case EBossAction::TakeOff:
        if (TakeOffMontage) Montage = TakeOffMontage;
        break;
    case EBossAction::Land:
        if (LandMontage) Montage = LandMontage;
        break;
    case EBossAction::Phase:
        if (PhaseMontage) Montage = PhaseMontage;
        break;
    }
    if(Montage) Anim->Montage_JumpToSection(Section, Montage);
}

void ABossBase::BeginPlay()
{
    Super::BeginPlay();
}

void ABossBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABossBase, ActiveAction);
}

ABaseCharacter* ABossBase::FindNearestPlayer(float Range) const
{
    TArray<AActor*> Actors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), Actors);
    ABaseCharacter* Player = nullptr;
    float Distance = Range;
    for (AActor* A : Actors)
    {
        if (!A) continue;
        const float Dist = FVector::Dist(A->GetActorLocation(), GetActorLocation());
        if (Dist <= Distance)
        {
            Distance = Dist;
            Player = Cast<ABaseCharacter>(A);
        }
    }
    return Player;
}

void ABossBase::ApplyDot(ABaseCharacter* Target, const FDotInfo& Dot)
{
    if (!HasAuthority() || !Target || !Target->Stat  || Dot.DPS <= 0.f || Dot.Duration <= 0.f || Dot.Tick <= 0.f) return;
    FDotInfo Info;
    Info.DPS = Dot.DPS;
    Info.Tick = Dot.Tick;
    Info.Duration = Dot.Duration;
    Info.Type = Dot.Type;
    Info.Source = this;

    Target->Stat->ApplyDot(Info);
}

void ABossBase::ServerReportThreat_Implementation(AController* AttackController, float Damage)
{
    if (!HasAuthority() || !AttackController || Damage <= 0.f) return;
    const float Now = GetWorld()->GetTimeSeconds();
    FThreatEntry& E = ThreatMap.FindOrAdd(AttackController);
    E.AccDamage += Damage;
    E.LastHitTime = Now;
    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
        {
            if (ABaseCharacter* Best = PickBestThreatTarget(GetWorld()->GetTimeSeconds()))
                BB->SetValueAsObject("TargetActor", Best);
        }
    }
}

ABaseCharacter* ABossBase::PickBestThreatTarget(float Now)
{
    const float MemorySec = 8.f;
    const float DecayPerSec = 0.15f;
    for (auto It = ThreatMap.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid())
        { 
            It.RemoveCurrent(); 
            continue; 
        }
        FThreatEntry& E = It.Value();
        const float age = Now - E.LastHitTime;
        if (age > MemorySec) E.AccDamage *= 0.5f;
        E.AccDamage = FMath::Max(0.f, E.AccDamage - DecayPerSec);
        if (E.AccDamage <= KINDA_SMALL_NUMBER) It.RemoveCurrent();
    }
    ABaseCharacter* Best = nullptr; 
    float BestT = -1.f;
    for (auto& Pair : ThreatMap)
    {
        if (AController* C = Pair.Key.Get())
            if (ABaseCharacter* P = Cast<ABaseCharacter>(C->GetPawn()))
            {
                if (Pair.Value.AccDamage > BestT)
                { 
                    BestT = Pair.Value.AccDamage; 
                    Best = P; 
                }
            }
    }
    return Best;
}
