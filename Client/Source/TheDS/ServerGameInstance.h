#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "ServerGameInstance.generated.h"

class UBaseItem;

UCLASS()
class THEDS_API UServerGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
    FSocket* AuthSocket = nullptr;
    FString LoggedInID;
    FString LoggedInNickname;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
    TArray<UBaseItem*> AllGameItems;

public:
    virtual void Init() override;
    virtual void Shutdown() override;

    /**서버에 접속*/
    UFUNCTION(BlueprintCallable)
    bool ConnectToAuthServer(const FString& Ip, int32 Port);

    UFUNCTION(BlueprintCallable)
    FString RegisterAccount(const FString& Id, const FString& Password);

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

    // ID 반환
    FString GetLoggedInID() { return LoggedInID; }

    // 데이터 저장 및 로드 요청
    void RequestSaveGameData(const FString& ID, const FString& Nickname, int32 Level, float Exp, int32 Gold, const FString& MapName, FVector Location, const FString& InventoryJson);
    void RequestLoadGameData(const FString& ID, const FString& Nickname);

    void CheckNetworkData();
    void ProcessPacket(const FString& Packet);

    // 아이템ID로 아이템 반환 함수
    UBaseItem* GetItemByID(int32 ItemID);

private:
    bool EnsureConnected();
    bool SendLine(const FString& Line);
    FString WaitResponse(float TimeoutSeconds);
};
