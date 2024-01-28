// Fill out your copyright notice in the Description page of Project Settings.
#include "HealthComponent.h"
#include "ArenaCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent() { }


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	HeartCount = MaxHeartCount;
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
}

void UHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
	if (Damage <= 0.f || !bIsVulnerable) return;

	if (DamageTakenCameraShakeClass != nullptr)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DamageTakenCameraShakeClass);
	}


	if (DamageTakenSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DamageTakenSound, GetOwner()->GetActorLocation());
	}

	Health -= Damage;
	HeartCount--;

	// If the Heart system is false AND health is 0 - OR - heart system is true and heart count is 0
	if ((!bIsHeartSystemActive && Health <= 0) || (bIsHeartSystemActive && HeartCount <= 0))
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

