// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArenaBaseData.h"
#include "Components/ActorComponent.h"
#include "CardManagementComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArnCardManagement, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurretSelected, const ETurretType&, Type);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuffSelected, const FArenaBuff&, Buff);

/* This is a CardManagementComponent specific struct to manage pools */
USTRUCT()
struct FRarityPools
{
	GENERATED_BODY()

	FRarityPools() = default;
	FRarityPools(const ERarity& InRarity, const TArray<ETurretType>& InTurretPool, const TArray<FArenaBuff>& InBuffPool)
	{
		Rarity = InRarity;
		TurretPool = InTurretPool;
		BuffPool = InBuffPool;
	};

	UPROPERTY()
	ERarity Rarity = ERarity::Common;

	UPROPERTY()
	TArray<ETurretType> TurretPool = TArray<ETurretType>();

	UPROPERTY()
	TArray<FArenaBuff> BuffPool = TArray<FArenaBuff>();
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARENA_API UCardManagementComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	/** Broadcasts the selected turret type */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnTurretSelected OnTurretSelected;

	/** Broadcasts the selected Buff */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnBuffSelected OnBuffSelected;

	/** Generates 3 Card Data with the algorithm based on Level Number */
	UFUNCTION()
	void GenerateCardData(const uint8& Level);

	UPROPERTY()
	TMap<ERarity, FRarityPools> Pools;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Turret Pools")
	TArray<ETurretType> CommonTurretPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Turret Pools")
	TArray<ETurretType> RareTurretPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Turret Pools")
	TArray<ETurretType> EpicTurretPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Turret Pools")
	TArray<ETurretType> LegendaryTurretPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Buff Pools")
	TArray<FArenaBuff> CommonBuffPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Buff Pools")
	TArray<FArenaBuff> RareBuffPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Buff Pools")
	TArray<FArenaBuff> EpicBuffPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Buff Pools")
	TArray<FArenaBuff> LegendaryBuffPool;

protected:

	virtual void BeginPlay() override;

private:

	bool RareUnlocked = false;
	bool EpicUnlocked = false;
	bool LegendaryUnlocked = false;

	UPROPERTY(EditDefaultsOnly, Category = "Arena - Rarity Unlocks", meta = (AllowPrivateAccess = "true"))
	int RareUnlockLevel = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Arena - Rarity Unlocks", meta = (AllowPrivateAccess = "true"))
	int EpicUnlockLevel = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Arena - Rarity Unlocks", meta = (AllowPrivateAccess = "true"))
	int LegendaryUnlockLevel = 15;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FCardData> CardsData;

	UPROPERTY(EditDefaultsOnly, Category = "Arena - Rarity Configuration", meta = (AllowPrivateAccess = "true"))
	float CommonBaseValue = 75.f;
	UPROPERTY(EditDefaultsOnly, Category = "Arena - Rarity Configuration", meta = (AllowPrivateAccess = "true"))
	float CommonGrowthRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena - Rarity Configuration", meta = (AllowPrivateAccess = "true"))
	float RareBaseValue = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Arena - Rarity Configuration", meta = (AllowPrivateAccess = "true"))
	float RareGrowthRate = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena - Rarity Configuration", meta = (AllowPrivateAccess = "true"))
	float EpicBaseValue = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Arena - Rarity Configuration", meta = (AllowPrivateAccess = "true"))
	float EpicGrowthRate = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena - Rarity Configuration", meta = (AllowPrivateAccess = "true"))
	float LegendaryBaseValue = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Arena - Rarity Configuration", meta = (AllowPrivateAccess = "true"))
	float LegendaryGrowthRate = 0.4f;

	UFUNCTION()
	static float GetWeight(const uint8& Level, const float& BaseValue, const float& GrowthRate)
	{
		return BaseValue * FMath::Pow((1 + GrowthRate), Level);
	}

	UFUNCTION()
	ERarity GetRarity(const uint8& Level) const;

	UFUNCTION()
	void BuffSelected(const FArenaBuff& InBuff);

	UFUNCTION()
	void CheckLevelUnlocks(const uint8& Level);
};
