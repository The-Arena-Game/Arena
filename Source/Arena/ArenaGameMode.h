// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArnGameMode, Log, All);

UCLASS(minimalapi)
class AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenaGameMode();
};



