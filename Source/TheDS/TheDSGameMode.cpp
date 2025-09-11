// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheDSGameMode.h"
#include "TheDSCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATheDSGameMode::ATheDSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
