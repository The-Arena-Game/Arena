// Fill out your copyright notice in the Description page of Project Settings.


#include "SinusProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASinusProjectile::ASinusProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	ProjectileType = EProjectileType::Sinus;
}

void ASinusProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(MovementComponent))
	{
		MovementComponent->Deactivate();
	}

	// Set the wave offset to center the wave
	Timer += DistanceY / 2;

	if (bReverse)
	{
		DistanceX = -DistanceX;
	}
}

void ASinusProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	HandleMovement(DeltaSeconds);
}

void ASinusProjectile::HandleMovement(float DeltaSeconds)
{
	Timer += DeltaSeconds * SpeedY;
	float ValueY = DistanceY * FMath::Sin(Timer / DistanceX);
	FVector FromZero = FVector(SpeedX, ValueY, 0.f);
	AddActorLocalOffset(FromZero);
}
