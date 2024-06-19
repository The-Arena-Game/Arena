// Fill out your copyright notice in the Description page of Project Settings.


#include "LobProjectile.h"


ALobProjectile::ALobProjectile()
{
	ProjectileType = EProjectileType::Lob;
}

void ALobProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetActorRotation(FRotator::ZeroRotator);
}

void ALobProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	for (USceneComponent* SpawnPoint : RootComponent->GetAttachChildren())
	{
		if (IsValid(SpawnPoint) && SpawnPoint->GetName().Contains("Spawn"))
		{
			// Spawn projectile and hold its instance in Projectile Variable
			AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(
				ProjectileClass, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation());

			// Attaching the projectile to the owner, so we can access it in case of a hit
			Projectile->SetOwner(GetOwner());
		}
	}

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
