// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	FORCEINLINE float GetHealth() const { return Health; }

protected:
	void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Arena Combat", meta = (AllowPrivateAccess = "true", UIMin = "1.0", UIMax = "100.0"))
	float MaxHealth = 100.f;
	float Health;

	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser);
};
