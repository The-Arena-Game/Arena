#pragma once

#include "CoreMinimal.h"
#include "ProjectileSpawner.h"
#include "ProjectileSpawnerLaser.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UProjectileSpawnerLaser : public UProjectileSpawner
{
	GENERATED_BODY()

public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Laser Type")
	TArray<float> LaserPatternValues;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void Fire() override;

private:

	TArray<float> VfxToggleTimes;

	void SetVfxToggleTimes();

	void HandleVfx();
};