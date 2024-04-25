// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArnProjectile, Log, All);

class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	Regular		UMETA(DisplayName = "Regular Projectile Type"),
	TypeZ		UMETA(DisplayName = "Type Z Projectile"),
	TypeV		UMETA(DisplayName = "Type V Projectile"),
	TypeS		UMETA(DisplayName = "Type S Projectile"),
};

UCLASS()
class AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AProjectileBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	// Bullet Type Dropdown
	UFUNCTION()
	TArray<EProjectileType> GetProjectileTypeOptions()
	{
		return { EProjectileType::Regular, EProjectileType::TypeZ, EProjectileType::TypeV, EProjectileType::TypeS };
	}

	UPROPERTY(EditDefaultsOnly, Category = "Arena Type", meta = (AllowPrivateAccess = "true", GetOptions = "GetProjectileTypeOptions"))
	EProjectileType ProjectileType = EProjectileType::Regular;

	UFUNCTION(BlueprintCallable, Category = "Arena Function")
	void Explode();

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Combat")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, Category = "Class Combat", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Class Effects")
	UParticleSystem* HitParticle;

	UPROPERTY(VisibleAnywhere, Category = "Class Effects")
	UParticleSystemComponent* TrailParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float LifeSpan = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float Speed = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float GravityScale = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	float Damage = 10.f;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	///////////////////////////////
	/// Type Z
	///////////////////////////////
	UPROPERTY(EditDefaultsOnly, Category = "Arena Type - Z", meta = (AllowPrivateAccess = "true"))
	float TypeZ_DistanceX = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Type - Z", meta = (AllowPrivateAccess = "true"))
	float TypeZ_DistanceY = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Type - Z", meta = (AllowPrivateAccess = "true"))
	float TypeZ_MovementSpeed = 250.f;

	enum class ETypeZSteps : uint8
	{
		StepOne, StepTwo, StepThree, StepFour
	};

	ETypeZSteps TypeZ_Step = ETypeZSteps::StepOne;

	FVector TypeZ_TargetLocation = FVector::Zero();

	void HandleTypeZ(float DeltaTime);

	///////////////////////////////
	/// Type V
	///////////////////////////////
	UPROPERTY(EditDefaultsOnly, Category = "Arena Type - V", meta = (AllowPrivateAccess = "true"))
	float TypeV_DistanceX = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Type - V", meta = (AllowPrivateAccess = "true"))
	float TypeV_DistanceY = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Type - V", meta = (AllowPrivateAccess = "true"))
	float TypeV_MovementSpeed = 400.f;

	enum class ETypeVSteps : uint8
	{
		StepZero, StepOne, StepTwo
	};

	ETypeVSteps TypeV_Step = ETypeVSteps::StepZero;

	bool IsMovingRight = true;

	FVector TypeV_TargetLocation = FVector::Zero();

	void HandleTypeV(float DeltaTime);

	///////////////////////////////
	/// Type S
	///////////////////////////////
	UPROPERTY(EditDefaultsOnly, Category = "Arena Type - S", meta = (AllowPrivateAccess = "true"))
	float TypeS_DistanceX = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Type - S", meta = (AllowPrivateAccess = "true"))
	float TypeS_DistanceY = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Type - S", meta = (AllowPrivateAccess = "true"))
	float TypeS_SpeedX = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Type - S", meta = (AllowPrivateAccess = "true"))
	float TypeS_SpeedY = 5.f;

	float TypeS_Timer = 0.f;

	FVector TypeS_TargetLocation = FVector::Zero();

	void HandleTypeS(float DeltaTime);
};
