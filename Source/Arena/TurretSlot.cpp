#include "TurretSlot.h"
#include "CardSelectionSubsystem.h"
#include "ProjectileSpawner.h"
#include "TurretBase.h"
#include "Components/BoxComponent.h"
#include "Components/RectLightComponent.h"

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
}

void ATurretSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATurretSlot::CardSelectionListener(ETurretType Type)
{
	UE_LOG(LogTemp, Log, TEXT("Turret Slot (%s) Received Type: %s"), *GetActorNameOrLabel(), *UEnum::GetDisplayValueAsText(Type).ToString());

	if (!IsValid(SingleTurretClass) || !IsValid(SingleTurretPreviewClass) || !IsValid(DualTurretClass) || !IsValid(DualTurretPreviewClass))
	{
		UE_LOG(LogTemp, Error, TEXT("The Turret classes are not defined in TurretSlot BP !!!"));
		return;
	}

	if (Type == ETurretType::SingleTurret)
	{
		for (URectLightComponent* Light : Lights)
		{
			Light->SetVisibility(false);
		}

		if (!IsValid(CurrentTurret))
		{
			CurrentTurret = GetWorld()->SpawnActor<ATurretBase>(SingleTurretClass, TurretSpawnPoint->GetComponentLocation(), GetActorRotation());
		}		
	} 
	else if (Type == ETurretType::DualTurret)
	{
		for (URectLightComponent* Light : Lights)
		{
			Light->SetVisibility(false);
		}

		if (!IsValid(CurrentTurret))
		{
			CurrentTurret = GetWorld()->SpawnActor<ATurretBase>(SingleTurretPreviewClass, TurretSpawnPoint->GetComponentLocation(), GetActorRotation());
		}
	}
	else
	{
		for (URectLightComponent* Light : Lights)
		{
			Light->SetVisibility(true);
		}

		if (IsValid(CurrentTurret))
		{
			UE_LOG(LogTemp, Warning, TEXT("Current is valid, destroying..."));
			CurrentTurret->Destroy();
		}
		else{
			UE_LOG(LogTemp, Warning, TEXT("There is no current !!"));
		}
	}
}

