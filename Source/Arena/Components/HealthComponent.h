// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "HealthComponent.generated.h"

class AArenaCharacter;
DECLARE_LOG_CATEGORY_EXTERN(LogArnHealthComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange, int, HealthCount, int, MaxHeartCount);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UPROPERTY(BlueprintAssignable)
	FOnHealthChange OnHealthChange;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsDead() const
	{
		return HeartCount <= 0 ? true : false;
	}

	/** Set the Vulnerability Status */
	UFUNCTION()
	FORCEINLINE void SetVulnerable(bool bInVulnerable)
	{
		bVulnerable = bInVulnerable;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetHeartCount() const
	{
		return HeartCount;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetMaxHeartCount() const
	{
		return MaxHeartCount;
	}

	/** Reset the health */
	UFUNCTION()
	FORCEINLINE void ResetHealth()
	{
		MaxHeartCount = InitialMaxHeart;
		HeartCount = MaxHeartCount;
		OnHealthChange.Broadcast(HeartCount, MaxHeartCount);
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void IncreaseHeartCount(const int IncrementAmount)
	{
		HeartCount = FMath::Clamp(HeartCount + IncrementAmount, HeartCount, MaxHeartCount);
		OnHealthChange.Broadcast(HeartCount, MaxHeartCount);
	}

	/**
	 *
	 * @param IncrementAmount Increment amount for a health container
	 * @param bFullHeart Indicates if the given heart container is full or empty. If true, increases the current heart count by IncrementAmount.
	 */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void IncreaseMaxHealth(const int IncrementAmount, const bool bFullHeart)
	{
		MaxHeartCount += IncrementAmount;
		HeartCount += bFullHeart ? IncrementAmount : 0;
		OnHealthChange.Broadcast(HeartCount, MaxHeartCount);
	}

protected:
	void BeginPlay() override;

private:

	UPROPERTY()
	AArenaCharacter* Player;

	UPROPERTY()
	bool bVulnerable = true;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowPrivateAccess = "true", UIMin = "1.0", UIMax = "10.0"))
	int MaxHeartCount = 3;

	UPROPERTY(BlueprintReadOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	int HeartCount;

	UPROPERTY()
	int InitialMaxHeart = 0;

	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser);

	UPROPERTY(EditDefaultsOnly, Category = "Arena Effects", meta = (AllowPrivateAccess = "true"))
	USoundBase* DamageTakenSound;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Effects", meta = (AllowPrivateAccess = "true"))
	USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, Category = "Arena Combat")
	TSubclassOf<UCameraShakeBase> DamageTakenCameraShakeClass;
};
