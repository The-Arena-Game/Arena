#include "ArenaGameMode.h"
#include "CardSelectionSubsystem.h"
#include "Components/BoxComponent.h"
#include "Components/RectLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectileSpawner.h"
#include "TurretBase.h"
#include "TurretSlot.h"

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

bool ATurretSlot::AreAllComponentsSet()
{
	bool AllGood = !IsValid(SingleTurretClass) || !IsValid(DualTurretClass) || !IsValid(TwinTurretClass) || !IsValid(SingleTurretPreviewClass) || !IsValid(DualTurretPreviewClass);

	if (AllGood)
	{
		UE_LOG(LogTemp, Error, TEXT("The Turret classes are not defined in TurretSlot BP !!!"));
		return false;
	}

	return true;
}

void ATurretSlot::BeginPlay()
{
	Super::BeginPlay();

	if (!AreAllComponentsSet())
	{
		return;
	}

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
			UE_LOG(LogTemp, Error, TEXT("The Turret Slot (%s) Actor's Parent Light has different child than RectLight!"), *GetActorNameOrLabel());
		}
	}

	if (UCardSelectionSubsystem* CardSelectionSS = GetGameInstance()->GetSubsystem<UCardSelectionSubsystem>())
	{
		CardSelectionSS->OnCardSelection.AddDynamic(this, &ATurretSlot::CardSelectionListener);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't get the UCardSelecitonSubsystem!"));
	}

	// Turn off light at start
	for (URectLightComponent* Light : Lights)
	{
		Light->SetVisibility(false);
	}

	// Set Restrat function
	ArenaGameMode = Cast<AArenaGameMode>(UGameplayStatics::GetGameMode(this));
	ArenaGameMode->OnRestart.AddDynamic(this, &ATurretSlot::OnRestart);
	ArenaGameMode->OnGameStateChange.AddDynamic(this, &ATurretSlot::OnGameStateChange);
}

void ATurretSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATurretSlot::CardSelectionListener(ETurretType Type)
{
	//UE_LOG(LogTemp, Log, TEXT("Turret Slot (%s) Received Type: %s"), *GetActorNameOrLabel(), *UEnum::GetDisplayValueAsText(Type).ToString());

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
	if (!IsValid(ArenaGameMode) || ArenaGameMode->GetGameState() != EGameStates::Prepare || ArenaGameMode->GetLevelNumber() < ActiveLevelNumber)
	{
		return;
	}

	// If the slot is empty
	if (!IsValid(CurrentTurret))
	{
		// We were using GetPreviewClass to display preview shape. But now, we are using the actual turret for preview as well until we decide what to do with preview.
		CurrentTurret = GetWorld()->SpawnActor<ATurretBase>(GetActualClass(LastSelectedType), TurretSpawnPoint->GetComponentLocation(), GetActorRotation());
	}
}

void ATurretSlot::SlotMouseLeft()
{
	if (!IsValid(ArenaGameMode) || ArenaGameMode->GetGameState() != EGameStates::Prepare || ArenaGameMode->GetLevelNumber() < ActiveLevelNumber)
	{
		return;
	}

	// If there is a turret but not installed before, then it is a preview turret, remove it.
	if (IsValid(CurrentTurret) && !bIsInstalled)
	{
		CurrentTurret->Destroy();
	}
}

void ATurretSlot::SlotMouseClicked()
{
	if (!IsValid(ArenaGameMode) || ArenaGameMode->GetGameState() != EGameStates::Prepare || ArenaGameMode->GetLevelNumber() < ActiveLevelNumber)
	{
		return;
	}

	// If the slot is empty, then there is no preview also! No good!
	if (!IsValid(CurrentTurret))
	{
		UE_LOG(LogTemp, Error, TEXT("There is no preview on this slot (%s)!"), *GetActorNameOrLabel());
	}
	else
	{
		CurrentTurret->Destroy();
	}

	CurrentTurret = GetWorld()->SpawnActor<ATurretBase>(GetActualClass(LastSelectedType), TurretSpawnPoint->GetComponentLocation(), GetActorRotation());
	bIsInstalled = true;

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

TSubclassOf<ATurretBase> ATurretSlot::GetPreviewClass(ETurretType Type)
{
	TSubclassOf<ATurretBase> PreviewClass;

	switch (Type)
	{
	case ETurretType::SingleTurret:
		PreviewClass = SingleTurretPreviewClass;
		break;
	case ETurretType::DualTurret:
		PreviewClass = DualTurretPreviewClass;
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("Received unregistered turret type (%s) on the Turret Slot!"), *UEnum::GetValueAsString<ETurretType>(Type));
		break;
	}

	return PreviewClass;
}

TSubclassOf<ATurretBase> ATurretSlot::GetActualClass(ETurretType Type)
{
	TSubclassOf<ATurretBase> ActualClass;

	switch (Type)
	{
	case ETurretType::SingleTurret:
		ActualClass = SingleTurretClass;
		break;
	case ETurretType::DualTurret:
		ActualClass = DualTurretClass;
		break;
	case ETurretType::TwinTurret:
		ActualClass = TwinTurretClass;
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("Received unregistered turret type (%s) on the Turret Slot!"), *UEnum::GetValueAsString<ETurretType>(Type));
		break;
	}

	return ActualClass;
}

void ATurretSlot::OnRestart()
{
	if (IsValid(CurrentTurret))
	{
		CurrentTurret->Destroy();
		bIsInstalled = false;
	}
}

void ATurretSlot::OnGameStateChange(EGameStates NewState)
{
	if (NewState == EGameStates::Ready)
	{
		for (URectLightComponent* Light : Lights)
		{
			Light->SetVisibility(false);
		}
	}
}