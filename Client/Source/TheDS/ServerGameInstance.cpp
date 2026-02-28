#include "ServerGameInstance.h"
#include "Misc/Paths.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "HAL/PlatformTime.h"
#include "HAL/PlatformProcess.h"
#include "TheDSPlayerState.h"
#include "BaseItem.h"

void UServerGameInstance::Init()
{
    Super::Init();

    // 테스트 용: 프로젝트 실행 시 자동 연결
    // ConnectToAuthServer(TEXT("127.0.0.1"), 6000);
    if (IsRunningDedicatedServer())
    {
        UE_LOG(LogTemp, Log, TEXT("[Dedicated Server] Connecting to Auth/DB Server."));
        ConnectToAuthServer(TEXT("127.0.0.1"), 6000);
        FTimerHandle hTimer;
        GetTimerManager().SetTimer(hTimer, this, &UServerGameInstance::CheckNetworkData, 0.1f, true);
    }
    else
    {
        ConnectToAuthServer(TEXT("127.0.0.1"), 6000);
    }
}

void UServerGameInstance::Shutdown()
{
    if (AuthSocket)
    {
        AuthSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(AuthSocket);
        AuthSocket = nullptr;
    }
    Super::Shutdown();
}

bool UServerGameInstance::ConnectToAuthServer(const FString& Ip, int32 Port)
{
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

    AuthSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("AuthSocket"), false);
    if (!AuthSocket)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create socket"));
        return false;
    }

    FIPv4Address AddrIP;
    if (!FIPv4Address::Parse(Ip, AddrIP))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid IP: %s"), *Ip);
        return false;
    }

    TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
    Addr->SetIp(AddrIP.Value);
    Addr->SetPort(Port);

    bool bConnected = AuthSocket->Connect(*Addr);

    UE_LOG(LogTemp, Log, TEXT("ConnectToAuthServer(%s:%d) : %s"),
        *Ip, Port, bConnected ? TEXT("Success") : TEXT("Fail"));

    return bConnected;
}

void UServerGameInstance::RequestSaveGameData(int32 Level, float Exp, int32 Gold, const FString& MapName, FVector Location, const FString& InventoryJson)
{
    if (!EnsureConnected())
    {
        ConnectToAuthServer(TEXT("127.0.0.1"), 6000);
        if (!EnsureConnected()) return;
    }
    FString Packet = FString::Printf(TEXT("SAVEUSER %d %f %d %s %f %f %f %s"),
        Level, Exp, Gold, *MapName, Location.X, Location.Y, Location.Z, *InventoryJson);
    if (SendLine(Packet))
    {
        UE_LOG(LogTemp, Log, TEXT("Save Request Sent for User"));
    }
}

void UServerGameInstance::RequestLoadGameData(const FString& ID, const FString& Nickname)
{
    if (!EnsureConnected())
    {
        ConnectToAuthServer(TEXT("127.0.0.1"), 6000);
        if (!EnsureConnected()) return;
    }
    FString Packet = FString::Printf(TEXT("LOADUSER %s %s"), *ID, *Nickname);
    SendLine(Packet);
}

void UServerGameInstance::CheckNetworkData()
{
    if (!AuthSocket) return;
    uint32 PendingDataSize = 0;
    if (AuthSocket->HasPendingData(PendingDataSize) && PendingDataSize > 0)
    {
        TArray<uint8> ReceivedData;
        ReceivedData.SetNumUninitialized(PendingDataSize);
        int32 BytesRead = 0;
        if (AuthSocket->Recv(ReceivedData.GetData(), PendingDataSize, BytesRead))
        {
            FString Message = FString(BytesRead, UTF8_TO_TCHAR((const char*)ReceivedData.GetData()));
            TArray<FString> Lines;
            Message.ParseIntoArray(Lines, TEXT("\n"), true);
            for (const FString& Line : Lines)
            {
                ProcessPacket(Line);
            }
        }
    }
}

void UServerGameInstance::ProcessPacket(const FString& Packet)
{
    TArray<FString> Tokens;
    Packet.ParseIntoArray(Tokens, TEXT(" "), true);
    if (Tokens.Num() <= 0) return;
    if (Tokens[0] == TEXT("LoadData"))
    {
        if (Tokens.Num() < 3) return;
        FString TargetNickname = Tokens[1];
        FString Payload = Packet.RightChop(Tokens[0].Len() + 1 + Tokens[1].Len() + 1);
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
        {
            if (APlayerController* PC = It->Get())
            {
                if (ATheDSPlayerState* PS = PC->GetPlayerState<ATheDSPlayerState>())
                {
                    if (PS->GetNickname() == TargetNickname)
                    {
                        PS->LoadUserData(Payload);
                        UE_LOG(LogTemp, Log, TEXT("Data Loaded for User: %s"), *TargetNickname);
                        break;
                    }
                }
            }
        }
    }
}

UBaseItem* UServerGameInstance::GetItemByID(int32 ItemID)
{
    for (UBaseItem* Item : AllGameItems)
    {
        if (Item && Item->ItemID == ItemID)
        {
            return Item;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Could not find Item: %d"), ItemID);
    return nullptr;
}

bool UServerGameInstance::EnsureConnected()
{
    if (AuthSocket && AuthSocket->GetConnectionState() == SCS_Connected) return true;
    return ConnectToAuthServer(TEXT("127.0.0.1"), 6000);
}

bool UServerGameInstance::SendLine(const FString& Line)
{
    if (!AuthSocket) return false;
    FString Msg = Line + TEXT("\n");
    FTCHARToUTF8 Converter(*Msg);
    int32 Size = Converter.Length();
    int32 Sent = 0;
    bool bOk = AuthSocket->Send((uint8*)Converter.Get(), Size, Sent);
    UE_LOG(LogTemp, Log, TEXT("SendLine: %s (Sent=%d/Size=%d)"), *Line, Sent, Size);
    return bOk && (Sent == Size);
}

FString UServerGameInstance::WaitResponse(float TimeoutSeconds)
{
    if (!AuthSocket) return TEXT("NOT_CONNECTED");
    double Start = FPlatformTime::Seconds();
    uint8 Buffer[1024];
    int32 BytesRead = 0;
    while (FPlatformTime::Seconds() - Start < TimeoutSeconds)
    {
        uint32 Pending = 0;
        if (AuthSocket->HasPendingData(Pending) && Pending > 0)
        {
            int32 ReadSize = FMath::Min<int32>((int32)Pending, (int32)sizeof(Buffer));
            if (AuthSocket->Recv(Buffer, ReadSize, BytesRead))
            {
                Buffer[BytesRead] = 0;
                FString Result = FString(UTF8_TO_TCHAR((char*)Buffer));
                Result.TrimEndInline();
                UE_LOG(LogTemp, Log, TEXT("Recv: %s"), *Result);
                return Result;
            }
        }
        FPlatformProcess::Sleep(0.01f);
    }
    return TEXT("TIMEOUT");
}

FString UServerGameInstance::RegisterAccount(const FString& Id, const FString& Password)
{
    if (!EnsureConnected()) return TEXT("Failed Connect Server");
    FString Line = FString::Printf(TEXT("REGISTER %s %s"), *Id, *Password);
    if (!SendLine(Line)) return TEXT("Failed Send");
    return WaitResponse(3.0f);
}

FString UServerGameInstance::LoginAccount(const FString& Id, const FString& Password)
{
    if (!EnsureConnected()) return TEXT("Failed Connect Server");
    FString Line = FString::Printf(TEXT("LOGIN %s %s"), *Id, *Password);
    if (!SendLine(Line)) return TEXT("Failed Send");
    FString ResultMessage = WaitResponse(3.0f);
    TArray<FString> Tokens;
    ResultMessage.ParseIntoArrayWS(Tokens);
    if (Tokens.Num() >= 2 && Tokens[0] == TEXT("Login") && Tokens[1] == TEXT("Success"))
    {
        LoggedInID = Id;
        //LoggedInNickname = Tokens[2];
        return TEXT("Login Success");
    }
    return ResultMessage;
}

FString UServerGameInstance::CheckID(const FString& Id)
{
    if (!EnsureConnected()) return TEXT("Failed Connect Server");
    FString Line = FString::Printf(TEXT("CHECKID %s"), *Id);
    if (!SendLine(Line)) return TEXT("Failed Send");
    return WaitResponse(3.0f);
}

FString UServerGameInstance::CheckNickname(const FString& Nickname)
{
    if (!EnsureConnected()) return TEXT("Failed Connect Server");
    FString Line = FString::Printf(TEXT("CHECKNICK %s"), *Nickname);
    if (!SendLine(Line)) return TEXT("Failed Send");
    return WaitResponse(3.0f);
}

FString UServerGameInstance::GetCharacterList()
{
    if (!EnsureConnected()) return TEXT("Failed Connect Server");
    if (!SendLine(TEXT("GETCHARACTERLIST"))) return TEXT("Failed Send");
    return WaitResponse(3.0f);
}

FString UServerGameInstance::SelectCharacter(const FString& Nickname)
{
    if (!EnsureConnected()) return TEXT("Failed Connect Server");
    FString Line = FString::Printf(TEXT("SELECTCHARACTER %s"), *Nickname);
    if (!SendLine(Line)) return TEXT("Failed Send");
    FString Result = WaitResponse(3.0f);
    // 성공 시 LoggedInNickname 세팅
    // 응답: "LoadData <nickname> ..."
    if (Result.StartsWith(TEXT("LoadData")))
    {
        LoggedInNickname = Nickname;
    }
    return Result;
}

FString UServerGameInstance::CreateCharacter(const FString& Nickname, const FString& CharClass)
{
    if (!EnsureConnected()) return TEXT("Failed Connect Server");
    FString Line = FString::Printf(TEXT("CREATECHARACTER %s %s"), *Nickname, *CharClass);
    if (!SendLine(Line)) return TEXT("Failed Send");
    return WaitResponse(3.0f);
}
