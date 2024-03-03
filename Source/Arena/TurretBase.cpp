// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretBase.h"
#include "ArenaCharacter.h"
#include "ArenaGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "DrawDebugHelpers.h"
#include "ProjectileBase.h"

DEFINE_LOG_CATEGORY(LogArnTurretBase);

// Sets default values
ATurretBase::ATurretBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and setup components
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root Capsule"));
	RootComponent = CapsuleComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);

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

	ProjectileSpawnPoints = ProjectileSpawnPoint->GetAttachChildren();
	//ProjectileSpawnPoint->GetChildrenComponents(true, ProjectileSpawnPoints);
}

// Called every frame
void ATurretBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDrawLines)
	{
		DrawLines();
	}

	// Don't execute any action further if the game state is not Play state
	if (GameMode->GetGameState() != EGameStates::Play)
	{
		return;
	}

	if (TurretType == ETurretType::StableRandom)
	{
		HandleStableRandomType(DeltaTime);
	}
	else if (TurretType == ETurretType::Follower)
	{
		HandleFollowerType(DeltaTime);
	}
}

void ATurretBase::HandleFollowerType(float DeltaTime)
{
	FireTimer += DeltaTime;
	if (InFireRange())
	{
		RotateTurret(PlayerCharacter->GetActorLocation(), DeltaTime);

		if (FireTimer >= (1 / FireRate) && IsFacingToTarget()) {
			FireTimer = 0;
			Fire();
		}
	}
}

void ATurretBase::HandleStableRandomType(float DeltaTime)
{
	// If the interval not set yet, set it.
	if (CurrentFireInterval == 0)
	{
		CurrentFireInterval = FMath::RandRange(MinimumFireDelay, MaximumFireDelay);
	}

	FireTimer += DeltaTime;

	if (FireTimer >= CurrentFireInterval) {
		FireTimer = 0;
		CurrentFireInterval = FMath::RandRange(MinimumFireDelay, MaximumFireDelay);
		Fire();
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

void ATurretBase::Fire()
{
	if (ProjectileClass == nullptr)
	{
		UE_LOG(LogArnTurretBase, Error, TEXT("The Projectile Class is not set on the turret!"));
		return;
	}

	if (ProjectileSpawnPoints.Num() == 0)
	{
		UE_LOG(LogArnTurretBase, Error, TEXT("There is no projectile spawn point attached on %s!"), *GetActorNameOrLabel());
		return;
	}

	// Spawn bullet from all spawn points
	for (USceneComponent* SpawnPoint : ProjectileSpawnPoints)
	{
		FVector ProjectileLocation = SpawnPoint->GetComponentLocation();
		FRotator ProjectileRotation = SpawnPoint->GetComponentRotation();

		// Spawn projectile and hold its instance in Projectile Variable
		AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(
			ProjectileClass, ProjectileLocation, ProjectileRotation
		);

		Projectile->SetOwner(this); // Attaching the projectile to the owner, so we can access it in case of a hit
	}	
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

	// Calculate the radius from the FireAccuracy
	float Radius = 100.1f - FireAccuracy;

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

void ATurretBase::DrawLines()
{
	if (ProjectileSpawnPoints.Num() == 0)
	{
		UE_LOG(LogArnTurretBase, Error, TEXT("There is no projectile spawn point attached on %s!"), *GetActorNameOrLabel());
		return;
	}

	for (USceneComponent* SpawnPoint : ProjectileSpawnPoints)
	{
		// Fire Sweep Channel
		FVector TraceStart = SpawnPoint->GetComponentLocation();
		FVector TraceEnd = SpawnPoint->GetComponentLocation() + (SpawnPoint->GetForwardVector() * Range);

		// Calculate the radius from the FireAccuracy
		float Radius = 100.1f - FireAccuracy;
		DrawSphereSweep(GetWorld(), TraceStart, TraceEnd, Radius, true, true, true, -1);

		// Range
		DrawDebugSphere(GetWorld(), TraceStart, Range, 32, FColor::Red, false, -1);
	}	
}

void ATurretBase::DrawSphereSweep(const UWorld* InWorld, const FVector& Start, const FVector& End, const float Radius, const bool DrawOnX, const bool DrawOnZ, const bool DrawSpheres, float Lifetime)
{
	FColor Color = FColor::White;
	// Spheres at start and end
	if (DrawSpheres)
	{
		DrawDebugSphere(InWorld, Start, Radius, FMath::Max(Radius / 4.f, 2.f), Color, false, Lifetime);
		DrawDebugSphere(InWorld, End, Radius, FMath::Max(Radius / 4.f, 2.f), Color, false, Lifetime);
	}

	Color = FColor::Red;
	// Up and down lines
	if (DrawOnZ)
	{
		DrawDebugLine(InWorld, Start + FVector(0, 0, Radius), End + FVector(0, 0, Radius), Color, false, Lifetime);
		DrawDebugLine(InWorld, Start - FVector(0, 0, Radius), End - FVector(0, 0, Radius), Color, false, Lifetime);
	}

	// Left and Right lines
	if (DrawOnX)
	{
		DrawDebugLine(InWorld, Start + FVector(Radius, 0, 0), End + FVector(Radius, 0, 0), Color, false, Lifetime);
		DrawDebugLine(InWorld, Start - FVector(Radius, 0, 0), End - FVector(Radius, 0, 0), Color, false, Lifetime);
	}
}

