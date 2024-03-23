#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProjectileSpawner.generated.h"

class AProjectileBase;
class AArenaGameMode;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UProjectileSpawner : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectileSpawner();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
    virtual void BeginPlay() override;
	void Fire();
		
private:

	AArenaGameMode* GameMode;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true"))
	TSubclassOf<AProjectileBase> ProjectileClass;

	float FireTimer = 0.f;

	// Called everytime the game state changes. We use it to execute initial fire on Play
	UFUNCTION()
	void OnNewGameModeState(EGameStates NewState);

	///////////////////////////////
	/// Stable Random Type
	///////////////////////////////

	UPROPERTY(EditAnywhere, Category = "Arena Combat - Stable Random", meta = (AllowBlueprintAccess = "true"))
	bool bIsFireOffsetActive = false;

	UPROPERTY(EditAnywhere, Category = "Arena Combat - Stable Random", meta = (AllowBlueprintAccess = "true", UIMin = "0.1", UIMax = "5.0"))
	float FireOffset = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Arena Combat - Stable Random", meta = (AllowBlueprintAccess = "true", UIMin = "0.1", UIMax = "10.0"))
	float MinimumFireDelay = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Arena Combat - Stable Random", meta = (AllowBlueprintAccess = "true", UIMin = "0.1", UIMax = "10.0"))
	float MaximumFireDelay = 10.0f;

	float CurrentFireInterval;

};