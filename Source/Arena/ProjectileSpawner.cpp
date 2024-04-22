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
	if (GameMode->GetArenaGameState() != EGameStates::Play)
	{
		return;
	}

	FireTimer += DeltaTime;

	if (FireTimer >= CurrentFireInterval)
	{
		FireTimer = 0;
		CurrentFireInterval = GetNewFireInterval();
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
		CurrentFireInterval = GetNewFireInterval();

		// If the Pattern system is active, FireTimer should start as 0
		if (IsPatternSystemActive)
		{
			return;
		}
		// If not, check offset is active, if so, start timer with offset
		else if (IsFireOffsetActive)
		{
			FireTimer = CurrentFireInterval - FireOffset;
		}
		// If not, start the game with fire at t = 0
		else
		{
			FireTimer = CurrentFireInterval;
		}
	}
}

float UProjectileSpawner::GetNewFireInterval()
{
	if (IsPatternSystemActive)
	{
		if (FireDelays.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("The FireDelay array has no element in it!"));
			return FMath::RandRange(MinimumFireDelay, MaximumFireDelay);
		}
		else
		{
			float NewInterval = FireDelays[GetPatternIndex()];

			if (NewInterval < 0.2f)
			{
				return 0.2f;
			}
			else
			{
				return NewInterval;
			}
		}
	}
	else
	{
		return FMath::RandRange(MinimumFireDelay, MaximumFireDelay);
	}
}

int UProjectileSpawner::GetPatternIndex()
{
	// Increase the index by one everytime
	DelayIndex++;

	// If we reached beyond of the array, then reset
	if (DelayIndex >= FireDelays.Num())
	{
		DelayIndex = 0;
	}

	return DelayIndex;
}

