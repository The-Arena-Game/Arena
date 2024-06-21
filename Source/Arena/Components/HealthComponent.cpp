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

	if (AArenaCharacter* ArenaCharacter = Cast<AArenaCharacter>(GetOwner()))
	{
		Player = ArenaCharacter;
		Player->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
	}

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

	if (IsValid(Player) && IsValid(DamageType))
	{
		if (DamageType->IsA(UTurtlePermDamageType::StaticClass()))
		{
			Player->OnTurtlePermHit();
		}
		else if (DamageType->IsA(UTurtleTempDamageType::StaticClass()))
		{
			Player->OnTurtleTempHit();
		}
		else if (DamageType->IsA(UDarknessDamageType::StaticClass()))
		{
			Player->OnDarknessHit();
		}
		else if (DamageType->IsA(UExhaustionDamageType::StaticClass()))
		{
			Player->OnExhaustionHit();
		}
		else
		{
			HeartCount--;
			OnHealthChange.Broadcast(HeartCount);
		}
	}

	if (DamageTakenSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DamageTakenSound, GetOwner()->GetActorLocation());
	}

	if (HeartCount <= 0)
	{
		if (DeathSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetOwner()->GetActorLocation());
		}

		if (IsValid(Player))
		{
			Player->HandleDestruction();
		}
	}
}

