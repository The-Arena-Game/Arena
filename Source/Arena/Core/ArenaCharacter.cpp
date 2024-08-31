// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArenaCharacter.h"
#include "ArenaGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GlobeBase.h"
#include "Arena/Components/HealthComponent.h"
#include "InputActionValue.h"
#include "Arena/Projectiles/BlackHoleProjectile.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogArnCharacter);

/*----------------------------------------------------------------------
		Base
----------------------------------------------------------------------*/

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
}

void AArenaCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	GameMode = Cast<AArenaGameMode>(UGameplayStatics::GetGameMode(this));
	GameMode->OnGameStateChange.AddDynamic(this, &AArenaCharacter::OnGameStateChange);
	GameMode->OnRestart.AddDynamic(this, &AArenaCharacter::OnRestart);

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

	// Save the initial amount to reset on restart
	Flash.InitialCooldownDuration = Flash.CooldownDuration;
	Dash.InitialCooldownDuration = Dash.CooldownDuration;

	Deflect.InitialCooldownDuration = Deflect.CooldownDuration;
	Deflect.DeflectCount = Deflect.UsageLimit;

	Stamina.InitialMax = Stamina.Max;
	Stamina.InitialBaseIncrease = Stamina.BaseIncrease;

	InitialSprintSpeed = SprintSpeed;

	// Start with initial values
	OnRestart();
}

void AArenaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Character Movement Component bug fix - By forcing to move, we are activating collision detection
	FHitResult OutHit;
	GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, 0.01f), GetActorRotation(), true, OutHit);
	GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, -0.01f), GetActorRotation(), true, OutHit);

	HandleDarkness(DeltaTime);
	HandleDeflect(DeltaTime);
	HandleDash(DeltaTime);
	HandleFlash(DeltaTime);
	HandleStamina(DeltaTime);
	HandleDebugLines();
	HandleBlackHoleAffect(DeltaTime);
}

/*----------------------------------------------------------------------
		Inputs
----------------------------------------------------------------------*/

void AArenaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AArenaCharacter::Jump);	// Using overrided version here
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AArenaCharacter::Move);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AArenaCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AArenaCharacter::StopSprint);

		// Deflect, Dash, Flash
		EnhancedInputComponent->BindAction(DeflectAction, ETriggerEvent::Started, this, &AArenaCharacter::EnableDeflect);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AArenaCharacter::StartDash);
		EnhancedInputComponent->BindAction(FlashAction, ETriggerEvent::Started, this, &AArenaCharacter::StartFlash);

		// Looking
		// Disabled for Top-Down EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AArenaCharacter::Look);
	}
	else
	{
		UE_LOG(LogArnCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

/*----------------------------------------------------------------------
		Movement
----------------------------------------------------------------------*/

void AArenaCharacter::Move(const FInputActionValue& Value)
{
	if (CurrentGameState == EGameStates::Menu || Dash.IsDashing)
	{
		return;
	}

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
	if (bCanSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bSprinting = true;
	}
}

void AArenaCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bSprinting = false;
}

void AArenaCharacter::Jump()
{
	if (Stamina.Current >= Stamina.JumpCost)
	{
		Super::Jump();
		Stamina.Current -= Stamina.JumpCost;
	}
}

/*----------------------------------------------------------------------
		Others
----------------------------------------------------------------------*/

void AArenaCharacter::OnGameStateChange(EGameStates NewState)
{
	Stamina.bIsActive = (NewState == EGameStates::Play) ? true : false;

	if (NewState == EGameStates::Win || NewState == EGameStates::Ready)
	{
		Stamina.Current = Stamina.Max;

		Deflect.Timer = Deflect.CooldownDuration;
		Deflect.Counter = Deflect.UsageLimit;

		Dash.Timer = Dash.bIsActive ? Dash.CooldownDuration : 0;
		Dash.Counter = Dash.UsageLimit;

		Flash.Timer = Flash.bIsActive ? Flash.CooldownDuration : 0;
		Flash.Counter = Flash.UsageLimit;
	}

	CurrentGameState = NewState;
}

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

/*----------------------------------------------------------------------
		Projectile Affects
----------------------------------------------------------------------*/

void AArenaCharacter::UpdateBlackHoleArray()
{
	BlackHoles.Empty();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, ABlackHoleProjectile::StaticClass(), Actors);

	TargetBlackHole = nullptr;
	float Distance = 1000000000.f;

	for (AActor* Actor : Actors)
	{
		if (ABlackHoleProjectile* BlackHole = Cast<ABlackHoleProjectile>(Actor))
		{
			BlackHoles.Add(BlackHole);

			if (FVector::Dist(GetActorLocation(), BlackHole->GetActorLocation()) < Distance)
			{
				TargetBlackHole = BlackHole;
				Distance = FVector::Dist(GetActorLocation(), BlackHole->GetActorLocation());
			}
		}
	}

	if (!IsValid(TargetBlackHole))
	{
		BlackHoles.Empty();
	}
}

void AArenaCharacter::OnTurtlePermHit()
{
	WalkSpeed *= UTurtlePermDamageType::DecreasePercentage;
	SprintSpeed *= UTurtlePermDamageType::DecreasePercentage;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AArenaCharacter::OnTurtleTempHit()
{
	const float ReducedWalkSpeed = WalkSpeed * UTurtleTempDamageType::DecreasePercentage;
	const float ReducedSprintSpeed = SprintSpeed * UTurtleTempDamageType::DecreasePercentage;

	WalkSpeed -= ReducedWalkSpeed;
	SprintSpeed -= ReducedSprintSpeed;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, ReducedWalkSpeed, ReducedSprintSpeed]()
		{
			WalkSpeed += ReducedWalkSpeed;
			SprintSpeed += ReducedSprintSpeed;
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}, UTurtleTempDamageType::Time, false);
}

void AArenaCharacter::OnExhaustionHit()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}

	Stamina.Current = 0.f;
	bPreventStaminaToFill = true;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]
		{
			bPreventStaminaToFill = false;
		}, UExhaustionDamageType::Time, false
	);
}

void AArenaCharacter::OnDarknessHit()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}

	DarknessTimer = 0;
	DarknessValue = 1.f;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]
		{
			if (DarknessTimer <= 0)
			{
				DarknessTimer = UDarknessDamageType::RevealingTime;
			}
		}, UDarknessDamageType::FullBlockTime, false
	);
}

/*----------------------------------------------------------------------
		Special Movement
----------------------------------------------------------------------*/

FVector AArenaCharacter::GetDashLocation(float TargetDistance, bool& SameLocation)
{
	// We are drawing 2 lines to check if there is something. One from left, one from right.
	FHitResult LeftHitResult;
	FHitResult RightHitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FVector StartLocation = GetActorLocation() - FVector(0.f, 0.f, Dash.CharacterFeetDistanceFromCenter);	// send the line to the feet
	FVector TargetLocation = GetActorLocation() + GetActorForwardVector() * TargetDistance; // Get the desired location
	TargetLocation -= FVector(0.f, 0.f, Dash.CharacterFeetDistanceFromCenter);	// send the line to the feet

	// Offset the start location to little back avoid getting inside of thin walls
	StartLocation -= FVector(Dash.ObstacleOffset, Dash.ObstacleOffset, 1.f) * GetActorForwardVector();

	// Right-Offset line
	FVector StartLocation_Right = StartLocation + FVector(1.f, Dash.CollisionLineOffset, 1.f) * GetActorForwardVector();
	FVector TargetLocation_Right = TargetLocation + FVector(1.f, Dash.CollisionLineOffset, 1.f) * GetActorForwardVector();
	//DrawDebugLine(GetWorld(), StartLocation_Right, TargetLocation_Right, FColor::Yellow, false, 3.f);	// draw line
	bool bHitRight = GetWorld()->LineTraceSingleByChannel(RightHitResult, StartLocation_Right, TargetLocation_Right, ECC_Visibility, CollisionParams);

	//// Left-Offset line
	FVector StartLocation_Left = StartLocation - FVector(1.f, Dash.CollisionLineOffset, 0.f) * GetActorForwardVector();
	FVector TargetLocation_Left = TargetLocation - FVector(1.f, Dash.CollisionLineOffset, 0.f) * GetActorForwardVector();
	//DrawDebugLine(GetWorld(), StartLocation_Left, TargetLocation_Left, FColor::Purple, false, 3.f);	// draw line
	bool bHitLeft = GetWorld()->LineTraceSingleByChannel(LeftHitResult, StartLocation_Left, TargetLocation_Left, ECC_Visibility, CollisionParams);

	if (bHitRight || bHitLeft)
	{
		// Take the one hit
		FHitResult HitResult = bHitRight ? RightHitResult : LeftHitResult;

		//UE_LOG(LogArnGameMode, Log, TEXT("Hit: %s"), *HitResult.GetActor()->GetActorNameOrLabel());
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 20.f, 12, FColor::Red, false, 3.f);

		// Select the target location with an offset towards the back (to the middle, where we turned behind) to avoid stucking into the obstacle!
		TargetLocation = HitResult.ImpactPoint - Dash.ObstacleOffset * GetActorForwardVector();
		TargetLocation.Z = GetActorLocation().Z;	// Adjust the height of the target to the Actors height

		// Calculate if the target location falls behind the character
		FVector CharacterToTargetLocation = TargetLocation - GetActorLocation();
		CharacterToTargetLocation.Normalize();
		float DotProduct = FVector::DotProduct(CharacterToTargetLocation, GetActorForwardVector());

		// If so, take ne current location as the target. Else, don't do anything and accept the target.
		if (DotProduct < 0)
		{
			SameLocation = true;
			//FVector DebugLoc = GetActorLocation() - GetActorForwardVector() * 5.f;
			//DrawDebugSphere(GetWorld(), DebugLoc, 20.f, 12, FColor::Red, false, 3.f);
			return GetActorLocation() - GetActorForwardVector() * 5.f;
		}
		else
		{
			//DrawDebugSphere(GetWorld(), TargetLocation, 20.f, 12, FColor::Red, false, 3.f);
			return TargetLocation;
		}
	}
	else
	{
		// If doesn't hit anything, just calculate the target by distance. 
		// But, again, take it little bit back to avoid getting stuck into obstacles that are not collides but nearby the target and causes bugs.
		//FVector DebugLoc = GetActorLocation() + GetActorForwardVector() * (TargetDistance - 50.f);
		//DrawDebugSphere(GetWorld(), DebugLoc, 20.f, 12, FColor::Red, false, 3.f);
		return GetActorLocation() + GetActorForwardVector() * (TargetDistance - 50.f);
	}
}

void AArenaCharacter::EnableDeflect()
{
	// If the deflect timer is not full, don't enable deflect
	if (Deflect.Timer < Deflect.CooldownDuration)
	{
		return;
	}

	// Don't use deflect if the state is not Play
	if (GameMode->GetArenaGameState() != EGameStates::Play)
	{
		return;
	}

	Deflect.Timer = 0;	// Reset timer
	Deflect.Counter--;

	// Make the healt component invulnerable
	HealthComp->SetValnerable(false);

	// Enable the mesh
	DeflectMesh->SetHiddenInGame(false);

	// Start the timer to disable it
	FTimerHandle TimerHandle; // Create a local dummy handle. We won’t use it.
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AArenaCharacter::DisableDeflect, Deflect.Duration, false);
}

void AArenaCharacter::DisableDeflect()
{
	// Make the healt component open to fire again
	HealthComp->SetValnerable(true);

	// Disablethe mesh
	DeflectMesh->SetHiddenInGame(true);
}

void AArenaCharacter::StartDash()
{
	// If the the timer is not full OR is dashing OR dash is not active, don't execute action
	if (Dash.Timer < Dash.CooldownDuration || Dash.IsDashing || !Dash.bIsActive)
	{
		return;
	}

	// Don't use dash if the state is not Play
	if (GameMode->GetArenaGameState() != EGameStates::Play)
	{
		return;
	}

	bool SameLocation = false;
	Dash.TargetLocation = GetDashLocation(Dash.Distance, SameLocation);
	// DrawDebugSphere(GetWorld(), Dash.TargetLocation, 60.f, 12, FColor::Green, false, 3.f);

	// If the location is not changed, don't execute dash
	if (SameLocation)
	{
		return;
	}

	Dash.Timer = 0;	// Reset timer
	// Dash.Counter--; REMOVED
	Dash.IsDashing = true;
}

void AArenaCharacter::FinishDash()
{
	Dash.IsDashing = false;
}

void AArenaCharacter::StartFlash()
{
	// If the the timer is not full OR is dashing OR flash is not active, don't execute action
	if (Flash.Timer < Flash.CooldownDuration || Dash.IsDashing || !Flash.bIsActive)
	{
		return;
	}

	// Don't use fash if the state is not Play
	if (GameMode->GetArenaGameState() != EGameStates::Play)
	{
		return;
	}

	// Get locations
	bool SameLocation = false;
	FVector InitialLocation = GetActorLocation();
	FVector TargetLocation = GetDashLocation(Flash.Distance, SameLocation);

	// Unreal has built-in teleport function but ours more functional for our purpose.
	// FVector TargetLocation = GetActorLocation() + GetActorForwardVector() * (Flash.Distance - 50.f);
	// SetActorLocation(TargetLocation, true);

	// If the location is not changed, don't execute Flash
	if (SameLocation)
	{
		return;
	}

	// Teleport
	SetActorLocation(TargetLocation);
	// DrawDebugSphere(GetWorld(), TargetLocation, 60.f, 12, FColor::Green, false, 3.f);

	Flash.Timer = 0;	// Reset timer
	// Flash.Counter--; REMOVED

	// TODO: Perform VFX, SFX
	if (FlashInitialParticle != nullptr && FlashTargetParticle != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, FlashInitialParticle, InitialLocation, GetActorRotation());
		UGameplayStatics::SpawnEmitterAtLocation(this, FlashTargetParticle, GetActorLocation(), GetActorRotation());
	}
	else
	{
		UE_LOG(LogArnCharacter, Warning, TEXT("The Flash Particles Effects are not set!"));
	}
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

/*----------------------------------------------------------------------
		Handlers
----------------------------------------------------------------------*/

void AArenaCharacter::HandleBlackHoleAffect(const float DeltaTime)
{
	if (BlackHoles.IsEmpty() || BlackHoles.Num() == 0 || CurrentGameState != EGameStates::Play)
	{
		return;
	}

	if (!IsValid(TargetBlackHole))
	{
		UpdateBlackHoleArray();
		return;
	}

	// Movement Action
	float TargetDistance = FVector::Distance(TargetBlackHole->GetActorLocation(), GetActorLocation());
	if (TargetDistance < TargetBlackHole->MaxRange)
	{
		// float ReverseDistance = TargetBlackHole->MaxRange - TargetDistance;

		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetBlackHole->GetActorLocation(), DeltaTime, TargetBlackHole->PullSpeed);
		SetActorLocation(NewLocation);
	}
}

void AArenaCharacter::HandleDarkness(const float DeltaTime)
{
	if (DarknessTimer > 0)
	{
		DarknessTimer = FMath::Clamp(DarknessTimer, 0.f, DarknessTimer - DeltaTime);
		DarknessValue = DarknessTimer / UDarknessDamageType::RevealingTime;
	}
}

void AArenaCharacter::HandleDeflect(const float DeltaTime)
{
	// If there is a usage limit, process deflect timer
	if (Deflect.Counter > 0)
	{
		// Increase the timer when it is not full
		if (Deflect.Timer < Deflect.CooldownDuration)
		{
			Deflect.Timer += DeltaTime;
		}
		else
		{
			// Make sure it is equal to the max duration if not increased
			Deflect.Timer = Deflect.CooldownDuration;
		}
	}
}

void AArenaCharacter::HandleDash(const float DeltaTime)
{
	if (!Dash.bIsActive)
	{
		return;
	}

	// REMOVED: If there is a usage limit, process dash timer
	// Increase the timer when it is not full
	if (Dash.Timer < Dash.CooldownDuration)
	{
		Dash.Timer += DeltaTime;
	}
	else
	{
		// Make sure it is equal to the max duration if not increased
		Dash.Timer = Dash.CooldownDuration;
	}

	if (Dash.IsDashing)
	{
		// Start movement
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, Dash.TargetLocation, DeltaTime, Dash.Speed);
		SetActorLocation(NewLocation);

		// Check if the actor has reached the target location
		float DistanceToTarget = FVector::DistSquared(CurrentLocation, Dash.TargetLocation);
		if (DistanceToTarget < FMath::Square(Dash.ReachThreshold))
		{
			//UE_LOG(LogArnCharacter, Log, TEXT("REACHED TO THE TARGET !!"));
			FinishDash();
		}

		// Release dash if it exceeds 1 second - Avoids bug
		Dash.DebugTimer += DeltaTime;
		if (Dash.DebugTimer > 1.f)
		{
			Dash.DebugTimer = 0;
			FinishDash();
		}
	}
}

void AArenaCharacter::HandleFlash(const float DeltaTime)
{
	if (!Flash.bIsActive)
	{
		return;
	}

	// REMOVED: If there is a usage limit, process Flash timer
	// Increase the timer when it is not full
	if (Flash.Timer < Flash.CooldownDuration)
	{
		Flash.Timer += DeltaTime;
	}
	else
	{
		// Make sure it is equal to the max duration if not increased
		Flash.Timer = Flash.CooldownDuration;
	}
}

void AArenaCharacter::HandleStamina(const float DeltaTime)
{
	if (!bPreventStaminaToFill)
	{
		Stamina.Current += Stamina.BaseIncrease * DeltaTime;
	}

	// Check if moving?
	if (Stamina.bIsActive && GetCharacterMovement()->Velocity.Size() > 0.0f)
	{
		Stamina.Current -= Stamina.WalkingDecrease * DeltaTime;

		if (Stamina.bIsActive && bSprinting)
		{
			Stamina.Current -= Stamina.SprintDecrease * DeltaTime;
		}
	}

	// Make sure the value stays between boundries
	if (Stamina.Current < 0)
	{
		Stamina.Current = 0;
	}
	else if (Stamina.Current > Stamina.Max)
	{
		Stamina.Current = Stamina.Max;
	}

	// When we are able to sprint, if the stamina drop below the Off limit, turn off sprinting
	if (bCanSprint && Stamina.Current < Stamina.SprintOffLevel)
	{
		bCanSprint = false;
		StopSprint();
	}
	else if (!bCanSprint && Stamina.Current > Stamina.SprintOnLevel)
	{
		bCanSprint = true;
	}
}

void AArenaCharacter::HandleDebugLines() const
{
	// Draw blue line for 
	if (IsValid(GameMode) && GameMode->GetArenaGameState() == EGameStates::Ready)
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

void AArenaCharacter::OnRestart()
{
	SprintSpeed = InitialSprintSpeed;

	Stamina.BaseIncrease = Stamina.InitialBaseIncrease;
	Stamina.Max = Stamina.InitialMax;
	Stamina.Current = Stamina.Max;

	Deflect.CooldownDuration = Deflect.InitialCooldownDuration;
	Deflect.Timer = Deflect.InitialCooldownDuration;
	Deflect.UsageLimit = Deflect.DeflectCount;
	Deflect.Counter = Deflect.DeflectCount;

	Dash.bIsActive = false;
	Dash.CooldownDuration = Dash.InitialCooldownDuration;
	Dash.Timer = 0;
	Dash.Counter = Dash.UsageLimit;

	Flash.bIsActive = false;
	Flash.CooldownDuration = Flash.InitialCooldownDuration;
	Flash.Timer = 0;
	Flash.Counter = Flash.UsageLimit;
}