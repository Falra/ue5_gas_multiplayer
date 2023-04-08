// Copyright Epic Games, Inc. All Rights Reserved.

#include "AG_GameMode.h"
#include "AG_Character.h"
#include "UObject/ConstructorHelpers.h"

AAG_GameMode::AAG_GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
