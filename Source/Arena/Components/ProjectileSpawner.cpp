#include "ProjectileSpawner.h"
#include "Arena/Core/ArenaGameMode.h"
#include "Arena/Projectiles/ProjectileBase.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogArnProjectileSpawner);

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

	FireTimer = 0;
	CurrentFireInterval = GetNewFireInterval();

	// Check if the VFX timing is good or not
	if (IsLaserType)
	{
		float Sum = 0.f;
		for (const float Value : LaserPatternValues)
		{
			if (Value < 0.01)
			{
				UE_LOG(LogArnProjectileSpawner, Error, TEXT("Laser VFX pattern value can not be less than 0.01!"));
				IsLaserType = false;
				break;
			}
			Sum += Value;
		}

		if (Sum >= MinimumFireDelay)
		{
			UE_LOG(LogArnProjectileSpawner, Error, TEXT("Minimum fire time is lower than the VFX timer! VFX disabled!"));
			IsLaserType = false;
		}
		else
		{
			// copy the array so we can make sure they are same size
			VfxToggleTimes = LaserPatternValues;
			SetVfxToggleTimes();
		}
	}
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

	// Laser Low Budget VFX
	if (IsLaserType)
	{
		HandleVfx();
	}

	if (FireTimer >= CurrentFireInterval)
	{
		FireTimer = 0;
		CurrentFireInterval = GetNewFireInterval();
		Fire();

		if (IsLaserType)
		{
			SetVfxToggleTimes();
		}
	}
}

void UProjectileSpawner::Fire()
{
	if (ProjectileClass == nullptr)
	{
		UE_LOG(LogArnProjectileSpawner, Error, TEXT("The Spawn Point (%s) is not set on the turret (%s)!"), *GetName(), *GetOwner()->GetActorNameOrLabel());
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
			UE_LOG(LogArnProjectileSpawner, Warning, TEXT("The FireDelay array has no element in it!"));
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

void UProjectileSpawner::SetVfxToggleTimes()
{
	//UE_LOG(LogArnProjectileSpawner, Warning, TEXT("Next Fire Time: %f"), CurrentFireInterval);

	VfxToggleTimes[0] = CurrentFireInterval - LaserPatternValues[0];
	for (int i = 1; i < VfxToggleTimes.Num(); i++)
	{
		VfxToggleTimes[i] = VfxToggleTimes[i - 1] - LaserPatternValues[i]; // subtract from 1 above
		//UE_LOG(LogArnProjectileSpawner, Log, TEXT("Vfx Toggle Time: %f"), VfxToggleTimes[i]);
	}
}

void UProjectileSpawner::HandleVfx()
{
	// Check all array variables if the timer reach that point, start from the back
	for (int i = VfxToggleTimes.Num() - 1; i > 0; i--)
	{
		if (FireTimer >= VfxToggleTimes[i])
		{
			// Calculate the lifetime of the line by getting the next toggle point
			const float LineTime = VfxToggleTimes[i - 1] - VfxToggleTimes[i];
			// UE_LOG(LogArnProjectileSpawner, Warning, TEXT("Turning on VFX! Fire Timer: %f - Line Lifespan: %f"), FireTimer, LineTime);


			const FVector StartLocation = GetComponentLocation() - GetForwardVector() * 1200;
			const FVector TargetLocation = GetComponentLocation() + GetForwardVector() * 1200;
			DrawDebugLine(GetWorld(), StartLocation, TargetLocation, FColor::Yellow, false, LineTime);

			// Set the value to max to avoid it being hit again for the next time. Kind of resetting it.
			VfxToggleTimes[i] = MaximumFireDelay + 1.f;
			VfxToggleTimes[i - 1] = MaximumFireDelay + 1.f;

			break;
		}
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

