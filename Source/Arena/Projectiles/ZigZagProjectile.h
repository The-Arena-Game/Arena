// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "ZigZagProjectile.generated.h"

/**
 *
 */
UCLASS()
class ARENA_API AZigZagProjectile : public AProjectileBase
{
	GENERATED_BODY()

public:
	AZigZagProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Arena ZigZag", meta = (AllowPrivateAccess = "true"))
	float DistanceX = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena ZigZag", meta = (AllowPrivateAccess = "true"))
	float DistanceY = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena ZigZag", meta = (AllowPrivateAccess = "true"))
	float MovementSpeed = 400.f;

	enum class ETypeVSteps : uint8
	{
		StepZero, StepOne, StepTwo
	};

	ETypeVSteps Step = ETypeVSteps::StepZero;

	bool bMovingRight = true;

	FVector TargetLocation = FVector::Zero();

	UFUNCTION()
	void HandleMovement(float DeltaSeconds);

};
