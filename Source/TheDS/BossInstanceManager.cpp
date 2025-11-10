#include "BossInstanceManager.h"
#include "InstancePortAllocator.h"

void UBossInstanceManager::Init(const FString& InServerExe, const FString& InOverworldURL, const FString& InMultiHome)
{
    ServerExe = InServerExe;
    OverworldURL = InOverworldURL;
    MultiHome = InMultiHome;
    Allocator = NewObject<UInstancePortAllocator>(this);
    Allocator->Init(7779, 7850);
}

void UBossInstanceManager::AllocateBossInstance(const FString& BossMapPath, const FString& PartyId, FOnInstanceReady OnReady)
{
    if (!Allocator) return;
    int32 Port = 0;
    if (!Allocator->TryAcquire(Port))
    { 
        OnReady.Execute(TEXT(""));
        return;
    }
    const FString InstanceId = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    FString MapURL = FString::Printf(TEXT("%s?listen?InstanceId=%s?PartyId=%s?Overworld=%s"),
        *BossMapPath, *InstanceId, *PartyId, *OverworldURL);
    FString Cmd = FString::Printf(TEXT("\"%s\" %s -port=%d -log"),
        *ServerExe, *MapURL, Port);
    if (!MultiHome.IsEmpty())
    {
        Cmd.Appendf(TEXT(" -MULTIHOME=%s"), *MultiHome);
    }
    FBossInstanceHandle Handle;
    Handle.InstanceId = InstanceId;
    Handle.PartyId = PartyId;
    Handle.Port = Port;
    Handle.Proc = FPlatformProcess::CreateProc(*ServerExe, *FString::Printf(TEXT("%s -port=%d -log%s"),
        *MapURL, Port, MultiHome.IsEmpty() ? TEXT("") : *FString::Printf(TEXT(" -MULTIHOME=%s"), *MultiHome)),
        true, false, false, nullptr, 0, nullptr, nullptr);
    if (!Handle.Proc.IsValid())
    { 
        Allocator->Release(Port);
        OnReady.Execute(TEXT(""));
        return;
    }
    FTimerHandle T; if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().SetTimer(T, FTimerDelegate::CreateLambda([this, OnReady, InstanceId, PartyId, Port]() {
            const FString TravelURL = FString::Printf(TEXT("%s:%d?InstanceId=%s?PartyId=%s"),
                this->MultiHome.IsEmpty() ? TEXT("127.0.0.1") : *this->MultiHome, Port, *InstanceId, *PartyId);
            OnReady.Execute(TravelURL);
            }), 1.5f, false);
    }
    Active.Add(InstanceId, Handle);
}

void UBossInstanceManager::ShutdownInstance(const FString& InstanceId)
{
    if (auto* H = Active.Find(InstanceId))
    {
        if (H->Proc.IsValid()) FPlatformProcess::TerminateProc(H->Proc, true);
        Allocator->Release(H->Port);
        Active.Remove(InstanceId);
    }
}
