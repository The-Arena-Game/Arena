// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArnTurretBase, Log, All);

class UCapsuleComponent;
class AProjectileBase;
class AArenaCharacter;

UCLASS()
class ATurretBase : public AActor
{
	GENERATED_BODY()

public:
	ATurretBase();

	virtual void Tick(float DeltaTime) override;

protected:
	void BeginPlay() override;
	void RotateTurret(FVector TargetLocation, float& DeltaTime);
	void Fire();

private:

	///*************************** 
	///			Components
	///*************************** 
	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	USceneComponent* ProjectileSpawnPoint;

	// Effects
	UPROPERTY(VisibleDefaultsOnly, Category = "Class Effects")
	USoundBase* FireSound;

	///*************************** 
	///			Combat
	///*************************** 

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true"))
	TSubclassOf<AProjectileBase> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true", UIMin = "300.0", UIMax = "2000.0"))
	float Range = 500.f;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true", UIMin = "50.0", UIMax = "100.0",
		ToolTip = "Fire accurarcy. Set 100 to have the highest."))
	float FireAccuracy = 100.f;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true", ToolTip = "Per second", UIMin = "0.1", UIMax = "100.0"))
	float FireRate = 1.f;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true", UIMin = "300.0", UIMax = "2000.0"))
	bool bCanFollow = false;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true", UIMin = "1.0", UIMax = "10.0"))
	float TurnSpeed = 3.f;

	float FireTimer = 0.f;

	bool InFireRange();
	bool IsFacingToTarget();

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true"))
	bool bDrawLines = false;

	void DrawLines();
	void DrawSphereSweep(const UWorld* InWorld, const FVector& Start, const FVector& End, const float Radius, const bool DrawOnX, const bool DrawOnZ, const bool DrawSpheres, float Lifetime);

	AArenaCharacter* PlayerCharacter;
};
