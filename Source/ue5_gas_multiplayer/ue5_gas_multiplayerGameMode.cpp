// Copyright Epic Games, Inc. All Rights Reserved.

#include "ue5_gas_multiplayerGameMode.h"
#include "ue5_gas_multiplayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

Aue5_gas_multiplayerGameMode::Aue5_gas_multiplayerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
