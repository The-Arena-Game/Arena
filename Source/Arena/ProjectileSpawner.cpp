#include "ProjectileSpawner.h"
#include "ArenaGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectileBase.h"

// Sets default values for this component's properties
UProjectileSpawner::UProjectileSpawner()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UProjectileSpawner::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<AArenaGameMode>(UGameplayStatics::GetGameMode(this));
	GameMode->OnGameStateChange.AddDynamic(this, &UProjectileSpawner::OnNewGameModeState);
}

// Called every frame
void UProjectileSpawner::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Don't execute any action further if the game state is not Play state
	if (GameMode->GetGameState() != EGameStates::Play)
	{
		return;
	}

	// If the interval not set yet, set it.
	if (CurrentFireInterval == 0)
	{
		CurrentFireInterval = FMath::RandRange(MinimumFireDelay, MaximumFireDelay);
	}

	FireTimer += DeltaTime;

	if (FireTimer >= CurrentFireInterval)
	{
		FireTimer = 0;
		CurrentFireInterval = FMath::RandRange(MinimumFireDelay, MaximumFireDelay);
		Fire();
	}
}



void UProjectileSpawner::Fire()
{
	if (ProjectileClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("The Spawn Point (%s) is not set on the turret (%s)!"), *GetName(), *GetOwner()->GetActorNameOrLabel());
		return;
	}

	// Spawn projectile and hold its instance in Projectile Variable
	AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(
		ProjectileClass, GetComponentLocation(), GetComponentRotation());

	// Attaching the projectile to the owner, so we can access it in case of a hit
	Projectile->SetOwner(GetOwner());
}

void UProjectileSpawner::OnNewGameModeState(EGameStates NewState)
{
	if (NewState == EGameStates::Play)
	{
		CurrentFireInterval = FMath::RandRange(MinimumFireDelay, MaximumFireDelay);

		// Set the timer so Tick would fire accourdingly.
		if (bIsFireOffsetActive)
		{
			FireTimer = CurrentFireInterval - FireOffset;
		}
		else
		{
			FireTimer = CurrentFireInterval;
		}
	}
}

