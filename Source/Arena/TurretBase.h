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
	UPROPERTY(VisibleDefaultsOnly, Category = "Arena Component")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Arena Component")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Arena Component")
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Arena Component")
	USceneComponent* ProjectileSpawnPoint;

	// Effects
	UPROPERTY(VisibleDefaultsOnly, Category = "Arena Effects")
	USoundBase* FireSound;

	///*************************** 
	///			Combat
	///*************************** 

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true"))
	TSubclassOf<AProjectileBase> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true"))
	float TurretRange = 500.f;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true"))
	float TurnSpeed = 3.f;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowBlueprintAccess = "true"))
	float FireRate = 1.f;

	AArenaCharacter* PlayerCharacter;
	void CheckFire();
	bool InFireRange();
};
