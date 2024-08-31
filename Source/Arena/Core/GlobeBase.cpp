// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobeBase.h"
#include "ArenaCharacter.h"
#include "ArenaGameMode.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogArnGlobeBase);

// Sets default values
AGlobeBase::AGlobeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Globe Visual"));
	MeshComp->SetupAttachment(RootComponent);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereComp->SetupAttachment(MeshComp);
}

// Called when the game starts or when spawned
void AGlobeBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGlobeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsPlayerTouched())
	{
		if (AArenaGameMode* GameMode = Cast<AArenaGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			bBlue ? GameMode->BlueTouch(this) : GameMode->YellowTouch(this);
		}
	}
}

bool AGlobeBase::IsPlayerTouched()
{
	TArray<AActor*> Actors;
	GetOverlappingActors(Actors);

	for (AActor* Actor : Actors)
	{
		if (AActor* Player = Cast<AArenaCharacter>(Actor))
		{
			return true;
		}
	}

	return false;
}

void AGlobeBase::SnapToGround()
{
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector::UpVector * 10000;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	// Perform the trace
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams))
	{
		// Snap the actor to the ground
		FVector NewLocation = HitResult.Location + FVector(0, 0, SphereComp->GetScaledSphereRadius());
		SetActorLocation(NewLocation);
	}
}


