#include "TurretSlot.h"
#include "CardSelectionSubsystem.h"
#include "ProjectileSpawner.h"

// Sets default values
ATurretSlot::ATurretSlot()
{
	
}

void ATurretSlot::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATurretSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATurretSlot::CardSelectionListener(ETurretType Type)
{
	UE_LOG(LogTemp, Log, TEXT("Turret Slot (%s) Received Type: %s"), *GetActorNameOrLabel(), *UEnum::GetDisplayValueAsText(Type).ToString());
}

