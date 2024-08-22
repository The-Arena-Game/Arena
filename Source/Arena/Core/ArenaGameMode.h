// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ArenaBaseData.h"
#include "CoreMinimal.h"
#include "Arena/Turrets/TurretBase.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

class UCardManagementComponent;
class AGlobeBase;
class AArenaCharacter;

DECLARE_LOG_CATEGORY_EXTERN(LogArnGameMode, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRestartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChangeDelegate, EGameStates, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBlueSpawnCollisionStateDelegate, bool, CollisionState);

UCLASS(minimalapi)
class AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenaGameMode();

	/** Called within the RestartArenaGame function to inform other about the restart */
	UPROPERTY(BlueprintAssignable)
	FRestartDelegate OnRestart;

	/** Called within the RestartArenaGame function to inform other about the restart */
	UPROPERTY(BlueprintAssignable)
	FOnStateChangeDelegate OnGameStateChange;

	/** Called right before and after blue globe spawn. Bound funciontion should turn on/off the collision for forbidden areas */
	UPROPERTY(BlueprintAssignable)
	FOnBlueSpawnCollisionStateDelegate OnBlueSpawnCollisionStateChange;

	/** Called from the ArenaCharacter when the character dies. Starts Game Over state. **/
	void HandlePlayerDeath();

	/** Called from the Globes to inform the interaction */
	void YellowTouch(AGlobeBase* Globe);

	/** Called from the Globes to inform the interaction */
	void BlueTouch(AGlobeBase* Globe);

	UFUNCTION(BlueprintCallable, Category = "Arena State")
	FORCEINLINE EGameStates GetArenaGameState() const
	{
		return ArenaGameState;
	}

	UFUNCTION(BlueprintCallable, Category = "Arena State")
	FORCEINLINE void SetGameState(const EGameStates State)
	{
		ArenaGameState = State; OnGameStateChange.Broadcast(State);
	}

	UFUNCTION(Blueprintcallable, Category = "Arena")
	FORCEINLINE int GetLevelNumber() const
	{
		return LevelNumber;
	}

	// Returns the blue globe. Nullptr if the globe is not spawned
	UFUNCTION()
	FORCEINLINE AGlobeBase* GetBlueGlobe() const
	{
		return BlueGlobe;
	}

	// Returns the card management component
	UFUNCTION(Blueprintcallable, Category = "Arena")
	UCardManagementComponent* GetCardComp() const
	{
		return CardManagementComp;
	}

	UFUNCTION(Blueprintcallable, Category = "Arena")
	FORCEINLINE UArenaBaseData* GetBaseData() const
	{
		return BaseData;
	}

	UFUNCTION(BlueprintCallable, Category = "Arena State")
	void Cheat_CollectAllYellows();

	UFUNCTION(BlueprintCallable, Category = "Arena State")
	void Cheat_CollectBlue();

	UFUNCTION(BlueprintCallable, Category = "Arena State")
	void RestartArenaGame();

	// Called from the Turret Slot. Starts the ready state after turret placement is done.
	UFUNCTION(BlueprintCallable, Category = "Arena State")
	void SetReadyState();

protected:

	void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Arena State")
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent, Category = "Arena State")
	void FinishGame();

	UFUNCTION(BlueprintImplementableEvent, Category = "Arena")
	void OpenCardSelectionUI();

	UFUNCTION(BlueprintImplementableEvent, Category = "Arena")
	void CloseCardSelectionUI();

private:

	UPROPERTY(EditDefaultsOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	UCardManagementComponent* CardManagementComp;

	UPROPERTY(EditDefaultsOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	UArenaBaseData* BaseData;

	UPROPERTY()
	APlayerController* PlayerController;
	UPROPERTY()
	AGlobeBase* BlueGlobe;

	uint8 LevelNumber = 1;
	bool AllYellowsEnabled = false;

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
	// Spawns a blue globe
	AGlobeBase* SpawnBlueGlobe(FVector CenterLocation, FRotator SpawnRotation);

	// Retuns a valid location for blue globe to spawn
	FVector GetBlueGlobeSpawnLocation(FVector CenterLocation);
};



