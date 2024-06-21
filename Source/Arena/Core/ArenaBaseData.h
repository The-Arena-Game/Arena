// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ArenaBaseData.generated.h"

/*----------------------------
			Enums
----------------------------*/

UENUM(BlueprintType)
enum class ETurretType : uint8
{
	None			UMETA(DisplayName = "None"),
	Single			UMETA(DisplayName = "Single Turret"),
	Dual			UMETA(DisplayName = "Dual Turret"),
	Twin			UMETA(DisplayName = "Twin Turret"),
	Chonky			UMETA(DisplayName = "Chonky Turret"),
	Gatling			UMETA(DisplayName = "Gatling Turret"),
	Triplet			UMETA(DisplayName = "Triplet Turret"),
	Lazer			UMETA(DisplayName = "Lazer Turret"),
	Knight			UMETA(DisplayName = "Knight Turret"),
	Sinus			UMETA(DisplayName = "Sinus Turret"),
	MinusSinus		UMETA(DisplayName = "Minus Sinus Turret"),
	ZigZag			UMETA(DisplayName = "ZigZag Turret"),
	Angry			UMETA(DisplayName = "Angry Turret"),
	BlackHole		UMETA(DisplayName = "Black Hole Turret"),
	LobTurret		UMETA(DisplayName = "Lob Turret"),
	Boomerang		UMETA(DisplayName = "Boomerang Turret"),
	TurtlePerm		UMETA(DisplayName = "Turtle Perm Turret"),
	TurtleTemp		UMETA(DisplayName = "Turtle Temp Turret"),
	Darkness		UMETA(DisplayName = "Darkness Turret"),
	Exhaustion		UMETA(DisplayName = "Exhaustion Turret"),
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
	SquareSinus	UMETA(DisplayName = "SquareSinus Projectile"),
	ZigZag		UMETA(DisplayName = "ZigZag Projectile"),
	Sinus		UMETA(DisplayName = "Sinus Projectile"),
	Laser		UMETA(DisplayName = "Lazer Projectile"),
	BlackHole	UMETA(DisplayName = "Black Hole Projectile"),
	Lob			UMETA(DisplayName = "Lob Projectile"),
	Boomerang	UMETA(DisplayName = "Boomerang Projectile"),
	TurtlePerm	UMETA(DisplayName = "Turtle Perm Projectile"),
	TurtleTemp	UMETA(DisplayName = "Turtle Temp Projectile"),
	Darkness	UMETA(DisplayName = "Darkness Projectile"),
	Exhaustion	UMETA(DisplayName = "Exhaustion Projectile"),
};

/*----------------------------
			Structs
----------------------------*/

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

/*--------------------------
		Damage Types
--------------------------*/

/**
 * Turtle Permanent Damage Type
 *
 * Decreases the speed of the player permanently
 */
UCLASS()
class UTurtlePermDamageType : public UDamageType
{
	GENERATED_BODY()

public:

	/** Must be from 0.0f to 1.0f - Currently set to 5% */
	static constexpr float DecreasePercentage = 0.05f;
};

/**
 * Turtle Temporary Damage Type
 *
 * Decreases the speed of the player for a certain time period
 */
UCLASS()
class UTurtleTempDamageType : public UDamageType
{
	GENERATED_BODY()

public:

	/** Must be from 0.0f to 1.0f - Currently set to 30% */
	static constexpr float DecreasePercentage = 0.3f;

	/** Time for the speed to recover */
	static constexpr float Time = 5.0f;
};

/**
 * Darkness Damage Type
 *
 * Blocks the view of the player for a certain time period
 */
UCLASS()
class UDarknessDamageType : public UDamageType
{
	GENERATED_BODY()

public:

	/** Time for 100% blocking */
	static constexpr float FullBlockTime = 2.0f;

	/** Time for full reveal */
	static constexpr float RevealingTime = 10.0f;
};

/**
 * Exhaustion Damage Type
 *
 * Empty the stamina of the player and prevents it to fill again for a certain time period
 */
UCLASS()
class UExhaustionDamageType : public UDamageType
{
	GENERATED_BODY()

public:

	/** Time for preventing stamina to start filling again */
	static constexpr float Time = 10.0f;
};
