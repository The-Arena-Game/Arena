// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArnProjectile, Log, All);

class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;

UCLASS()
class AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AProjectileBase();

protected:
	void BeginPlay() override;

private:

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Combat")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, Category = "Class Combat", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Class Effects")
	UParticleSystem* HitParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Class Effects")
	UParticleSystemComponent* TrailParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float Speed = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float GravityScale = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	float Damage = 10.f;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
