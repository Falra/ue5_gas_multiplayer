// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AG_GameMode.generated.h"

class AAG_PlayerController;

UCLASS(minimalapi)
class AAG_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAG_GameMode();

    void NotifyPlayerDied(AAG_PlayerController* PlayerController);
};



