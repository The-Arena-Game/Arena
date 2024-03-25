// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArenaGameMode.h"
#include "ArenaCharacter.h"
#include "DrawDebugHelpers.h"
#include "GlobeBase.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY(LogArnGameMode);

AArenaGameMode::AArenaGameMode()
{
}

void AArenaGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Get all the Yellow Globes that is currently in the level
	UGameplayStatics::GetAllActorsOfClass(this, AGlobeBase::StaticClass(), YellowGlobes);
	GlobeCounter = YellowGlobes.Num();
	ArenaGameState = EGameStates::Ready;

	RestartArenaGame();	// Execute a freash start
}

void AArenaGameMode::RestartArenaGame()
{
	FinishGame(); // Execute end game actions
	CloseCardSelectionUI(); // Close it in case of in a card selection state

	// Get the player start location
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector StartLocation = FindPlayerStart(PlayerController)->GetActorLocation();

	// Reset mouse stuff
	PlayerController->bEnableMouseOverEvents = false;
	PlayerController->bShowMouseCursor = false;

	// if player doesn't exist, this means it is dead after a game, spawn it agan.
	AArenaCharacter* Player = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!IsValid(Player))
	{
		Player = GetWorld()->SpawnActor<AArenaCharacter>(ArenaCharacterClass, StartLocation, FRotator::ZeroRotator);

		if (IsValid(Player))
		{
			// Possess the new player character
			if (PlayerController)
			{
				PlayerController->Possess(Player);
			}
		}

	}
	// Else, relocate the player
	else
	{
		Player->SetActorLocation(StartLocation);
		Player->GetHealthComponent()->ResetHeatlh();
	}

	// Find all globes. Disable yellow ones, destroy the blue one
	TArray<AActor*> GlobeActors;
	UGameplayStatics::GetAllActorsOfClass(this, AGlobeBase::StaticClass(), GlobeActors);
	for (AActor* Actor : GlobeActors)
	{
		AGlobeBase* Globe = Cast<AGlobeBase>(Actor);

		if (Globe->IsBlue())
		{
			Globe->Destroy();
		}
		else
		{
			Globe->SetActorHiddenInGame(true);
			Globe->SetActorEnableCollision(false);
		}
	}

	// Spawn a new blue globe
	BlueGlobe = SpawnBlueGlobe(Player->GetActorLocation(), Player->GetActorRotation());

	ArenaGameState = EGameStates::Ready;
	OnRestart.Broadcast();
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
		FinishGame();
	}
}

void AArenaGameMode::YellowTouch(AGlobeBase* Globe)
{
	if (!IsValid(Globe))
	{
		return;
	}

	// Disable it
	Globe->SetActorHiddenInGame(true);
	Globe->SetActorEnableCollision(false);

	// Decrease the counter by 1 and check if it is zero
	if (--GlobeCounter <= 0)
	{
		// If so, all of the yellow globes are collected, win state
		ArenaGameState = EGameStates::Win;
		FinishGame();
		OpenCardSelectionUI();
		PlayerController->bShowMouseCursor = true;
	}
}

void AArenaGameMode::SetReadyState()
{
	// Spawn blue globe		
	if (AArenaCharacter* Player = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		BlueGlobe = SpawnBlueGlobe(Player->GetActorLocation(), Player->GetActorRotation());
	}

	ArenaGameState = EGameStates::Ready;
	OnGameStateChange.Broadcast(EGameStates::Ready);
	LevelNumber++;
}

void AArenaGameMode::BlueTouch(AGlobeBase* Globe)
{
	if (!IsValid(Globe))
	{
		return;
	}

	Globe->Destroy();

	// Enable all the Yellow Globes
	for (AActor* YellowGlobe : YellowGlobes)
	{
		YellowGlobe->SetActorHiddenInGame(false);
		YellowGlobe->SetActorEnableCollision(true);
	}

	GlobeCounter = YellowGlobes.Num();	// Reset the counter

	ArenaGameState = EGameStates::Play;
	OnGameStateChange.Broadcast(EGameStates::Play);
	StartGame();
}

AGlobeBase* AArenaGameMode::SpawnBlueGlobe(FVector CenterLocation, FRotator SpawnRotation)
{
	// Broadcast to turn ON the forbidden area collisions
	OnBlueSpawnCollisionStateChange.Broadcast(true);

	AGlobeBase* NewGlobe = nullptr;
	FVector SpawnLocation = GetBlueGlobeSpawnLocation(CenterLocation);

	// Spawn blue globe		
	if (BlueGlobeClass != nullptr)
	{
		NewGlobe = GetWorld()->SpawnActor<AGlobeBase>(BlueGlobeClass, SpawnLocation, SpawnRotation);
		NewGlobe->SnapToGround();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BlueGlobeClass is not set on ArenaGameMode! Can not spawn the globe!"));
	}

	// Broadcast to turn OFF the forbidden area collisions
	OnBlueSpawnCollisionStateChange.Broadcast(false);

	return NewGlobe;
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
		}
		while (FVector::Dist(SpawnLocation, CenterLocation) < BlueGlobeMinSpawnDistance);

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

		//// DEBUG
		//DrawDebugSphere(GetWorld(), SpawnLocation, 60.f, 12, FColor::Red, false, 1.f);
		//if (bHit)
		//{
		//	UE_LOG(LogArnGameMode, Log, TEXT("Hit: %s"), *HitResult.GetActor()->GetActorNameOrLabel());
		//}

	}
	while (bHit);

	return SpawnLocation;
}
