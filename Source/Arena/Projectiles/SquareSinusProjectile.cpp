// Fill out your copyright notice in the Description page of Project Settings.


#include "SquareSinusProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASquareSinusProjectile::ASquareSinusProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	ProjectileType = EProjectileType::SquareSinus;
}

void ASquareSinusProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(MovementComponent))
	{
		MovementComponent->Deactivate();
	}

	// Set the initial target
	TargetLocation = GetActorLocation() + GetActorForwardVector() * DistanceX;
}

void ASquareSinusProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	HandleMovement(DeltaSeconds);
}

void ASquareSinusProjectile::HandleMovement(float DeltaSeconds)
{
	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaSeconds, MovementSpeed);
	SetActorLocation(NewLocation);

	// Check if the actor has reached the target location, if so, set a new target
	float DistanceToTarget = FVector::DistSquared(CurrentLocation, TargetLocation);
	if (DistanceToTarget < FMath::Square(1.0f))
	{
		if (Step == ETypeZSteps::StepOne)
		{
			TargetLocation = GetActorLocation() + (-GetActorRightVector() * DistanceY);
			Step = ETypeZSteps::StepTwo;
		}
		else if (Step == ETypeZSteps::StepTwo)
		{
			TargetLocation = GetActorLocation() + GetActorForwardVector() * DistanceX;
			Step = ETypeZSteps::StepThree;
		}
		else if (Step == ETypeZSteps::StepThree)
		{
			TargetLocation = GetActorLocation() + GetActorRightVector() * DistanceY;
			Step = ETypeZSteps::StepFour;
		}
		else if (Step == ETypeZSteps::StepFour)
		{
			TargetLocation = GetActorLocation() + GetActorForwardVector() * DistanceX;
			Step = ETypeZSteps::StepOne;
		}
	}
}
