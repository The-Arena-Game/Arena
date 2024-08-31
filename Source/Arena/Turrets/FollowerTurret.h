// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TurretBase.h"
#include "FollowerTurret.generated.h"

UCLASS()
class AFollowerTurret : public ATurretBase
{
	GENERATED_BODY()

public:
	void Tick(float DeltaTime) override;

protected:
	void RotateTurret(const FVector& TargetLocation, float& DeltaTime);

private:

	UPROPERTY(EditAnywhere, Category = "Arena Combat - Follower", meta = (AllowBlueprintAccess = "true"))
	bool bFollowerTurret = false;

	UPROPERTY(EditAnywhere, Category = "Arena Combat - Follower", meta = (AllowBlueprintAccess = "true", UIMin = "300.0", UIMax = "2000.0"))
	float Range = 500.f;

	UPROPERTY(EditAnywhere, Category = "Arena Combat - Follower", meta = (AllowBlueprintAccess = "true", UIMin = "1.0", UIMax = "10.0"))
	float TurnSpeed = 3.f;

	bool InFireRange() const;
	bool IsFacingToTarget() const;
};
