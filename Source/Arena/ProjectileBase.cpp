// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "ArenaCharacter.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

DEFINE_LOG_CATEGORY(LogArnProjectile);

// Sets default values
AProjectileBase::AProjectileBase()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	TrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail Particle"));
	TrailParticle->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	MovementComponent->MaxSpeed = Speed;
	MovementComponent->InitialSpeed = Speed;
	MovementComponent->ProjectileGravityScale = GravityScale;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);

	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);

	if (ProjectileType != EProjectileType::Regular)
	{
		MovementComponent->Deactivate();

		if (ProjectileType == EProjectileType::TypeZ)
		{
			// Set the initial target
			TypeZ_TargetLocation = GetActorLocation() + GetActorForwardVector() * TypeZ_DistanceX;
		}
	}
}

void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (ProjectileType)
	{
	case EProjectileType::TypeZ:
		HandleTypeZ(DeltaTime);
		break;
	default:
		// UE_LOG(LogArnProjectile, Warning, TEXT("The unsupported type selected!"));
		break;
	}
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* MyOwner = GetOwner();

	// defensive
	if (MyOwner == nullptr)
	{
		Destroy(); return;
	}

	AController* MyOwnerInst = MyOwner->GetInstigatorController();
	UClass* DamageTypeClass = UDamageType::StaticClass();	// We don't specify any class yet

	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInst, this, DamageTypeClass);

		if (HitParticle == nullptr)
		{
			UE_LOG(LogArnProjectile, Error, TEXT("The HitParticle is not set on the ProjectileBase!"));
			Destroy();
			return;
		}

		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, GetActorLocation(), GetActorRotation());

		if (HitSound == nullptr)
		{
			// UE_LOG(LogArnProjectile, Error, TEXT("The HitSound is not set on the ProjectileBase!"));
			Destroy();
			return;
		}

		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}

	Destroy();
}

void AProjectileBase::HandleTypeZ(float DeltaTime)
{
	// Start movement
	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TypeZ_TargetLocation, DeltaTime, TypeZ_MovementSpeed);
	SetActorLocation(NewLocation);

	// Check if the actor has reached the target location, if so, set a new target
	float DistanceToTarget = FVector::DistSquared(CurrentLocation, TypeZ_TargetLocation);
	if (DistanceToTarget < FMath::Square(1.0f))
	{
		if (TypeZ_Step == ETypeZSteps::StepOne)
		{
			TypeZ_TargetLocation = GetActorLocation() + (-GetActorRightVector() * TypeZ_DistanceY);
			TypeZ_Step = ETypeZSteps::StepTwo;
		}
		else if (TypeZ_Step == ETypeZSteps::StepTwo)
		{
			TypeZ_TargetLocation = GetActorLocation() + GetActorForwardVector() * TypeZ_DistanceX;
			TypeZ_Step = ETypeZSteps::StepThree;
		}
		else if (TypeZ_Step == ETypeZSteps::StepThree)
		{
			TypeZ_TargetLocation = GetActorLocation() + GetActorRightVector() * TypeZ_DistanceY;
			TypeZ_Step = ETypeZSteps::StepFour;
		}
		else if (TypeZ_Step == ETypeZSteps::StepFour)
		{
			TypeZ_TargetLocation = GetActorLocation() + GetActorForwardVector() * TypeZ_DistanceX;
			TypeZ_Step = ETypeZSteps::StepOne;
		}
	}
}

void AProjectileBase::Explode()
{
	if (HitParticle == nullptr)
	{
		UE_LOG(LogArnProjectile, Error, TEXT("The HitParticle is not set on the ProjectileBase!"));
		Destroy();
		return;
	}
	UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, GetActorLocation(), GetActorRotation());

	if (HitSound == nullptr)
	{
		// UE_LOG(LogArnProjectile, Error, TEXT("The HitSound is not set on the ProjectileBase!"));
		Destroy();
		return;
	}
	UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());

	Destroy();
}

