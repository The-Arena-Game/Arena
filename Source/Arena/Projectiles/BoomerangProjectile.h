// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "BoomerangProjectile.generated.h"

/**
 *
 */
UCLASS()
class ARENA_API ABoomerangProjectile : public AProjectileBase
{
	GENERATED_BODY()

public:
	ABoomerangProjectile();
	~ABoomerangProjectile();

protected:
	void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	float RotationSpeed = 1.f;

	UPROPERTY()
	float Angle;

	UPROPERTY()
	FVector StartingPoint;

	UPROPERTY()
	FTimerHandle LocationCheckTimerHandle;

	UPROPERTY()
	bool CheckLocation = false;
};
