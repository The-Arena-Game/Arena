#pragma once

#include "CoreMinimal.h"
#include "ProjectileSpawner.h"
#include "ProjectileSpawnerAngry.generated.h"

class AArenaCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UProjectileSpawnerAngry : public UProjectileSpawner
{
	GENERATED_BODY()

public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Arena - Angry Type")
	TArray<float> DistanceLevels;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Arena - Angry Type")
	TArray<float> FireDelayLevels;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	UPROPERTY()
	AArenaCharacter* Player;

	UPROPERTY(EditAnywhere, Category = "Arena - Angry Type", meta = (AllowPrivateAccess = "true"))
	bool DisplayDebugLines;

	UFUNCTION()
	void HandleDistanceCheck();

	UFUNCTION()
	void HandleDebugLines();

	UFUNCTION()
	void HandleDistanceChecks();

	UPROPERTY()
	float CurrentMaxDelay = 0.f;
};