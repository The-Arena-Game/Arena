// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArnTurretBase, Log, All);

UENUM(BlueprintType)
enum class ETurretType : uint8
{
	SingleTurret    UMETA(DisplayName = "Single Turret"),
	DualTurret		UMETA(DisplayName = "Dual Turret"),
	TwinTurret		UMETA(DisplayName = "Twin Turret"),
	Chonky			UMETA(DisplayName = "Chonky Turret"),
	Gatling			UMETA(DisplayName = "Gatling Turret"),
	Triplet			UMETA(DisplayName = "Triplet Turret"),
	Lazer			UMETA(DisplayName = "Lazer Turret"),
	Knight			UMETA(DisplayName = "Knight Turret"),
	//Sinus deactivated until Sinus Bullet is fixed.
	//Sinus			UMETA(DisplayName = "Sinus Turret"),
	MinusSinus		UMETA(DisplayName = "Minus Sinus Turret"),
};

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

protected:
	void BeginPlay() override;
	void RotateTurret(FVector TargetLocation, float& DeltaTime);

private:

	AArenaCharacter* PlayerCharacter;
	AArenaGameMode* GameMode;

	/*----------------------------------------------------------------------------
		Components
	----------------------------------------------------------------------------*/

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UBoxComponent* ForbidenAreaBoxComp;

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
		Combat
	----------------------------------------------------------------------------*/

	///////////////////////////////
	/// Follower Type
	///////////////////////////////

	UPROPERTY(EditAnywhere, Category = "Arena Combat - Follower", meta = (AllowBlueprintAccess = "true"))
	bool IsFollowerTurret = false;

	UPROPERTY(EditAnywhere, Category = "Arena Combat - Follower", meta = (AllowBlueprintAccess = "true", UIMin = "300.0", UIMax = "2000.0"))
	float Range = 500.f;

	UPROPERTY(EditAnywhere, Category = "Arena Combat - Follower", meta = (AllowBlueprintAccess = "true", UIMin = "1.0", UIMax = "10.0"))
	float TurnSpeed = 3.f;

	bool InFireRange();
	bool IsFacingToTarget();

	/*----------------------------------------------------------------------------
		Others
	----------------------------------------------------------------------------*/
	// Called on Game Mode changes GameState. Broadcast listener. Disables collision on Prepare state
	UFUNCTION()
	void OnSpawnCollisionChange(bool NewState);
};
