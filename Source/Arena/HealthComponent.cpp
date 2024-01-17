// Fill out your copyright notice in the Description page of Project Settings.
#include "HealthComponent.h"
#include "ArenaCharacter.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
}

void UHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
	if (Damage <= 0.f) return;

	Health -= Damage;

	if (Health <= 0)
	{
		if (AArenaCharacter* Player = GetOwner<AArenaCharacter>())
		{
			Player->CheckHealth();
		}
	}
}

