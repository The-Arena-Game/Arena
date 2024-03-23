// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArnTurretBase, Log, All);

class UCapsuleComponent;
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

	UPROPERTY(EditAnywhere, Category = "Arena")
	bool IsPreviewBP = false;

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
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UStaticMeshComponent* TurretMesh;

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
};
