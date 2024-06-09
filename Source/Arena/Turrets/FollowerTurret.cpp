// Fill out your copyright notice in the Description page of Project Settings.


#include "FollowerTurret.h"

#include "TurretBase.h"
#include "Arena/Core/ArenaCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

// Called every frame
void AFollowerTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InFireRange())
	{
		RotateTurret(GetPlayerCharacter()->GetActorLocation(), DeltaTime);
	}
}

void AFollowerTurret::RotateTurret(const FVector& TargetLocation, float& DeltaTime)
{
	// Create the vector for the target
	FVector ToTarget = TargetLocation - GetTurretMesh()->GetComponentLocation();

	// Get the rotation and reset the Pitch and Roll, Just apply the Yaw
	FRotator TargetRotation = ToTarget.Rotation();
	TargetRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

	// Apply the rotation with interpolation to have control over the speed and smoothness
	GetTurretMesh()->SetWorldRotation(FMath::RInterpTo(
		GetTurretMesh()->GetComponentRotation(),
		TargetRotation,
		UGameplayStatics::GetWorldDeltaSeconds(this),
		TurnSpeed
	));
}

bool AFollowerTurret::InFireRange() const
{
	if (!IsValid(GetPlayerCharacter()))
	{
		return false;
	}

	float Distance = FVector::Dist(GetActorLocation(), GetPlayerCharacter()->GetActorLocation());

	// If in the TurretRange, turn true
	if (Distance < Range)
	{
		return true;
	}

	return false;
}

bool AFollowerTurret::IsFacingToTarget() const
{
	if (!IsValid(GetPlayerCharacter()))
	{
		UE_LOG(LogArnTurretBase, Error, TEXT("PlayerCharacter is null on TurretBase!"));
		return false;
	}

	// Calculate the end location for the line trace	Start + Foward * Range
	FVector TraceStart = GetTurretMesh()->GetComponentLocation();
	FVector TraceEnd = GetTurretMesh()->GetComponentLocation() + (GetTurretMesh()->GetForwardVector() * Range);

	// Draw a channel with radius of 10
	float Radius = 10.0f;

	// Perform the line trace
	FHitResult HitResult;
	FCollisionShape CollShpere = FCollisionShape::MakeSphere(Radius);
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult, TraceStart, TraceEnd, FQuat::Identity, ECC_Visibility, CollShpere);

	// Check if the line trace hit something and that thing is the PlayerCharacter
	if (bHit && HitResult.GetActor() == GetPlayerCharacter())
	{
		return true;
	}
	else
	{
		return false;
	}
}