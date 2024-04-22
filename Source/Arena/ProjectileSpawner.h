#pragma once

#include "Components/SceneComponent.h"
#include "CoreMinimal.h"
#include "ArenaGameMode.h"
#include "ProjectileSpawner.generated.h"

class AProjectileBase;
class AArenaGameMode;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UProjectileSpawner : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UProjectileSpawner();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Pattern System", meta = (ToolTip = "Enter fire delays for each step. If the given value is less then 0.2, it will be overwritten as 0.2!"))
	TArray<float> FireDelays;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void Fire();

private:

	AArenaGameMode* GameMode;

	UPROPERTY(EditAnywhere, Category = "Arena", meta = (AllowBlueprintAccess = "true"))
	TSubclassOf<AProjectileBase> ProjectileClass;

	float FireTimer = 0.f;

	// Called everytime the game state changes. We use it to execute initial fire on Play
	UFUNCTION()
	void OnNewGameModeState(EGameStates NewState);

	float GetNewFireInterval();

	///////////////////////////////
	/// Stable Random Type
	///////////////////////////////

	UPROPERTY(EditAnywhere, Category = "Arena - Stable Random", meta = (AllowBlueprintAccess = "true"))
	bool IsFireOffsetActive = false;

	UPROPERTY(EditAnywhere, Category = "Arena - Stable Random", meta = (AllowBlueprintAccess = "true", UIMin = "0.1", UIMax = "5.0"))
	float FireOffset = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Arena - Stable Random", meta = (AllowBlueprintAccess = "true", UIMin = "0.1", UIMax = "10.0"))
	float MinimumFireDelay = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Arena - Stable Random", meta = (AllowBlueprintAccess = "true", UIMin = "0.1", UIMax = "10.0"))
	float MaximumFireDelay = 10.0f;

	float CurrentFireInterval;

	///////////////////////////////
	/// Pattern System
	///////////////////////////////

	UPROPERTY(EditAnywhere, Category = "Arena - Pattern System", meta = (AllowBlueprintAccess = "true"))
	bool IsPatternSystemActive = false;

	// Counter for elements of the FireDelays array
	int DelayIndex = -1; // Start by -1 to make it start with 0 in the GetPatternIndex

	int GetPatternIndex();
};