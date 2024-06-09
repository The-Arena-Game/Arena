// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arena/Core/ArenaBaseData.h"
#include "GameFramework/Actor.h"
#include "TurretBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArnTurretBase, Log, All);

class UCapsuleComponent;
class UBoxComponent;
class AProjectileBase;
class AArenaCharacter;
class AArenaGameMode;

UCLASS()
class ATurretBase : public AActor
{
	GENERATED_BODY()

public:
	ATurretBase();

	void Tick(float DeltaTime) override;

	ETurretType GetTurretType() const
	{
		return TurretType;
	}

	AArenaCharacter* GetPlayerCharacter() const
	{
		return PlayerCharacter;
	}

	UStaticMeshComponent* GetTurretMesh() const
	{
		return TurretMesh;
	}

protected:
	void BeginPlay() override;

private:

	UPROPERTY()
	AArenaCharacter* PlayerCharacter;
	UPROPERTY()
	AArenaGameMode* GameMode;

	/*----------------------------------------------------------------------------
		Components
	----------------------------------------------------------------------------*/

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UBoxComponent* ForbiddenAreaBoxComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UStaticMeshComponent* TurretMesh;

	/*----------------------------------------------------------------------------
		Definition
	----------------------------------------------------------------------------*/

	// Turret Type Dropdown
	UFUNCTION()
	TArray<ETurretType> GetTurretTypeOptions();

	// Turret Type Dropdown
	UPROPERTY(EditAnywhere, Category = "Arena", meta = (AllowBlueprintAccess = "true", GetOptions = "GetTurretTypeOptions"))
	ETurretType TurretType = ETurretType::SingleTurret;

	/*----------------------------------------------------------------------------
		Others
	----------------------------------------------------------------------------*/
	// Called on Game Mode changes GameState. Broadcast listener. Disables collision on Prepare state
	UFUNCTION()
	void OnSpawnCollisionChange(bool NewState);
};
