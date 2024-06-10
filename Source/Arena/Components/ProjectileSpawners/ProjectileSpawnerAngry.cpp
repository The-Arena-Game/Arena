#include "ProjectileSpawnerAngry.h"
#include "DrawDebugHelpers.h"
#include "ToolBuilderUtil.h"
#include "Arena/Core/ArenaCharacter.h"
#include "Kismet/GameplayStatics.h"

// Called when the game starts
void UProjectileSpawnerAngry::BeginPlay()
{
	Super::BeginPlay();

	if (AArenaCharacter* Character = Cast<AArenaCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AArenaCharacter::StaticClass())))
	{
		Player = Character;
	}

	HandleDistanceChecks();

	CurrentMaxDelay = MaximumFireDelay;
}

// Called every frame
void UProjectileSpawnerAngry::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(Player))
	{
		return;
	}

	HandleDistanceCheck();
	HandleDebugLines();
}

void UProjectileSpawnerAngry::HandleDistanceCheck()
{
	for (int i = 0; i < DistanceLevels.Num(); i++)
	{
		const float DistanceLevel = DistanceLevels[i];
		const float PlayerDistance = FVector::Distance(GetComponentLocation(), Player->GetActorLocation());

		if (PlayerDistance <= DistanceLevel)
		{
			MinimumFireDelay = MaximumFireDelay = FireDelayLevels[i];

			if (MaximumFireDelay < CurrentMaxDelay)
			{
				CurrentMaxDelay = MaximumFireDelay;
				CurrentFireInterval = 0;	// Set current interval to trigger a fire and get new interval
			}

			return;
		}
	}

	// If we are not in any of range, then pick the longest
	MinimumFireDelay = MaximumFireDelay = FireDelayLevels[FireDelayLevels.Num() - 1];
}

void UProjectileSpawnerAngry::HandleDebugLines()
{
	if (!DisplayDebugLines)
	{
		return;
	}

	for (const float Dist : DistanceLevels)
	{
		DrawDebugCircle(GetWorld(), GetComponentLocation(), Dist, 64, FColor::Red);
	}
}

void UProjectileSpawnerAngry::HandleDistanceChecks()
{
	FString ErrorMessage = TEXT("");

	if (DistanceLevels.Num() == 0 || FireDelayLevels.Num() == 0)
	{
		ErrorMessage = TEXT("The DistanceLevels or FireDelayLevels can not have ZERO element!");
	}

	if (DistanceLevels.Num() != FireDelayLevels.Num())
	{
		ErrorMessage = TEXT("The DistanceLevels and FireDelayLevels arrays should have same size!");
	}

	float TempDist = 0;
	for (const float Dist : DistanceLevels)
	{
		if (Dist == 0)
		{
			ErrorMessage = TEXT("Distance level can not be ZERO on Angry Projectile Spawner!!!");
			break;
		}

		if (Dist < TempDist)
		{
			ErrorMessage = TEXT("Distance levels are not in correct order!!");
			break;
		}

		TempDist = Dist;
	}

	for (float FireDelayLevel : FireDelayLevels)
	{
		if (FireDelayLevel < 0.2f)
		{
			ErrorMessage = TEXT("Minimum Fire Delay Level can not be less than 0.2 seconds!!");
			break;
		}
	}

	if (!ErrorMessage.IsEmpty())
	{
		UE_LOG(LogArnProjectileSpawner, Error, TEXT("%s"), *ErrorMessage);
		IsSystemActive = false;
	}
}



