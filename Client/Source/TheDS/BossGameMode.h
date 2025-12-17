#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BossGameMode.generated.h"

UCLASS()
class THEDS_API ABossGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	FString LockedPartyId;
	FString InstanceId;
	FString OverworldURL;

public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& Error) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	UFUNCTION(BlueprintCallable)
	void ReturnToOverworld();
};
