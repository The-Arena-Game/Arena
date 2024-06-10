#pragma once

#include "CoreMinimal.h"
#include "ProjectileSpawner.h"
#include "ProjectileSpawnerAngry.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UProjectileSpawnerAngry : public UProjectileSpawner
{
	GENERATED_BODY()

public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

};