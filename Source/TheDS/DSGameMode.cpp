#include "DSGameMode.h"
#include "BossInstanceManager.h"

ADSGameMode::ADSGameMode()
{
    bUseSeamlessTravel = true;
    BossInstanceManager = CreateDefaultSubobject<UBossInstanceManager>(TEXT("BossInstanceManager"));
    const FString ServerExe = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), TEXT("Binaries/Win64/TheDragonSlayerServer.exe")));
    BossInstanceManager->Init(ServerExe, /*Overworld*/ TEXT("127.0.0.1:7777"), /*MultiHome*/ TEXT("127.0.0.1"));
}

void ADSGameMode::StartBossInstance(const FString& PartyId, const FString& BossMapPath, TFunction<void(const FString&)> OnReady)
{
    BossInstanceManager->AllocateBossInstance(BossMapPath, PartyId, FOnInstanceReady::CreateLambda([OnReady](const FString& URL) {
        OnReady(URL);
        }));
}
