// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "Arena/Core/ArenaCharacter.h"
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
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);

	if (IsValid(ProjectileMesh))
	{
		ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
	}

	if (ProjectileType == EProjectileType::Laser)
	{
		MovementComponent->Deactivate();
	}
	else if (ProjectileType == EProjectileType::Homing)
	{
		if (AArenaCharacter* Character = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
		{
			MovementComponent->HomingTargetComponent = Character->GetRootComponent();
		}
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
	UClass* DamageTypeClass = GetDamageType();

	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInst, this, DamageTypeClass);

		if (HitParticle == nullptr)
		{
			Destroy();
			return;
		}

		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, Hit.Location, GetActorRotation());

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

void AProjectileBase::Explode()
{
	if (HitParticle == nullptr)
	{
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

UClass* AProjectileBase::GetDamageType() const
{
	switch (ProjectileType)
	{
	case EProjectileType::TurtleTemp:
		return UTurtleTempDamageType::StaticClass();
		break;
	case EProjectileType::TurtlePerm:
		return UTurtlePermDamageType::StaticClass();
		break;
	case EProjectileType::Darkness:
		return UDarknessDamageType::StaticClass();
		break;
	case EProjectileType::Exhaustion:
		return UExhaustionDamageType::StaticClass();
		break;
	default:
		return UDamageType::StaticClass();
	}
}

