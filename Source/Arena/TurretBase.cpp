// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaCharacter.h"
#include "ArenaGameMode.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "TurretBase.h"

DEFINE_LOG_CATEGORY(LogArnTurretBase);

// Sets default values
ATurretBase::ATurretBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and setup components
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root Capsule"));
	RootComponent = CapsuleComp;
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ForbidenAreaBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Forbiden Area to Spawn Blue"));
	ForbidenAreaBoxComp->SetupAttachment(RootComponent);
	ForbidenAreaBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	// Set Turret and Base Meshes' collisions to custom and set the "Visibility" to "Overlap" to avoid tracing to see itself when targeting!
	BaseMesh->SetCollisionProfileName("Custom");
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BaseMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BaseMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	TurretMesh->SetCollisionProfileName("Custom");
	TurretMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TurretMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TurretMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void ATurretBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	GameMode = Cast<AArenaGameMode>(UGameplayStatics::GetGameMode(this));
	GameMode->OnGameStateChange.AddDynamic(this, &ATurretBase::OnGameStateChange);
}

// Called every frame
void ATurretBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Don't execute any action further if the game state is not Play state
	if (IsValid(GameMode) && GameMode->GetGameState() != EGameStates::Play)
	{
		return;
	}

	if (IsFollowerTurret && InFireRange())
	{
		RotateTurret(PlayerCharacter->GetActorLocation(), DeltaTime);
	}
}

void ATurretBase::RotateTurret(FVector TargetLocation, float& DeltaTime)
{
	// Create the vector for the target
	FVector ToTarget = TargetLocation - TurretMesh->GetComponentLocation();

	// Get the rotation and reset the Pitch and Roll, Just apply the Yaw
	FRotator TargetRotation = ToTarget.Rotation();
	TargetRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

	// Apply the rotation with interpolation to have control over the speed and smoothness
	TurretMesh->SetWorldRotation(FMath::RInterpTo(
		TurretMesh->GetComponentRotation(),
		TargetRotation,
		UGameplayStatics::GetWorldDeltaSeconds(this),
		TurnSpeed
	));
}

bool ATurretBase::InFireRange()
{
	if (!IsValid(PlayerCharacter))
	{
		return false;
	}

	float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());

	// If in the TurretRange, turn true
	if (Distance < Range)
	{
		return true;
	}

	return false;
}

bool ATurretBase::IsFacingToTarget()
{
	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(LogArnTurretBase, Error, TEXT("PlayerCharacter is null on TurretBase!"));
		return false;
	}

	// Calculate the end location for the line trace	Start + Foward * Range
	FVector TraceStart = TurretMesh->GetComponentLocation();
	FVector TraceEnd = TurretMesh->GetComponentLocation() + (TurretMesh->GetForwardVector() * Range);

	// Draw a channel with radius of 10
	float Radius = 10.0f;

	// Perform the line trace
	FHitResult HitResult;
	FCollisionShape CollShpere = FCollisionShape::MakeSphere(Radius);
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult, TraceStart, TraceEnd, FQuat::Identity, ECC_Visibility, CollShpere);

	// Check if the line trace hit something and that thing is the PlayerCharacter
	if (bHit && HitResult.GetActor() == PlayerCharacter)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ATurretBase::OnGameStateChange(EGameStates NewState)
{
	if (NewState == EGameStates::Prepare)
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ForbidenAreaBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ForbidenAreaBoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}
