// Fill out your copyright notice in the Description page of Project Settings.

#include "CardManagementComponent.h"
#include "Arena/Core/ArenaGameMode.h"
#include "Arena/Core/ArenaCharacter.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogArnCardManagement);

void UCardManagementComponent::BeginPlay()
{
	Super::BeginPlay();

	// Start 
	OnRestart();

	OnBuffSelected.AddDynamic(this, &UCardManagementComponent::BuffSelected);
}

void UCardManagementComponent::OnRestart()
{
	if (const AArenaGameMode* GameMode = Cast<AArenaGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		BaseData = GameMode->GetBaseData();
	}

	if (AArenaCharacter* ArenaCharacter = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Character = ArenaCharacter;
	}

	Pools.Empty();

	Pools.Add(ERarity::Common, { ERarity::Common, CommonTurretPool, CommonBuffPool });
	Pools.Add(ERarity::Rare, { ERarity::Rare, RareTurretPool, RareBuffPool });
	Pools.Add(ERarity::Epic, { ERarity::Epic, EpicTurretPool, EpicBuffPool });
	Pools.Add(ERarity::Legendary, { ERarity::Legendary, LegendaryTurretPool, LegendaryBuffPool });
}

ERarity UCardManagementComponent::GetRarity(const uint8& Level) const
{
	ERarity SelectedRarity = ERarity::None;

	const float CommonWeight = GetWeight(Level, CommonBaseValue, CommonGrowthRate);
	const float& RareWeight = GetWeight(Level, RareBaseValue, RareGrowthRate);
	const float& EpicWeight = GetWeight(Level, EpicBaseValue, EpicGrowthRate);
	const float& LegendaryWeight = GetWeight(Level, LegendaryBaseValue, LegendaryGrowthRate);

	const float TotalWeight = CommonWeight + RareWeight + EpicWeight + LegendaryWeight;

	const float CommonValue = (CommonWeight / TotalWeight);
	const float RareValue = CommonValue + (RareWeight / TotalWeight);
	const float EpicValue = RareValue + (EpicWeight / TotalWeight);

	do
	{
		const float Random = FMath::RandRange(0.f, 1.f);

		// UE_LOG(LogArnCardManagement, Log, TEXT("Level: %i - Random: %f | Common: %f, Rare: %f, Epic: %f, Legendary: %f"),
		// 	Level, Random * 100, (CommonWeight / TotalWeight) * 100, (RareWeight / TotalWeight) * 100, (EpicWeight / TotalWeight) * 100, (LegendaryWeight / TotalWeight) * 100
		// );

		if (Random <= CommonValue)
		{
			SelectedRarity = ERarity::Common;
		}
		else if (Random <= RareValue)
		{
			SelectedRarity = ERarity::Rare;
		}
		else if (Random <= EpicValue)
		{
			SelectedRarity = ERarity::Epic;
		}
		else
		{
			SelectedRarity = ERarity::Legendary;
		}
	}
	while ((!RareUnlocked && SelectedRarity == ERarity::Rare) || (!EpicUnlocked && SelectedRarity == ERarity::Epic) || (!LegendaryUnlocked && SelectedRarity == ERarity::Legendary));

	return SelectedRarity;
}

void UCardManagementComponent::CheckLevelUnlocks(const uint8& Level)
{
	// This is an example scope here for level based unlocks. But we use Rarity for the buffs, so I don't think we need the level-based unlock but good to keep this here.
	if (Level == 5)
	{
		// Check common buffs
		if (FRarityPool* RarityPools = Pools.Find(ERarity::Common))
		{
			for (FArenaBuff& Buff : RarityPools->BuffPool)
			{
				// Unlock Deflect and bonuses
				if (Buff.Type == EBuffType::TestBuff_10)
				{
					UE_LOG(LogArnCardManagement, Log, TEXT("Buff 10 unlocked"));
					Buff.Unlocked = true;
				}
			}
		}
	}

	// Rarity Unlocks - Unlocks the rarities based on level.
	if (Level == RareUnlockLevel)
	{
		RareUnlocked = true;
	}
	else if (Level == EpicUnlockLevel)
	{
		EpicUnlocked = true;
	}
	else if (Level == LegendaryUnlockLevel)
	{
		LegendaryUnlocked = true;
	}
}

void UCardManagementComponent::GenerateCardData(const uint8& Level)
{
	CheckLevelUnlocks(Level);

	CardsData.Empty();
	TArray<FCardData> NewCards = TArray<FCardData>();

	// Define rarity, get turrets and buffs based on that rarity for each 3 cards!
	for (int i = 0; i < 3; i++)
	{
		//UE_LOG(LogArnCardManagement, Warning, TEXT("Creating new one, current card number: %i"), NewCards.Num());
		NewCards.Add(FCardData());	// Add a new card

		// Define the rarity, rarity can be same for all cards, doesn't matter!
		ERarity NewRarity = GetRarity(Level);
		NewCards[i].Rarity = NewRarity;
		// UE_LOG(LogArnCardManagement, Warning, TEXT("Selected Rarity: %s"), *UEnum::GetValueAsString(NewRarity));

		const FRarityPool* SelectedPools = Pools.Find(NewRarity);
		if (SelectedPools == nullptr)
		{
			//UE_LOG(LogArnCardManagement, Error, TEXT("Pools not found! Rarity: %s"), *UEnum::GetValueAsString(NewRarity));
			return;
		}

		if (SelectedPools->TurretPool.Num() < 3 || SelectedPools->BuffPool.Num() < 3)
		{
			//UE_LOG(LogArnCardManagement, Error, TEXT("Insufficient number of elemts in Turret or Buff Pool! Rarity: %s"), *UEnum::GetValueAsString(NewRarity));
			return;
		}

		//UE_LOG(LogArnCardManagement, Log, TEXT("Starting for Turret"));

		// Get turrets
		ETurretType NewTurret = ETurretType::None;
		bool ElementFound;
		do
		{
			ElementFound = false;
			//UE_LOG(LogArnCardManagement, Log, TEXT("Getting new Turret"));
			const int Index = FMath::RandRange(0, SelectedPools->TurretPool.Num() - 1);
			NewTurret = SelectedPools->TurretPool[Index];

			for (const FCardData Card : NewCards)
			{
				if (Card.TurretType == NewTurret)
				{
					//UE_LOG(LogArnCardManagement, Error, TEXT("ALREADY EXIST!! %s"), *UEnum::GetValueAsString(NewTurret));
					ElementFound = true;
					break;
				}
			}

			// If not found in others, get this one
			if (!ElementFound)
			{
				// UE_LOG(LogArnCardManagement, Warning, TEXT("Added new turret: %s"), *UEnum::GetValueAsString(NewTurret));
				NewCards[i].TurretType = NewTurret;
			}
		}
		while (ElementFound);	// If the selected turret is already in others, then pick another!

		//UE_LOG(LogArnCardManagement, Log, TEXT("Getting new Buff"));
		// Get Buffs
		FArenaBuff NewBuff = FArenaBuff();
		do
		{
			ElementFound = false;
			const int Index = FMath::RandRange(0, SelectedPools->BuffPool.Num() - 1);
			NewBuff = SelectedPools->BuffPool[Index];

			for (FCardData Card : NewCards)
			{
				if (Card.Buff == NewBuff)
				{
					ElementFound = true;
					//UE_LOG(LogArnCardManagement, Error, TEXT("ALREADY EXIST!! %s"), *UEnum::GetValueAsString(NewBuff.Type));
					break;
				}
			}

			FString UnlockStr = NewBuff.Unlocked ? TEXT("True") : TEXT("False");
			//UE_LOG(LogArnCardManagement, Error, TEXT("Selected Buff: %s - Unlocked?: %s - Usage: %i"), *UEnum::GetValueAsString(NewBuff.Type), *UnlockStr, NewBuff.UsageCount);

			// If not found in others, get this one
			if (!ElementFound)
			{
				//UE_LOG(LogArnCardManagement, Warning, TEXT("Added new buff: %s"), *UEnum::GetValueAsString(NewBuff.Type));
				NewCards[i].Buff = NewBuff;
			}
		}
		while (ElementFound || !NewBuff.IsActive());	// If the selected turret is already in others OR it is not active, then pick another!
	}

	//UE_LOG(LogArnCardManagement, Warning, TEXT("Finito. Number of cards: %i"), NewCards.Num());
	CardsData = NewCards;
}

void UCardManagementComponent::BuffSelected(const FArenaBuff& InBuff)
{
	// Find the buff and decrease the counter
	if (FRarityPool* RarityPool = Pools.Find(InBuff.Rarity))
	{
		for (FArenaBuff& Buff : RarityPool->BuffPool)
		{
			if (InBuff == Buff)
			{
				Buff.UsageCount--;
			}
		}
	}

	CheckFlashBuff(InBuff);
	CheckDashBuff(InBuff);
	CheckDeflectBuff(InBuff);

	if (InBuff.Type == EBuffType::TestBuff_10)
	{
		if (FRarityPool* RarityPool = Pools.Find(InBuff.Rarity))
		{
			for (FArenaBuff& Buff : RarityPool->BuffPool)
			{
				if (Buff.Type == EBuffType::TestBuff_11)
				{
					Buff.Unlocked = true;
				}
			}
		}
	}
}

void UCardManagementComponent::CheckFlashBuff(const FArenaBuff& InBuff)
{
	if (!IsValid(Character))
	{
		UE_LOG(LogArnCardManagement, Error, TEXT("Character is not valid!!"));
		return;
	}

	switch (InBuff.Type)
	{
	case EBuffType::UnlockFlash:	// Unlock all the flash buffs

		// Check all the pools
		for (TTuple<ERarity, FRarityPool>& Pool : Pools)
		{
			// All the buffs in a pool
			for (FArenaBuff& Buff : Pool.Value.BuffPool)
			{
				if (Buff.Type == EBuffType::DecFlashCD_Common || Buff.Type == EBuffType::DecFlashCD_Rare || Buff.Type == EBuffType::DecFlashCD_Epic)
				{
					Buff.Unlocked = true;
					Character->ActivateFlash();
					UE_LOG(LogArnCardManagement, Log, TEXT("%s Buff Unlocked!"), *UEnum::GetValueAsString(Buff.Type));
				}
			}
		}
		break;

		// TODO: Replace the hardcoded values with BaseData->DecFlashCD_Common - But set the DA on the Game Mode first!
	case EBuffType::DecFlashCD_Common:
		Character->DecreaseFlashCooldownDuration(5.f);
		UE_LOG(LogArnCardManagement, Log, TEXT("Flash Cooldown Common Used!"));
		break;
	case EBuffType::DecFlashCD_Rare:
		Character->DecreaseFlashCooldownDuration(7.5f);
		UE_LOG(LogArnCardManagement, Log, TEXT("Flash Cooldown Rare Used!"));
		break;
	case EBuffType::DecFlashCD_Epic:
		Character->DecreaseFlashCooldownDuration(10.f);
		UE_LOG(LogArnCardManagement, Log, TEXT("Flash Cooldown Epic Used!"));
		break;

	default:
		break;
	}
}

void UCardManagementComponent::CheckDashBuff(const FArenaBuff& InBuff)
{
	if (!IsValid(Character))
	{
		UE_LOG(LogArnCardManagement, Error, TEXT("Character is not valid!!"));
		return;
	}

	switch (InBuff.Type)
	{
	case EBuffType::UnlockDash:	// Unlock all the Dash buffs

		// Check all the pools
		for (TTuple<ERarity, FRarityPool>& Pool : Pools)
		{
			// All the buffs in a pool
			for (FArenaBuff& Buff : Pool.Value.BuffPool)
			{
				if (Buff.Type == EBuffType::DecDashCD_Common || Buff.Type == EBuffType::DecDashCD_Rare || Buff.Type == EBuffType::DecDashCD_Epic)
				{
					Buff.Unlocked = true;
					Character->ActivateDash();
					UE_LOG(LogArnCardManagement, Log, TEXT("%s Buff Unlocked!"), *UEnum::GetValueAsString(Buff.Type));
				}
			}
		}
		break;

		// TODO: Replace the hardcoded values with BaseData->DecDashCD_Common - But set the DA on the Game Mode first!
	case EBuffType::DecDashCD_Common:
		Character->DecreaseDashCooldownDuration(5.f);
		UE_LOG(LogArnCardManagement, Log, TEXT("Dash Cooldown Common Used!"));
		break;
	case EBuffType::DecDashCD_Rare:
		Character->DecreaseDashCooldownDuration(7.5f);
		UE_LOG(LogArnCardManagement, Log, TEXT("Dash Cooldown Rare Used!"));
		break;
	case EBuffType::DecDashCD_Epic:
		Character->DecreaseDashCooldownDuration(10.f);
		UE_LOG(LogArnCardManagement, Log, TEXT("Dash Cooldown Epic Used!"));
		break;

	default:
		break;
	}
}

void UCardManagementComponent::CheckDeflectBuff(const FArenaBuff& InBuff)
{
	if (!IsValid(Character))
	{
		UE_LOG(LogArnCardManagement, Error, TEXT("Character is not valid!!"));
		return;
	}

	switch (InBuff.Type)
	{
		// TODO: Replace the hardcoded values with BaseData->DecDashCD_Common - But set the DA on the Game Mode first!
	case EBuffType::DeflectCharge_Rare:
		Character->IncreaseDeflectCharge(1);
		UE_LOG(LogArnCardManagement, Log, TEXT("Deflect Charge Rare Used!"));
		break;
	case EBuffType::DeflectCharge_Epic:
		Character->IncreaseDeflectCharge(2);
		UE_LOG(LogArnCardManagement, Log, TEXT("Deflect Charge Rare Used!"));
		break;
	case EBuffType::DecDeflectCD_Common:
		Character->DecreaseDashCooldownDuration(5.0f);
		UE_LOG(LogArnCardManagement, Log, TEXT("Deflect Cooldown Common Used!"));
		break;
	case EBuffType::DecDeflectCD_Rare:
		Character->DecreaseDashCooldownDuration(7.5f);
		UE_LOG(LogArnCardManagement, Log, TEXT("Deflect Cooldown Rare Used!"));
		break;
	case EBuffType::DecDeflectCD_Epic:
		Character->DecreaseDashCooldownDuration(10.f);
		UE_LOG(LogArnCardManagement, Log, TEXT("Deflect Cooldown Epic Used!"));
		break;

	default:
		break;
	}
}

