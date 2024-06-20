// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arena/Core/ArenaBaseData.h"
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
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, Category = "Arena Type", meta = (AllowPrivateAccess = "true"))
	EProjectileType ProjectileType = EProjectileType::Regular;

	UPROPERTY(EditAnywhere, Category = "Class Combat", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* MovementComponent;

private:

	UFUNCTION(BlueprintCallable, Category = "Arena Function")
	void Explode();

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Combat")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Class Effects")
	UParticleSystem* HitParticle;

	UPROPERTY(VisibleAnywhere, Category = "Class Effects")
	UParticleSystemComponent* TrailParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float LifeSpan = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	float Damage = 10.f;

	/*--------------------------
			Utils
	--------------------------*/

	UFUNCTION()
	UClass* GetDamageType() const;

};
