// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArenaCharacter.h"
#include "HealthComponent.h"
#include "ArenaGameMode.h"
#include "GlobeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogArnCharacter);

//////////////////////////////////////////////////////////////////////////
// AArenaCharacter

AArenaCharacter::AArenaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 10000000000000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 10000000000000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Top-Down - Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.0f;		// The Camera follows the character from a great distance
	// Disabled for Top-Down View --> CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SetRelativeRotation(FRotator(-40.f, 0.f, 0.f));		// Top-Down - Perspective
	CameraBoom->bDoCollisionTest = false;	// Top-Down - Don't want to pull camera in when it collides with level

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset (to avoid direct content references in C++)

	// Create Deflect Static Mesh
	DeflectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Deflect Mesh"));
	DeflectMesh->SetupAttachment(RootComponent);
	DeflectMesh->SetHiddenInGame(true);	// Disable Deflect mesh at start

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));

	GameMode = Cast<AArenaGameMode>(UGameplayStatics::GetGameMode(this));
}

void AArenaCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Disable acceleration by setting it too high
	GetCharacterMovement()->MaxAcceleration = 10000000000000000000000000000000000.f;

	DeflectTimer = 0.f;
}

void AArenaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Increase the timer when needed
	if (DeflectTimer < DeflectCooldownDuration)
	{
		DeflectTimer += DeltaTime;
	}
	else
	{
		// Make sure it is equal to the max duration if not increased
		DeflectTimer = DeflectCooldownDuration;
	}

	// Draw blue line for 
	if (IsValid(GameMode) && GameMode->GetGameState() == EGameStates::Ready)
	{
		if (IsValid(GameMode->GetBlueGlobe()))
		{
			FVector TargetLoc = GameMode->GetBlueGlobe()->GetActorLocation();

			DrawDebugLine(
				GetWorld(),
				GetActorLocation(),
				TargetLoc,
				FColor::Cyan
			);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AArenaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AArenaCharacter::Move);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AArenaCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AArenaCharacter::StopSprint);

		// Deflect
		EnhancedInputComponent->BindAction(DeflectAction, ETriggerEvent::Started, this, &AArenaCharacter::EnableDeflect);

		// Looking
		// Disabled for Top-Down EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AArenaCharacter::Look);
	}
	else
	{
		UE_LOG(LogArnCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AArenaCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
	    // Top-Down -> Changed the PlayerController with CameraBoom
		const FRotator Rotation = CameraBoom->GetRelativeRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AArenaCharacter::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AArenaCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AArenaCharacter::EnableDeflect()
{
	// If the deflect timer is not full, don't enable deflect
	if (DeflectTimer < DeflectCooldownDuration)
	{
		return;
	}

	DeflectTimer = 0;	// Reset timer

	// Make the healt component invulnerable
	HealthComp->SetValnerable(false);

	// Enable the mesh
	DeflectMesh->SetHiddenInGame(false);

	// Start the timer to disable it
	FTimerHandle TimerHandle; // Create a local dummy handle. We won’t use it.
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AArenaCharacter::DisableDeflect, DeflectDuration, false);
}

void AArenaCharacter::DisableDeflect()
{
	// Make the healt component open to fire again
	HealthComp->SetValnerable(true);

	// Disablethe mesh
	DeflectMesh->SetHiddenInGame(true);
}

// Disabled for Top-Down 
/*

void AArenaCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

*/

void AArenaCharacter::HandleDestruction()
{
	// TODO: SFX, VFX, etc.
	if (!HealthComp->IsDead())
	{
		return;
	}
	
	if (IsValid(GameMode))
	{
		GameMode->HandlePlayerDeath();
	}

	Destroy();
}