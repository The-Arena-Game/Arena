// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArenaGameMode.h"
#include "ArenaCharacter.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY(LogArnGameMode);

AArenaGameMode::AArenaGameMode() {}

void AArenaGameMode::BeginPlay()
{
	Super::BeginPlay();

	StartGame();
}

void AArenaGameMode::HandlePlayerDeath()
{
	// Get the player from the GameplayStatics
	AArenaCharacter* Player = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!IsValid(Player))
	{
		return;
	}

	// If player is still alive, don't execute game over state
	if (Player->GetHealthComponent()->IsDead())
	{
		FinishGame(false);
	}
}
