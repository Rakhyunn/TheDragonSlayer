#pragma once

#include "CoreMinimal.h"
#include "PartyType.generated.h"

USTRUCT(BlueprintType)
struct FPartyMember
{
	GENERATED_BODY()

	UPROPERTY()
	APlayerState* Member;

	UPROPERTY(BlueprintReadOnly)
	FString Nickname;

	UPROPERTY(BlueprintReadOnly)
	int32 Level;
};

USTRUCT(BlueprintType)
struct FPartyInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	APlayerState* Leader;

	UPROPERTY(BlueprintReadOnly)
	TArray<FPartyMember> Members;
};