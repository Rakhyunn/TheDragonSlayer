#pragma once

#include "CoreMinimal.h"
#include "PortalType.generated.h"

UENUM(BlueprintType)
enum class EPortalType : uint8
{
	Village,
	MonsterField,
	RaidField
};