// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "ArenaCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
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

	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* MyOwner = GetOwner();
	if (MyOwner == nullptr) { Destroy(); return; } // defensive

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
			UE_LOG(LogArnProjectile, Error, TEXT("The HitSound is not set on the ProjectileBase!"));
			Destroy();
			return;
		}

		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}

	Destroy();
}

