// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ArenaBaseData.generated.h"

/*----------------------------------------------------------------------------
		Enums
----------------------------------------------------------------------------*/

UENUM(BlueprintType)
enum class ETurretType : uint8
{
	None			UMETA(DisplayName = "None"),
	SingleTurret    UMETA(DisplayName = "Single Turret"),
	DualTurret		UMETA(DisplayName = "Dual Turret"),
	TwinTurret		UMETA(DisplayName = "Twin Turret"),
	Chonky			UMETA(DisplayName = "Chonky Turret"),
	Gatling			UMETA(DisplayName = "Gatling Turret"),
	Triplet			UMETA(DisplayName = "Triplet Turret"),
	Lazer			UMETA(DisplayName = "Lazer Turret"),
	Knight			UMETA(DisplayName = "Knight Turret"),
	//Sinus deactivated until Sinus Bullet is fixed.
	//Sinus			UMETA(DisplayName = "Sinus Turret"),
	MinusSinus		UMETA(DisplayName = "Minus Sinus Turret"),
	Angry			UMETA(DisplayName = "Angry Turret"),
};

UENUM(BlueprintType)
enum class EBuffType : uint8
{
	None			UMETA(DisplayName = "None"),
	TestBuff_0		UMETA(DisplayName = "Test Buff Name 0"),
	TestBuff_1		UMETA(DisplayName = "Test Buff Name 1"),
	TestBuff_2		UMETA(DisplayName = "Test Buff Name 2"),
	TestBuff_3		UMETA(DisplayName = "Test Buff Name 3"),
	TestBuff_4		UMETA(DisplayName = "Test Buff Name 4"),
	TestBuff_5		UMETA(DisplayName = "Test Buff Name 5"),
	TestBuff_6		UMETA(DisplayName = "Test Buff Name 6"),
	TestBuff_7		UMETA(DisplayName = "Test Buff Name 7"),
	TestBuff_8		UMETA(DisplayName = "Test Buff Name 8"),
	TestBuff_9		UMETA(DisplayName = "Test Buff Name 9"),
	TestBuff_10		UMETA(DisplayName = "Test Buff Name 10"),
	TestBuff_11		UMETA(DisplayName = "Test Buff Name 11"),
	TestBuff_12		UMETA(DisplayName = "Test Buff Name 12"),
	TestBuff_13		UMETA(DisplayName = "Test Buff Name 13"),
	TestBuff_14		UMETA(DisplayName = "Test Buff Name 14"),
	TestBuff_15		UMETA(DisplayName = "Test Buff Name 15"),
	TestBuff_16		UMETA(DisplayName = "Test Buff Name 16"),
};

UENUM(BlueprintType)
enum class EGameStates : uint8
{
	None    UMETA(DisplayName = "None"),
	Menu    UMETA(DisplayName = "Menu State"),
	Ready	UMETA(DisplayName = "Ready State"),
	Play	UMETA(DisplayName = "Play State"),
	Pause	UMETA(DisplayName = "Pause State"),
	Lost	UMETA(DisplayName = "Lost State"),
	Win		UMETA(DisplayName = "Win State"),
	Prepare UMETA(DisplayName = "Prepare State"),
};

UENUM(BlueprintType)
enum class ERarity : uint8
{
	None		UMETA(DisplayName = "None"),
	Common		UMETA(DisplayName = "Common"),
	Rare		UMETA(DisplayName = "Rare"),
	Epic		UMETA(DisplayName = "Epic"),
	Legendary	UMETA(DisplayName = "Legendary"),
};

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	Regular		UMETA(DisplayName = "Regular Projectile Type"),
	TypeZ		UMETA(DisplayName = "Type Z Projectile"),
	TypeV		UMETA(DisplayName = "Type V Projectile"),
	TypeS		UMETA(DisplayName = "Type S Projectile"),
	Laser		UMETA(DisplayName = "Lazer Projectile"),
};

/*----------------------------------------------------------------------------
		Structs
----------------------------------------------------------------------------*/

USTRUCT(BlueprintType)
struct FArenaBuff
{
	GENERATED_BODY()

	FArenaBuff() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERarity Rarity = ERarity::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBuffType Type = EBuffType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description = TEXT("No description defined!");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Unlocked = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 UsageCount = 255;

	bool operator==(const FArenaBuff& Other) const
	{
		// We just need to compare the Type.
		return Type == Other.Type;
	}

	bool IsActive() const
	{
		return Unlocked && UsageCount > 0;
	}
};

USTRUCT(BlueprintType)
struct FCardData
{
	GENERATED_BODY()

	FCardData() = default;
	FCardData(const ERarity& InRarity, const ETurretType& InTurret, const FArenaBuff& InBuff)
	{
		Rarity = InRarity;
		TurretType = InTurret;
		Buff = InBuff;
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERarity Rarity = ERarity();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETurretType TurretType = ETurretType();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FArenaBuff Buff = FArenaBuff();
};


/**
 *	Holds the base data for classes including Enums, Structs.
 */
UCLASS()
class ARENA_API UArenaBaseData : public UPrimaryDataAsset
{
	GENERATED_BODY()

};

UCLASS()
class UTurtlePermDamageType : public UDamageType
{
	GENERATED_BODY()
};

UCLASS()
class UTurtleTempDamageType : public UDamageType
{
	GENERATED_BODY()
};

UCLASS()
class UDarknessDamageType : public UDamageType
{
	GENERATED_BODY()
};

UCLASS()
class UExhaustionDamageType : public UDamageType
{
	GENERATED_BODY()
};
