#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BossInstanceManager.generated.h"

DECLARE_DELEGATE_OneParam(FOnInstanceReady, const FString&);

USTRUCT()
struct FBossInstanceHandle
{
    GENERATED_BODY()

public:
    FString InstanceId;
    FString PartyId;
    int32   Port = 0;
    FProcHandle Proc;
};

UCLASS()
class THEDS_API UBossInstanceManager : public UObject
{
	GENERATED_BODY()
	
private:
    FString ServerExe;
    FString OverworldURL;
    FString MultiHome;
    TMap<FString, FBossInstanceHandle> Active;
    UPROPERTY() 
    class UInstancePortAllocator* Allocator = nullptr;

public:
    void Init(const FString& InServerExe, const FString& InOverworldURL, const FString& InMultiHome = TEXT(""));
    void AllocateBossInstance(const FString& BossMapPath, const FString& PartyId, FOnInstanceReady OnReady);
    void ShutdownInstance(const FString& InstanceId);
};
