// Fill out your copyright notice in the Description page of Project Settings.
#include "HealthComponent.h"
#include "Arena/Core/ArenaCharacter.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogArnHealthComponent);

UHealthComponent::UHealthComponent()
{
}

void UHealthComponent::OnRoundWin()
{
	if (bRegenActive)
	{
		if (RegenRoundCounter < RegenRoundAmount)
		{
			RegenRoundCounter++;
		}
		else
		{
			RegenRoundCounter = 1;	// Reset the counter
			IncreaseHeartCount(RegenHealthAmount);
		}
	}
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

	InitialMaxHeart = MaxHeartCount;
	HeartCount = MaxHeartCount;
	OnHealthChange.Broadcast(HeartCount, MaxHeartCount);
}

void UHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
	Player->OnDamageTaken();	// Check Auto Deflect

	if (Damage <= 0.f || !bVulnerable) return;

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
			OnHealthChange.Broadcast(HeartCount, MaxHeartCount);
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

