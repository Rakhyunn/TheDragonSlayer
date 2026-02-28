#include "BasicGmaeMode.h"
#include "TheDSPlayerState.h"
#include "ServerGameInstance.h"
#include "Kismet/GameplayStatics.h"

FString ABasicGmaeMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
    FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

    // 1. URL 옵션에서 UserID와 Nickname 추출
    FString UserID = UGameplayStatics::ParseOption(Options, TEXT("UserID"));
    FString Nickname = UGameplayStatics::ParseOption(Options, TEXT("Nickname"));

    // 2. PlayerState에 세팅
    if (ATheDSPlayerState* PS = NewPlayerController->GetPlayerState<ATheDSPlayerState>())
    {
        PS->SetUserID(UserID);
        PS->SetNickname(Nickname);
    }
    return ErrorMessage;
}

void ABasicGmaeMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ATheDSPlayerState* PS = NewPlayer->GetPlayerState<ATheDSPlayerState>();
    if (!PS) return;
    // UserID나 Nickname이 없으면 로드 요청하지 않음
    if (PS->GetUserID().IsEmpty() || PS->GetNickname().IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("PostLogin: UserID or Nickname is empty, skip load"));
        return;
    }
    UServerGameInstance* GI = Cast<UServerGameInstance>(GetGameInstance());
    if (!GI) return;
    UE_LOG(LogTemp, Log, TEXT("PostLogin: RequestLoad for %s / %s"),
        *PS->GetUserID(), *PS->GetNickname());
    GI->RequestLoadGameData(PS->GetUserID(), PS->GetNickname());
}
