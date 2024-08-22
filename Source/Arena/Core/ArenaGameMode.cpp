// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArenaGameMode.h"
#include "ArenaCharacter.h"
#include "Arena/Components/CardManagementComponent.h"
#include "Arena/Components/HealthComponent.h"
#include "DrawDebugHelpers.h"
#include "GlobeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY(LogArnGameMode);

AArenaGameMode::AArenaGameMode()
{
	CardManagementComp = CreateDefaultSubobject<UCardManagementComponent>("Card Management Component");
	check(CardManagementComp);
}

void AArenaGameMode::BeginPlay()
{
	Super::BeginPlay();

	// CardManagementComp = Cast<UCardManagementComponent>(GetComponentByClass(UCardManagementComponent::StaticClass()));
	// if (!IsValid(CardManagementComp))
	// {
	// 	CardManagementComp
	// }

	// Get all the Yellow Globes that is currently in the level
	UGameplayStatics::GetAllActorsOfClass(this, AGlobeBase::StaticClass(), YellowGlobes);
	GlobeCounter = YellowGlobes.Num();
}

void AArenaGameMode::Cheat_CollectAllYellows()
{
	if (ArenaGameState != EGameStates::Play)
	{
		return;
	}

	for (AActor* YellowGlobe : YellowGlobes)
	{
		YellowGlobe->SetActorHiddenInGame(true);
		YellowGlobe->SetActorEnableCollision(false);
	}

	GlobeCounter = 0;

	CardManagementComp->GenerateCardData(LevelNumber);

	SetGameState(EGameStates::Win);
	FinishGame();
	OpenCardSelectionUI();
	PlayerController->bShowMouseCursor = true;
}

void AArenaGameMode::Cheat_CollectBlue()
{
	if (ArenaGameState != EGameStates::Ready)
	{
		return;
	}

	AGlobeBase* Blue = nullptr;
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AGlobeBase::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		if (AGlobeBase* Globe = Cast<AGlobeBase>(Actor))
		{
			if (Globe->IsBlue())
			{
				BlueTouch(Globe);
			}
		}
	}
}

void AArenaGameMode::RestartArenaGame()
{
	if (ArenaGameState != EGameStates::Menu)
	{
		FinishGame(); // Execute end game actions if this isn't called from menu
	}
	CloseCardSelectionUI(); // Close it in case of in a card selection state

	// Get the player start location
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->SetInputMode(FInputModeGameAndUI());	// Make the default input mode work with both UI and game, default is GameOnly!
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
		if (IsValid(Player) && IsValid(Player->GetHealthComponent()))
		{
			Player->SetActorLocation(StartLocation);
			Player->GetHealthComponent()->ResetHeatlh();
		}
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
	LevelNumber = 1;

	SetGameState(EGameStates::Ready);
	CardManagementComp->OnRestart();
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
		SetGameState(EGameStates::Lost);

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

	// If this was the first yellow, then enable all yellows
	if (!AllYellowsEnabled)
	{
		for (AActor* YellowGlobe : YellowGlobes)
		{
			YellowGlobe->SetActorHiddenInGame(false);
			YellowGlobe->SetActorEnableCollision(true);
		}

		AllYellowsEnabled = true;
	}

	// Disable it this one
	Globe->SetActorHiddenInGame(true);
	Globe->SetActorEnableCollision(false);

	// Decrease the counter by 1 and check if it is zero
	if (--GlobeCounter <= 0)
	{
		CardManagementComp->GenerateCardData(LevelNumber);

		// If so, all of the yellow globes are collected, win state
		SetGameState(EGameStates::Win);
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

	SetGameState(EGameStates::Ready);
	LevelNumber++;
}

void AArenaGameMode::BlueTouch(AGlobeBase* Globe)
{
	if (!IsValid(Globe))
	{
		return;
	}

	// Find the farthest yellow
	AActor* FarthestYellow = YellowGlobes[0];
	for (AActor* YellowGlobe : YellowGlobes)
	{
		double Distance = FVector::Dist(YellowGlobe->GetActorLocation(), Globe->GetActorLocation());
		double FarthestDistance = FVector::Dist(FarthestYellow->GetActorLocation(), Globe->GetActorLocation());

		if (Distance > FarthestDistance)
		{
			FarthestYellow = YellowGlobe;
		}
	}

	// Enable the closest one and destroy the blue
	FarthestYellow->SetActorHiddenInGame(false);
	FarthestYellow->SetActorEnableCollision(true);
	Globe->Destroy();

	// Mark as not all yellows enabled
	AllYellowsEnabled = false;

	GlobeCounter = YellowGlobes.Num();	// Reset the counter

	SetGameState(EGameStates::Play);
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
		UE_LOG(LogArnGameMode, Error, TEXT("BlueGlobeClass is not set on ArenaGameMode! Can not spawn the globe!"));
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