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

	/** Called from the ArenaCharacter when the character dies. Starts Game Over state. **/
	void HandlePlayerDeath();

protected:

	void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void FinishGame(bool bWin);

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena Sounds", meta = (AllowPrivateAccess = "true"))
	USoundBase* BackgroundMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena Sounds", meta = (AllowPrivateAccess = "true"))
	USoundBase* WinSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena Sounds", meta = (AllowPrivateAccess = "true"))
	USoundBase* FailSound;
};



