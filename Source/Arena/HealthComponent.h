// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDead() const
	{
		return bIsHeartSystemActive ? (HeartCount <= 0 ? true : false) : (Health <= 0 ? true : false);
	}

	/* Set the Vulnerability Status */
	FORCEINLINE void SetValnerable(bool IsVulnerable)
	{
		bIsVulnerable = IsVulnerable;
	}

	/* Set the Vulnerability Status */
	FORCEINLINE void ResetHeatlh()
	{
		HeartCount = MaxHeartCount; Health = MaxHealth;
	}

protected:
	void BeginPlay() override;

private:

	bool bIsVulnerable = true;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowPrivateAccess = "true", UIMin = "1.0", UIMax = "100.0"))
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	float Health;

	// Heart System
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsHeartSystemActive = true;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowPrivateAccess = "true", UIMin = "1.0", UIMax = "10.0"))
	int MaxHeartCount = 3;

	UPROPERTY(BlueprintReadOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	int HeartCount;

	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser);

	UPROPERTY(EditDefaultsOnly, Category = "Arena Effects", meta = (AllowPrivateAccess = "true"))
	USoundBase* DamageTakenSound;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Effects", meta = (AllowPrivateAccess = "true"))
	USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, Category = "Arena Combat")
	TSubclassOf<UCameraShakeBase> DamageTakenCameraShakeClass;
};
