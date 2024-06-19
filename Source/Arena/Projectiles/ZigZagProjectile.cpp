// Fill out your copyright notice in the Description page of Project Settings.


#include "ZigZagProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"


AZigZagProjectile::AZigZagProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	ProjectileType = EProjectileType::ZigZag;
}

void AZigZagProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(MovementComponent))
	{
		MovementComponent->Deactivate();
	}

	// Set the initial target
	TargetLocation = GetActorLocation() + (GetActorForwardVector() * DistanceX / 2) + (GetActorRightVector() * DistanceY / 2);
}

void AZigZagProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	HandleMovement(DeltaSeconds);
}

void AZigZagProjectile::HandleMovement(float DeltaSeconds)
{
	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaSeconds, MovementSpeed);
	SetActorLocation(NewLocation);

	// Check if the actor has reached the target location, if so, set a new target
	float DistanceToTarget = FVector::DistSquared(CurrentLocation, TargetLocation);
	if (DistanceToTarget < FMath::Square(1.0f))
	{
		if (IsMovingRight)
		{
			TargetLocation = GetActorLocation() + (GetActorForwardVector() * DistanceX) - (GetActorRightVector() * DistanceY);
		}
		else
		{
			TargetLocation = GetActorLocation() + (GetActorForwardVector() * DistanceX) + (GetActorRightVector() * DistanceY);
		}

		IsMovingRight = !IsMovingRight;
	}
}