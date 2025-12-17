#include "BossGameMode.h"
#include "Kismet/GameplayStatics.h"

void ABossGameMode::InitGame(const FString& MapName, const FString& Options, FString& Error)
{
    Super::InitGame(MapName, Options, Error);
    InstanceId = UGameplayStatics::ParseOption(Options, TEXT("InstanceId"));
    LockedPartyId = UGameplayStatics::ParseOption(Options, TEXT("PartyId"));
    OverworldURL = UGameplayStatics::ParseOption(Options, TEXT("Overworld"));
    bUseSeamlessTravel = true;
}

void ABossGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
    if (!ErrorMessage.IsEmpty()) return; // 이미 상위에서 에러가 생기면 그대로 중단
    const FString IncomingParty = UGameplayStatics::ParseOption(Options, TEXT("PartyId"));
    if (LockedPartyId.IsEmpty() || IncomingParty != LockedPartyId)
    {
        ErrorMessage = TEXT("DENY:WrongParty");
        return;
    }
}

void ABossGameMode::ReturnToOverworld()
{
    const FString URL = OverworldURL.IsEmpty() ? TEXT("127.0.0.1:7777") : OverworldURL;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        if (APlayerController* PC = It->Get()) 
            PC->ClientTravel(URL, TRAVEL_Absolute);
}
