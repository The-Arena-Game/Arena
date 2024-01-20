// Fill out your copyright notice in the Description page of Project Settings.
#include "HealthComponent.h"
#include "ArenaCharacter.h"
#include "Kismet/GameplayStatics.h"

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

	if (DamageTakenCameraShakeClass != nullptr)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DamageTakenCameraShakeClass);
	}


	if (DamageTakenSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DamageTakenSound, GetOwner()->GetActorLocation());
	}

	if (Health <= 0)
	{
		if (DeathSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetOwner()->GetActorLocation());
		}

		if (AArenaCharacter* Player = GetOwner<AArenaCharacter>())
		{
			Player->HandleDestruction();
		}
	}
}

