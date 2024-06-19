// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "BlackHoleProjectile.generated.h"

/**
 *
 */
UCLASS()
class ARENA_API ABlackHoleProjectile : public AProjectileBase
{
	GENERATED_BODY()

	ABlackHoleProjectile();
	~ABlackHoleProjectile();

public:

	UPROPERTY(EditDefaultsOnly, Category = "Arena - Blackhole")
	float MaxRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena - Blackhole", meta = (UIMin = "20", UIMax = "200"))
	float PullSpeed = 50.f;

protected:

	virtual void BeginPlay() override;

};
