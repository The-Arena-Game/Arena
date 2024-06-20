// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretBase.h"
#include "Arena/Core/ArenaCharacter.h"
#include "Arena/Core/ArenaGameMode.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

DEFINE_LOG_CATEGORY(LogArnTurretBase);

// Sets default values
ATurretBase::ATurretBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create and setup components
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root Capsule"));
	RootComponent = CapsuleComp;
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ForbiddenAreaBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Forbiden Area to Spawn Blue"));
	ForbiddenAreaBoxComp->SetupAttachment(RootComponent);
	ForbiddenAreaBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	// Set Turret and Base Meshes' collisions to custom and set the "Visibility" to "Overlap" to avoid tracing to see itself when targeting!
	BaseMesh->SetCollisionProfileName("Custom");
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BaseMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BaseMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	TurretMesh->SetCollisionProfileName("Custom");
	TurretMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TurretMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TurretMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void ATurretBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AArenaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	GameMode = Cast<AArenaGameMode>(UGameplayStatics::GetGameMode(this));
	GameMode->OnBlueSpawnCollisionStateChange.AddDynamic(this, &ATurretBase::OnSpawnCollisionChange);
}

// Called every frame
void ATurretBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Don't execute any action further if the game state is not Play state
	if (IsValid(GameMode) && GameMode->GetArenaGameState() != EGameStates::Play)
	{
		return;
	}
}

void ATurretBase::OnSpawnCollisionChange(bool NewState)
{
	if (NewState)
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ForbiddenAreaBoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ForbiddenAreaBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}