// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArenaBaseData.h"
#include "Components/ActorComponent.h"
#include "CardManagementComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArnCardManagement, Log, All);

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

	/** Generates 3 Card Data with the algorithm based on Level Number */
	UFUNCTION()
	void GetCardData(const int& Level, TArray<FCardData>& CardData);

	UPROPERTY()
	TMap<ERarity, FRarityPools> Pools;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	TArray<ETurretType> CommonTurretPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	TArray<ETurretType> RareTurretPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	TArray<ETurretType> EpicTurretPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	TArray<ETurretType> LegendaryTurretPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	TArray<FArenaBuff> CommonBuffPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	TArray<FArenaBuff> RareBuffPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	TArray<FArenaBuff> EpicBuffPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	TArray<FArenaBuff> LegendaryBuffPool;

protected:

	virtual void BeginPlay() override;
};
