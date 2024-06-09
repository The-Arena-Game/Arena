// Fill out your copyright notice in the Description page of Project Settings.
#include "HealthComponent.h"
#include "Arena/Core/ArenaCharacter.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogArnHealthComponent);

UHealthComponent::UHealthComponent()
{
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);

	HeartCount = MaxHeartCount;
	OnHealthChange.Broadcast(HeartCount);
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

	HeartCount--;
	OnHealthChange.Broadcast(HeartCount);

	if (HeartCount <= 0)
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

