// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "SquareSinusProjectile.generated.h"

/**
 *
 */
UCLASS()
class ARENA_API ASquareSinusProjectile : public AProjectileBase
{
	GENERATED_BODY()

public:
	ASquareSinusProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Arena Square Sinus", meta = (AllowPrivateAccess = "true"))
	float DistanceX = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Square Sinus", meta = (AllowPrivateAccess = "true"))
	float DistanceY = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Square Sinus", meta = (AllowPrivateAccess = "true"))
	float MovementSpeed = 250.f;

	enum class ETypeZSteps : uint8
	{
		StepOne, StepTwo, StepThree, StepFour
	};

	ETypeZSteps Step = ETypeZSteps::StepOne;

	FVector TargetLocation = FVector::Zero();

	UFUNCTION()
	void HandleMovement(float DeltaSeconds);

};
