// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHoleProjectile.h"

#include "Arena/Core/ArenaCharacter.h"
#include "Kismet/GameplayStatics.h"

ABlackHoleProjectile::~ABlackHoleProjectile()
{
	if (AArenaCharacter* Player = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Player->UpdateBlackHoleArray();
	}
}

void ABlackHoleProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (AArenaCharacter* Player = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Player->UpdateBlackHoleArray();
	}


	ProjectileType = EProjectileType::BlackHole;
}
