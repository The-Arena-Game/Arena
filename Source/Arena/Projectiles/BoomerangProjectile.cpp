// Fill out your copyright notice in the Description page of Project Settings.


#include "BoomerangProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABoomerangProjectile::~ABoomerangProjectile()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}
}

ABoomerangProjectile::ABoomerangProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	ProjectileType = EProjectileType::Boomerang;
}

void ABoomerangProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Override the lifespan with too high value to avoid being destroyed by it.
	SetLifeSpan(100000.f);

	//StartingPoint = GetActorLocation();
	//
	//GetWorld()->GetTimerManager().SetTimer(LocationCheckTimerHandle, [this]()
	//	{
	//		bCheckLocation = true;
	//	}, 2.f, false);
}

void ABoomerangProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// This code destroys the projectile once it turns back to the starting point.
	//if (bCheckLocation)
	//{
	//	if (FVector::Distance(GetActorLocation(), StartingPoint) < 10.f)
	//	{
	//		Destroy();
	//	}
	//}

	Angle -= RotationSpeed * DeltaTime;
	const FVector NewDirection = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f);

	if (IsValid(MovementComponent))
	{
		MovementComponent->Velocity = NewDirection * MovementComponent->InitialSpeed;
	}
}
