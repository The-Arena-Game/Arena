// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "LobProjectile.generated.h"

/**
 *
 */
UCLASS()
class ARENA_API ALobProjectile : public AProjectileBase
{
	GENERATED_BODY()

protected:

	void BeginPlay() override;
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectileBase> ProjectileClass;

};
