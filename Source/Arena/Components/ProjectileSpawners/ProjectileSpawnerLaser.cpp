#include "ProjectileSpawnerLaser.h"

// Called when the game starts
void UProjectileSpawnerLaser::BeginPlay()
{
	Super::BeginPlay();

	float Sum = 0.f;
	for (const float Value : LaserPatternValues)
	{
		if (Value < 0.01)
		{
			UE_LOG(LogArnProjectileSpawner, Error, TEXT("Laser VFX pattern value can not be less than 0.01!"));
			bSystemActive = false;
			break;
		}
		Sum += Value;
	}
	if (Sum >= MinimumFireDelay)
	{
		UE_LOG(LogArnProjectileSpawner, Error, TEXT("Minimum fire time is lower than the VFX timer! VFX disabled!"));
		bSystemActive = false;
	}
	else
	{
		// copy the array so we can make sure they are same size
		VfxToggleTimes = LaserPatternValues;
		SetVfxToggleTimes();
	}
}

// Called every frame
void UProjectileSpawnerLaser::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandleVfx();
}

void UProjectileSpawnerLaser::Fire()
{
	Super::Fire();

	SetVfxToggleTimes();
}

void UProjectileSpawnerLaser::SetVfxToggleTimes()
{
	//UE_LOG(LogArnProjectileSpawner, Warning, TEXT("Next Fire Time: %f"), CurrentFireInterval);

	VfxToggleTimes[0] = CurrentFireInterval - LaserPatternValues[0];
	for (int i = 1; i < VfxToggleTimes.Num(); i++)
	{
		VfxToggleTimes[i] = VfxToggleTimes[i - 1] - LaserPatternValues[i]; // subtract from 1 above
		//UE_LOG(LogArnProjectileSpawner, Log, TEXT("Vfx Toggle Time: %f"), VfxToggleTimes[i]);
	}
}

void UProjectileSpawnerLaser::HandleVfx()
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
