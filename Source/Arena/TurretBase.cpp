// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaCharacter.h"
#include "Components/CapsuleComponent.h"
#include "TurretBase.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
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
}

// Called when the game starts or when spawned
void ATurretBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &ATurretBase::CheckFire, FireRate, true);
}

// Called every frame
void ATurretBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(PlayerCharacter) && InFireRange())
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

void ATurretBase::Fire()
{
	if (ProjectileClass == nullptr)
	{
		UE_LOG(LogArnTurretBase, Error, TEXT("The Projectile Class is not set on the turret!"));
		return;
	}

	FVector ProjectileLocation = ProjectileSpawnPoint->GetComponentLocation();
	FRotator ProjectileRotation = ProjectileSpawnPoint->GetComponentRotation();

	// Spawn projectile and hold its instance in Projectile Variable
	AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(
		ProjectileClass, ProjectileLocation, ProjectileRotation
	);

	Projectile->SetOwner(this); // Attaching the projectile to the owner, so we can access it in case of a hit
}

void ATurretBase::CheckFire()
{
	if (InFireRange() && PlayerCharacter) { Fire(); }
}

bool ATurretBase::InFireRange()
{
	if (IsValid(PlayerCharacter))
	{
		float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());

		// If in the TurretRange, turn true
		if (Distance < TurretRange)
		{
			return true;
		}
		return false;
	}
	return false;
}

