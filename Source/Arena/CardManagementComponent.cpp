// Fill out your copyright notice in the Description page of Project Settings.

#include "CardManagementComponent.h"

DEFINE_LOG_CATEGORY(LogArnCardManagement);

void UCardManagementComponent::BeginPlay()
{
	Super::BeginPlay();

	Pools.Add(ERarity::Common, { ERarity::Common, CommonTurretPool, CommonBuffPool });
	Pools.Add(ERarity::Rare, { ERarity::Rare, RareTurretPool, RareBuffPool });
	Pools.Add(ERarity::Epic, { ERarity::Epic, EpicTurretPool, EpicBuffPool });
	Pools.Add(ERarity::Legendary, { ERarity::Legendary, LegendaryTurretPool, LegendaryBuffPool });

	OnBuffSelected.AddDynamic(this, &UCardManagementComponent::BuffSelected);
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

void UCardManagementComponent::BuffSelected(const FArenaBuff& InBuff)
{
	// Find the buff and decrease the counter
	if (FRarityPools* RarityPools = Pools.Find(InBuff.Rarity))
	{
		for (FArenaBuff& Buff : RarityPools->BuffPool)
		{
			if (InBuff == Buff)
			{
				Buff.UsageCount--;
			}
		}
	}

	if (InBuff.Type == EBuffType::TestBuff_10)
	{
		if (FRarityPools* RarityPools = Pools.Find(InBuff.Rarity))
		{
			for (FArenaBuff& Buff : RarityPools->BuffPool)
			{
				if (Buff.Type == EBuffType::TestBuff_11)
				{
					Buff.Unlocked = true;
				}
			}
		}
	}
}

void UCardManagementComponent::CheckLevelUnlocks(const uint8& Level)
{
	if (Level == 5)
	{
		// Check common buffs
		if (FRarityPools* RarityPools = Pools.Find(ERarity::Common))
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

	// Rarity Unlocks
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

		const FRarityPools* SelectedPools = Pools.Find(NewRarity);
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
