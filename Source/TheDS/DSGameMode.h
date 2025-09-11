#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DSGameMode.generated.h"

UCLASS()
class THEDS_API ADSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class UBossInstanceManager* BossInstanceManager;

public:
	ADSGameMode();
	void StartBossInstance(const FString& PartyId, const FString& BossMapPath, TFunction<void(const FString&)> OnReady);
};
