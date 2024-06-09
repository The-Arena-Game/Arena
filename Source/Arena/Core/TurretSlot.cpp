#include "TurretSlot.h"
#include "ArenaGameMode.h"
#include "Arena/Components/CardManagementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/RectLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Arena/Turrets/TurretBase.h"

DEFINE_LOG_CATEGORY(LogArnTurretSlot);

// Sets default values
ATurretSlot::ATurretSlot()
{
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Base Box Comp"));
	RootComponent = BoxComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	TurretSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Turret Spawn Point"));
	TurretSpawnPoint->SetupAttachment(BaseMesh);

	LightsParent = CreateDefaultSubobject<USceneComponent>(TEXT("Parent Comp for Lights"));
	LightsParent->SetupAttachment(BaseMesh);
}

void ATurretSlot::BeginPlay()
{
	Super::BeginPlay();

	// Get all attached Rect Lights
	const TArray<USceneComponent*>& AttachedChildren = LightsParent->GetAttachChildren();
	for (USceneComponent* ChildComponent : AttachedChildren)
	{
		if (URectLightComponent* RectLightComponent = Cast<URectLightComponent>(ChildComponent))
		{
			RectLightComponent->SetVisibility(false);
			Lights.Add(RectLightComponent);
		}
		else
		{
			UE_LOG(LogArnTurretSlot, Error, TEXT("The Turret Slot (%s) Actor's Parent Light has different child than RectLight!"), *GetActorNameOrLabel());
		}
	}

	// Turn off light at start
	for (URectLightComponent* Light : Lights)
	{
		Light->SetVisibility(false);
	}

	// Bind Restart function and State Change Function
	ArenaGameMode = Cast<AArenaGameMode>(UGameplayStatics::GetGameMode(this));
	ArenaGameMode->OnRestart.AddDynamic(this, &ATurretSlot::OnRestart);
	ArenaGameMode->OnGameStateChange.AddDynamic(this, &ATurretSlot::OnGameStateChange);
	ArenaGameMode->GetCardComp()->OnTurretSelected.AddDynamic(this, &ATurretSlot::OnTurretSelected);

	// Check if any editor time turret spawned
	IsAnyTurretSpawned();
}

bool ATurretSlot::IsTurretPlacementAvailable()
{
	if (!IsValid(ArenaGameMode) || ArenaGameMode->GetArenaGameState() != EGameStates::Prepare || ArenaGameMode->GetLevelNumber() < ActiveLevelNumber)
	{
		return false;
	}

	if (bTurretInstalled)
	{
		return false;
	}

	return true;
}

bool ATurretSlot::IsAnyTurretSpawned()
{
	// Check if any turret is spawned in editor time on the slot
	FHitResult HitResult;
	ECollisionChannel CollisionChannel = ECC_Camera;
	FCollisionQueryParams CollisionParams;
	FVector SpawnLocation = TurretSpawnPoint->GetComponentLocation();

	// Perform the collision check
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		SpawnLocation,
		SpawnLocation,
		FQuat::Identity,
		CollisionChannel,
		FCollisionShape::MakeSphere(50.f),
		CollisionParams
	);

	//// DEBUG
	//DrawDebugSphere(GetWorld(), SpawnLocation, 60.f, 12, FColor::Red, false, 3.f);

	if (bHit)
	{
		if (ATurretBase* Turret = Cast<ATurretBase>(HitResult.GetActor()))
		{
			// UE_LOG(LogArnGameMode, Log, TEXT("Editor Time Spawned Object: %s"), *HitResult.GetActor()->GetActorNameOrLabel());
			bTurretInstalled = true;
			return true;
		}
	}

	return false;
}

void ATurretSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATurretSlot::OnTurretSelected(const ETurretType& Type)
{
	//UE_LOG(LogArnTurretSlot, Log, TEXT("Turret Slot (%s) Received Type: %s"), *GetActorNameOrLabel(), *UEnum::GetDisplayValueAsText(Type).ToString());

	// Check we have turret installed on. 
	if (bTurretInstalled)
	{
		return;
	}
	// If we don't have installed turret at the moment, Check if any runtime turret spawned 
	else if (IsAnyTurretSpawned())
	{
		return;
	}

	// If we are in the active level or above and the CurrentTurret is empty, then lights up!
	if (ArenaGameMode->GetLevelNumber() >= ActiveLevelNumber && !IsValid(CurrentTurret))
	{
		for (URectLightComponent* Light : Lights)
		{
			Light->SetVisibility(true);
		}
	}

	if (IsValid(ArenaGameMode))
	{
		ArenaGameMode->SetGameState(EGameStates::Prepare);
	}

	// Since the Type is selected on the UI, enable mouse over events
	if (APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0))
	{
		Controller->bEnableMouseOverEvents = true;
		Controller->bEnableClickEvents = true;
	}

	LastSelectedType = Type;
}

void ATurretSlot::SlotMouseOver()
{
	if (!IsTurretPlacementAvailable())
	{
		return;
	}

	// If the slot is empty
	if (!IsValid(CurrentTurret))
	{
		// We were using GetPreviewClass to display preview shape. But now, we are using the actual turret for preview as well until we decide what to do with preview.
		CurrentTurret = GetWorld()->SpawnActor<ATurretBase>(GetTurretClass(LastSelectedType), TurretSpawnPoint->GetComponentLocation(), GetActorRotation());
	}
}

void ATurretSlot::SlotMouseLeft()
{
	if (!IsTurretPlacementAvailable())
	{
		return;
	}

	// If there is a turret but not installed before, then it is a preview turret, remove it.
	if (IsValid(CurrentTurret) && !bTurretInstalled)
	{
		CurrentTurret->Destroy();
	}
}

void ATurretSlot::SlotMouseClicked()
{
	if (!IsTurretPlacementAvailable())
	{
		return;
	}

	// If the slot is empty, then there is no preview also! No good!
	if (!IsValid(CurrentTurret))
	{
		UE_LOG(LogArnTurretSlot, Error, TEXT("There is no preview on this slot (%s)!"), *GetActorNameOrLabel());
	}
	else
	{
		CurrentTurret->Destroy();
	}

	CurrentTurret = GetWorld()->SpawnActor<ATurretBase>(GetTurretClass(LastSelectedType), TurretSpawnPoint->GetComponentLocation(), GetActorRotation());
	bTurretInstalled = true;

	for (URectLightComponent* Light : Lights)
	{
		Light->SetVisibility(false);
	}

	// Since the Turret placement is over, disable mouse over events
	if (APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0))
	{
		Controller->bEnableMouseOverEvents = false;
		Controller->bEnableClickEvents = false;
		Controller->bShowMouseCursor = false;
	}

	ArenaGameMode->SetReadyState();
}

void ATurretSlot::SpawnEditorTurret()
{
	GetWorld()->SpawnActor<ATurretBase>(GetTurretClass(EditorTurretType), TurretSpawnPoint->GetComponentLocation(), GetActorRotation());
}

TSubclassOf<ATurretBase> ATurretSlot::GetTurretClass(const ETurretType& Type)
{
	for (TSubclassOf<ATurretBase> Class : TurretClasses)
	{
		ATurretBase* Turret = Class.GetDefaultObject();

		if (!IsValid(Turret))
		{
			UE_LOG(LogArnTurretSlot, Error, TEXT("Turret isn't valid!!!"));
			return nullptr;
		}

		if (Turret->GetTurretType() == Type)
		{
			return Class;
		}
	}

	UE_LOG(LogArnTurretSlot, Error, TEXT("Couldn't find turret type (%s)!"), *UEnum::GetValueAsString<ETurretType>(Type));

	return nullptr;
}

TArray<ETurretType> ATurretSlot::GetTurretTypeOptions()
{
	return {
		ETurretType::SingleTurret, ETurretType::DualTurret, ETurretType::TwinTurret,
		ETurretType::Chonky, ETurretType::Gatling, ETurretType::Triplet,
		ETurretType::Lazer, ETurretType::Knight, ETurretType::MinusSinus
	};
}

void ATurretSlot::OnRestart()
{
	if (IsValid(CurrentTurret))
	{
		CurrentTurret->Destroy();
		bTurretInstalled = false;
	}
}

void ATurretSlot::OnGameStateChange(EGameStates NewState)
{
	if (NewState == EGameStates::Ready)
	{
		for (URectLightComponent* Light : Lights)
		{
			if (IsValid(Light))
			{
				Light->SetVisibility(false);
			}
		}
	}
}