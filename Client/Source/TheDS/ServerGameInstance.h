#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "ServerGameInstance.generated.h"

UCLASS()
class THEDS_API UServerGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
    FSocket* AuthSocket = nullptr;
    FString LoggedInID;
    FString LoggedInNickname;

public:
    virtual void Init() override;
    virtual void Shutdown() override;

    /**서버에 접속*/
    UFUNCTION(BlueprintCallable)
    bool ConnectToAuthServer(const FString& Ip, int32 Port);

    UFUNCTION(BlueprintCallable)
    FString RegisterAccount(const FString& Id, const FString& Password, const FString& Nickname);

    /**로그인*/
    UFUNCTION(BlueprintCallable)
    FString LoginAccount(const FString& Id, const FString& Password);

    /**ID 중복 체크*/
    UFUNCTION(BlueprintCallable)
    FString CheckID(const FString& Id);

    /**닉네임 중복 체크*/
    UFUNCTION(BlueprintCallable)
    FString CheckNickname(const FString& Nickname);

    FString GetLoggedInNickname() { return LoggedInNickname; }

private:
    bool EnsureConnected();
    bool SendLine(const FString& Line);
    FString WaitResponse(float TimeoutSeconds);
};
