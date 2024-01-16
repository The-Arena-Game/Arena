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

	UPROPERTY(VisibleDefaultsOnly, Category = "Arena Combat")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Effects")
	USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Effects")
	UParticleSystem* HitParticle;

	UPROPERTY(VisibleDefaultsOnly, Category = "Arena Effects")
	UParticleSystemComponent* TrailParticle;

	UPROPERTY(VisibleDefaultsOnly, Category = "Arena Effects")
	TSubclassOf<UCameraShakeBase> HitCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	float Damage = 10.f;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
