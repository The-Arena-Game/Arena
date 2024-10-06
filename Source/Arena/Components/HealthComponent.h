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

	UFUNCTION(BlueprintCallable)
	FORCEINLINE uint8 GetRegenRoundAmount() const
	{
		return RegenRoundAmount;
	}

	/** Reset the health */
	UFUNCTION()
	FORCEINLINE void ResetHealth()
	{
		MaxHeartCount = InitialMaxHeart;
		HeartCount = MaxHeartCount;
		OnHealthChange.Broadcast(HeartCount, MaxHeartCount);
	}

	UFUNCTION()
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
	UFUNCTION()
	FORCEINLINE void IncreaseMaxHealth(const int IncrementAmount, const bool bFullHeart)
	{
		MaxHeartCount += IncrementAmount;
		HeartCount += bFullHeart ? IncrementAmount : 0;
		OnHealthChange.Broadcast(HeartCount, MaxHeartCount);
	}

	/**
	 * Activates Health Regeneration
	 * @param InRegenHealthAmount Amount of health increase in the given round interval. 1 health means half heart.
	 * @param InRegenRoundAmount The round interval amount to regenerate the given amount of health
	 */
	UFUNCTION()
	FORCEINLINE void ActivateRegen(const uint8 InRegenHealthAmount, const uint8 InRegenRoundAmount)
	{
		RegenHealthAmount = InRegenHealthAmount;
		RegenRoundAmount = InRegenRoundAmount;
		bRegenActive = true;
	}

	UFUNCTION()
	FORCEINLINE void UpgradeRegen()
	{
		if (RegenRoundAmount > 1)
		{
			RegenRoundAmount--;
		}
	}

	UFUNCTION(BlueprintCallable)
	void OnRoundWin();

protected:
	void BeginPlay() override;

private:

	UPROPERTY()
	AArenaCharacter* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Effects", meta = (AllowPrivateAccess = "true"))
	USoundBase* DamageTakenSound;

	UPROPERTY(EditDefaultsOnly, Category = "Arena Effects", meta = (AllowPrivateAccess = "true"))
	USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, Category = "Arena Combat")
	TSubclassOf<UCameraShakeBase> DamageTakenCameraShakeClass;

	// Main Variables

	UPROPERTY()
	bool bVulnerable = true;

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowPrivateAccess = "true", UIMin = "1.0", UIMax = "10.0"))
	int MaxHeartCount = 3;

	UPROPERTY(BlueprintReadOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	int HeartCount;

	UPROPERTY()
	int InitialMaxHeart = 0;

	// Regen Buff Variables

	UPROPERTY()
	bool bRegenActive = false;

	UPROPERTY()
	uint8 RegenHealthAmount = 1;

	UPROPERTY()
	uint8 RegenRoundAmount = 4;

	UPROPERTY()
	uint8 RegenRoundCounter = 1;


	// Functions

	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser);
};
