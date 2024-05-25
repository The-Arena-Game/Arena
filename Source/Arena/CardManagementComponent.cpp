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
}

void UCardManagementComponent::GetCardData(const int& Level, TArray<FCardData>& CardData)
{
	TArray<FCardData> NewCards = TArray<FCardData>();

	// Define rarity, get turrets and buffs based on that rarity for each 3 cards!
	for (int i = 0; i < 3; i++)
	{
		//UE_LOG(LogArnCardManagement, Warning, TEXT("Creating new one, current card number: %i"), NewCards.Num());
		NewCards.Add(FCardData());	// Add a new card

		// TODO: Level number is irrelevant right now. I'll add rarity algorithm later! Current we are choosing only from the common pool
		// Define the rarity, rarity can be same for all cards, doesn't matter!
		ERarity NewRarity = ERarity::Common;
		NewCards[i].Rarity = NewRarity;

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
			int Index = FMath::RandRange(0, SelectedPools->TurretPool.Num() - 1);
			NewTurret = SelectedPools->TurretPool[Index];

			for (FCardData Card : NewCards)
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
			int Index = FMath::RandRange(0, SelectedPools->BuffPool.Num() - 1);
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

			// If not found in others, get this one
			if (!ElementFound)
			{
				//UE_LOG(LogArnCardManagement, Warning, TEXT("Added new buff: %s"), *UEnum::GetValueAsString(NewBuff.Type));
				NewCards[i].Buff = NewBuff;
			}
		}
		while (ElementFound);	// If the selected turret is already in others, then pick another!
	}

	//UE_LOG(LogArnCardManagement, Warning, TEXT("Finito. Number of cards: %i"), NewCards.Num());
	CardData = NewCards;
}
