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

	UPROPERTY(EditDefaultsOnly, Category = "Class Effects")
	USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Class Effects")
	UParticleSystem* HitParticle;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Effects")
	UParticleSystemComponent* TrailParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	float Damage = 10.f;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
