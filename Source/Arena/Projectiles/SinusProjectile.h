// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "SinusProjectile.generated.h"

/**
 *
 */
UCLASS()
class ARENA_API ASinusProjectile : public AProjectileBase
{
	GENERATED_BODY()

public:
	ASinusProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Arena Sinus", meta = (AllowPrivateAccess = "true"))
	float DistanceX = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Sinus", meta = (AllowPrivateAccess = "true"))
	float DistanceY = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Sinus", meta = (AllowPrivateAccess = "true"))
	float SpeedX = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Sinus", meta = (AllowPrivateAccess = "true"))
	float SpeedY = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Sinus", meta = (AllowPrivateAccess = "true"))
	bool bReverse = false;

	float Timer = 0.f;

	FVector TargetLocation = FVector::Zero();

	UFUNCTION()
	void HandleMovement(float DeltaSeconds);
};
