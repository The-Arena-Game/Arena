// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArenaGameMode.h"
#include "ArenaCharacter.h"
#include "GlobeBase.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogArnGameMode);

AArenaGameMode::AArenaGameMode() {}

void AArenaGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Get all the Yellow Globes that is currently in the level
	UGameplayStatics::GetAllActorsOfClass(this, AGlobeBase::StaticClass(), YellowGlobes);
	ArenaGameState = EGameStates::Play;

	// Execute StartGame BP Event
	StartGame();
}

void AArenaGameMode::HandlePlayerDeath()
{
	// Get the player from the GameplayStatics
	AArenaCharacter* Player = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!IsValid(Player))
	{
		return;
	}

	// If player is still alive, don't execute game over state
	if (Player->GetHealthComponent()->IsDead())
	{
		ArenaGameState = EGameStates::Lost;

		// Execute FinishGame BP Event
		FinishGame(false);
	}
}

void AArenaGameMode::YellowTouch(AGlobeBase* Globe)
{
	// If the globe that calls this function is one of our globes, then destroy it
	if (YellowGlobes.Contains<AGlobeBase*>(Globe))
	{
		YellowGlobes.Remove(Globe);
		Globe->Destroy();
	}

	// If all the globes are collected by the player, then it is a win state
	if (YellowGlobes.Num() == 0)
	{
		ArenaGameState = EGameStates::Win;

		AArenaCharacter* Player = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

		// Spawn blue globe		
		if (IsValid(Player) && BlueGlobeClass != nullptr)
		{
			FVector SpawnLocation = GetBlueGlobeSpawnLocation(Player->GetActorLocation());
			FRotator SpawnRotation = Player->GetActorRotation();

			AGlobeBase* Globe = GetWorld()->SpawnActor<AGlobeBase>(BlueGlobeClass, SpawnLocation, SpawnRotation);
			Globe->SnapToGround();
		}
	}
}

FVector AArenaGameMode::GetBlueGlobeSpawnLocation(FVector CenterLocation)
{
	FVector SpawnLocation;
	bool bHit = false;

	// Get a random spawn location that is not colliding with anything
	do
	{
		// If it is too close, get a new location
		do
		{
			float XDistance = FMath::FRandRange(-BlueGlobeMaxSpawnDistance, BlueGlobeMaxSpawnDistance);
			float YDistance = FMath::FRandRange(-BlueGlobeMaxSpawnDistance, BlueGlobeMaxSpawnDistance);
			SpawnLocation = CenterLocation + FVector(XDistance, YDistance, 0.f);
		} while (FVector::Dist(SpawnLocation, CenterLocation) < BlueGlobeMinSpawnDistance);

		FHitResult HitResult;
		ECollisionChannel CollisionChannel = ECC_Visibility;
		FCollisionQueryParams CollisionParams;

		// Perform the collision check
		bHit = GetWorld()->SweepSingleByChannel(
			HitResult,
			SpawnLocation,
			SpawnLocation,
			FQuat::Identity,
			CollisionChannel,
			FCollisionShape::MakeSphere(GlobeSphereRadius),
			CollisionParams
		);
	} while (bHit);

	return SpawnLocation;
}

void AArenaGameMode::BlueTouch(AGlobeBase* Globe)
{
	Globe->Destroy();
	ArenaGameState = EGameStates::Ready;
}
