// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlatformerTemplateGameMode.h"
#include "PlatformerTemplateCharacter.h"
#include "UObject/ConstructorHelpers.h"

APlatformerTemplateGameMode::APlatformerTemplateGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/Character/BP_Mario64Character"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
