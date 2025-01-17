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
	Homing			UMETA(DisplayName = "Homing Turret"),
	YoYo			UMETA(DisplayName = "Yo-Yo! Turret"),
};

UENUM(BlueprintType)
enum class EBuffType : uint8
{
	None				UMETA(DisplayName = "None"),
	TestBuff_9			UMETA(DisplayName = "Test Buff Name 9"),
	TestBuff_10			UMETA(DisplayName = "Test Buff Name 10"),
	TestBuff_11			UMETA(DisplayName = "Test Buff Name 11"),
	UnlockFlash			UMETA(DisplayName = "Unlock Flash"),
	DecFlashCD_Common	UMETA(DisplayName = "Decrease Flash CD Common"),
	DecFlashCD_Rare		UMETA(DisplayName = "Decrease Flash CD Rare"),
	DecFlashCD_Epic		UMETA(DisplayName = "Decrease Flash CD Epic"),
	FlashExplosion		UMETA(DisplayName = "Activates Explosion On Flash"),
	UnlockDash			UMETA(DisplayName = "Unlock Dash"),
	DecDashCD_Common	UMETA(DisplayName = "Decrease Dash CD Common"),
	DecDashCD_Rare		UMETA(DisplayName = "Decrease Dash CD Rare"),
	DecDashCD_Epic		UMETA(DisplayName = "Decrease Dash CD Epic"),
	DeflectCharge_Rare	UMETA(DisplayName = "Deflect Charge Rare"),
	DeflectCharge_Epic	UMETA(DisplayName = "Deflect Charge Epic"),
	DecDeflectCD_Common	UMETA(DisplayName = "Decrease Deflect CD Common"),
	DecDeflectCD_Rare	UMETA(DisplayName = "Decrease Deflect CD Rare"),
	DecDeflectCD_Epic	UMETA(DisplayName = "Decrease Deflect CD Epic"),
	SprintDeflect		UMETA(DisplayName = "Activate Sprint Deflect"),
	FlashDeflect		UMETA(DisplayName = "Activate Flash Deflect"),
	AutoDeflect			UMETA(DisplayName = "Activate Auto Deflect"),
	DeflectExplosion	UMETA(DisplayName = "Activates Explosion On Deflect"),
	MaxStamina_Common	UMETA(DisplayName = "Max Stamina Common"),
	MaxStamina_Rare		UMETA(DisplayName = "Max Stamina Rare"),
	MaxStamina_Epic		UMETA(DisplayName = "Max Stamina Epic"),
	StaminaRegen_Common	UMETA(DisplayName = "Stamina Regen Common"),
	StaminaRegen_Rare	UMETA(DisplayName = "Stamina Regen Rare"),
	StaminaRegen_Epic	UMETA(DisplayName = "Stamina Regen Epic"),
	StaminaRegenDelay_Common	UMETA(DisplayName = "Stamina Regen Delay Common"),
	StaminaRegenDelay_Rare		UMETA(DisplayName = "Stamina Regen Delay Rare"),
	StaminaRegenDelay_Epic		UMETA(DisplayName = "Stamina Regen Delay Epic"),
	WalkSpeed_Common	UMETA(DisplayName = "Walk Speed Common"),
	WalkSpeed_Rare		UMETA(DisplayName = "Walk Speed Rare"),
	WalkSpeed_Epic		UMETA(DisplayName = "Walk Speed Epic"),
	SprintSpeed_Common	UMETA(DisplayName = "Sprint Speed Common"),
	SprintSpeed_Rare	UMETA(DisplayName = "Sprint Speed Rare"),
	SprintSpeed_Epic	UMETA(DisplayName = "Sprint Speed Epic"),
	Healing_Common		UMETA(DisplayName = "Healing Common"),
	Healing_Rare		UMETA(DisplayName = "Healing Rare"),
	MaxHealth_Rare		UMETA(DisplayName = "Max Health Rare"),
	MaxHealth_Epic		UMETA(DisplayName = "Max Health Epic"),
	HealthRegen_Rare	UMETA(DisplayName = "Health Regen Rare"),
	HealthRegen_Epic	UMETA(DisplayName = "Health Regen Epic"),
	HealthRegenUpgrade  UMETA(DisplayName = "Health Regen Upgrade"),
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
	Homing		UMETA(DisplayName = "Homing Projectile"),
	YoYo	    UMETA(DisplayName = "Yo-Yo! Projectile"),
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
	bool bUnlocked = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 UsageCount = 255;

	bool operator==(const FArenaBuff& Other) const
	{
		// We just need to compare the Type.
		return Type == Other.Type;
	}

	bool IsActive() const
	{
		return bUnlocked && UsageCount > 0;
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

public:

	/*--------------------------
		Buff Variables
	--------------------------*/

	// Flash

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	float DecFlashCD_Common = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	float DecFlashCD_Rare = 7.5f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	float DecFlashCD_Epic = 10.0f;


	// Dash

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	float DecDashCD_Common = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	float DecDashCD_Rare = 7.5f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	float DecDashCD_Epic = 10.0f;


	// Deflect

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	int8 DeflectCharge_Rare = 1;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	int8 DeflectCharge_Epic = 2;

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	float DecDeflectCD_Common = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	float DecDeflectCD_Rare = 7.5f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	float DecDeflectCD_Epic = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Special Movements")
	float SprintDeflectCooldown = 30.0f;


	// Max Stamina

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Stamina")
	float MaxStamina_Common = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Stamina")
	float MaxStamina_Rare = 7.5f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Stamina")
	float MaxStamina_Epic = 10.0f;


	// Stamina Regen

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Stamina")
	float StaminaRegen_Common = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Stamina")
	float StaminaRegen_Rare = 7.5f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Stamina")
	float StaminaRegen_Epic = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Stamina")
	float StaminaRegenDelay = 3.0f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Stamina")
	float StaminaRegenDelay_Common = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Stamina")
	float StaminaRegenDelay_Rare = 7.5f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Stamina")
	float StaminaRegenDelay_Epic = 10.0f;


	// Sprint Sped

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Basic Movements")
	float SprintSpeed_Common = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Basic Movements")
	float SprintSpeed_Rare = 7.5f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Basic Movements")
	float SprintSpeed_Epic = 10.0f;


	// Walk Sped

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Basic Movements")
	float WalkSpeed_Common = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Basic Movements")
	float WalkSpeed_Rare = 7.5f;
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Basic Movements")
	float WalkSpeed_Epic = 10.0f;

	// Health Regen

	// Amount of Health to Regenerate in the selected round amount. 1 Health means a half heart!
	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Health", meta = (UIMin = 1, UIMax = 4, ClampMin = 1, ClampMax = 4, Tooltip = "Amount of Health to Regenerate in the selected round amount. 1 Health means a half heart!"))
	uint8 HealthRegenAmount = 1;

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Health", meta = (UIMin = 1, UIMax = 4, ClampMin = 1, ClampMax = 4))
	uint8 HealthRegenRound_Rare = 4;

	UPROPERTY(EditAnywhere, Category = "Arena - Buff - Health", meta = (UIMin = 1, UIMax = 4, ClampMin = 1, ClampMax = 4))
	uint8 HealthRegenRound_Epic = 2;
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
	static constexpr float FullBlockTime = 1.0f;

	/** Time for full reveal */
	static constexpr float RevealingTime = 5.0f;
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
