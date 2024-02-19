// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

class AGlobeBase;
class AArenaCharacter;

DECLARE_LOG_CATEGORY_EXTERN(LogArnGameMode, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRestartDelegate);

UENUM(BlueprintType)
enum class EGameStates : uint8 {
	None    UMETA(DisplayName = "None"),
	Menu    UMETA(DisplayName = "Menu State"),
	Ready	UMETA(DisplayName = "Ready State"),
	Play	UMETA(DisplayName = "Play State"),
	Pause	UMETA(DisplayName = "Pause State"),
	Lost	UMETA(DisplayName = "Lost State"),
	Win		UMETA(DisplayName = "Win State"),
	Prepare UMETA(DisplayName = "Prepare State"),
};

UCLASS(minimalapi)
class AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenaGameMode();

	/** Called within the RestartArenaGame function to inform other about the restart */
	UPROPERTY(BlueprintAssignable) FRestartDelegate OnRestart;

	/** Called from the ArenaCharacter when the character dies. Starts Game Over state. **/
	void HandlePlayerDeath();

	/** Called from the Globes to inform the interaction */
	void YellowTouch(AGlobeBase* Globe);

	/** Called from the Globes to inform the interaction */
	void BlueTouch(AGlobeBase* Globe);

	UFUNCTION(BlueprintCallable, Category = "Arena State")
	FORCEINLINE EGameStates GetGameState() const { return ArenaGameState; }

	UFUNCTION(BlueprintCallable, Category = "Arena State")
	FORCEINLINE void SetGameState(EGameStates State) { ArenaGameState = State; }

	UFUNCTION(BlueprintCallable, Category = "Arena State")
	void RestartArenaGame();

protected:

	void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Arena State")
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent, Category = "Arena State")
	void FinishGame();

private:

	/*----------------------------------------------------------------------------
		Effects
	----------------------------------------------------------------------------*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena Sounds", meta = (AllowPrivateAccess = "true"))
	USoundBase* BackgroundMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena Sounds", meta = (AllowPrivateAccess = "true"))
	USoundBase* WinSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena Sounds", meta = (AllowPrivateAccess = "true"))
	USoundBase* FailSound;

	/*----------------------------------------------------------------------------
		Game State Variables
	----------------------------------------------------------------------------*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena State", meta = (AllowPrivateAccess = "true"))
	EGameStates ArenaGameState = EGameStates::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena State", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGlobeBase> BlueGlobeClass;

	TArray<AActor*> YellowGlobes;
	int GlobeCounter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena State", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AArenaCharacter> ArenaCharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena State", meta = (AllowPrivateAccess = "true"))
	float BlueGlobeMinSpawnDistance = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena State", meta = (AllowPrivateAccess = "true"))
	float BlueGlobeMaxSpawnDistance = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arena State", meta = (AllowPrivateAccess = "true"))
	float GlobeSphereRadius = 51.f;

	/*----------------------------------------------------------------------------
		Internal functions
	----------------------------------------------------------------------------*/
	FVector GetBlueGlobeSpawnLocation(FVector CenterLocation);

};



