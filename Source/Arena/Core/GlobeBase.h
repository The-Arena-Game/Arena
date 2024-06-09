// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobeBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArnGlobeBase, Log, All);

class USphereComponent;

UCLASS()
class AGlobeBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGlobeBase();

	// Called every frame
	void Tick(float DeltaTime) override;

	/** Snaps the globe to the ground */
	void SnapToGround();

	FORCEINLINE bool IsBlue()
	{
		return bIsBlue;
	}

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

private:

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnyWhere, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	bool bIsBlue = false;

	bool IsPlayerTouched();

};
