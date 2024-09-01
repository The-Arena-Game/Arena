// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ArenaBaseData.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ArenaCharacter.generated.h"

class ABlackHoleProjectile;
class AArenaGameMode;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCameraShakeBase;
class UHealthComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogArnCharacter, Log, All);

USTRUCT()
struct FStamina
{
	GENERATED_BODY()

	UPROPERTY()
	float Current = 100.f;

	UPROPERTY(EditAnywhere)
	float Max = 300.f;

	UPROPERTY()
	bool bActive = true;

	/** Stamina increase per second in stand by position */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Per second"))
	float BaseIncrease = 200.f;

	/** Stamina decrease per second when walking */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Should be positive value! Per second"))
	float WalkingDecrease = 0.f;

	/** Stamina decrease per second when running */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Should be positive value! Per second"))
	float SprintDecrease = 800.f;

	/** Turn ON sprinting on stamina value of */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Sprinting will be turned on after this stamina level"))
	float SprintOnLevel = 250.f;

	/** Turn OFF sprinting on stamina value of */
	UPROPERTY(EditAnywhere, meta = (ToolTip = "Sprinting will be turned off after this stamina level"))
	float SprintOffLevel = 10.f;

	/** Turn OFF sprinting on stamina value of */
	UPROPERTY(EditAnywhere)
	float JumpCost = 99999999.9f;

	UPROPERTY()
	float InitialMax = 0.f;

	UPROPERTY()
	float InitialBaseIncrease = 0.f;
};

USTRUCT()
struct FDash
{
	GENERATED_BODY()

	UPROPERTY()
	bool bActive = false;

	UPROPERTY(EditAnywhere)
	float Distance = 400.f;

	UPROPERTY(EditAnywhere)
	float Speed = 2000.f;

	UPROPERTY(EditAnywhere)
	int UsageLimit = 3;

	UPROPERTY(EditAnywhere)
	float CooldownDuration = 2.f;

	UPROPERTY()
	float InitialCooldownDuration = 0.f;

	UPROPERTY()
	float Timer = 0.f;

	// Used for calculating time if the character gets stucked.
	UPROPERTY()
	float DebugTimer = 0.f;

	UPROPERTY()
	int Counter = 0;

	UPROPERTY()
	bool bDashing = false;

	UPROPERTY()
	FVector TargetLocation = FVector::Zero();

	// The distance that is acceptable as reached
	UPROPERTY()
	float ReachThreshold = 20.f;

	UPROPERTY()
	float ObstacleOffset = 50.f;

	UPROPERTY()
	float CharacterFeetDistanceFromCenter = 85.f;

	UPROPERTY()
	float CollisionLineOffset = 50.f;
};

USTRUCT()
struct FFlash
{
	GENERATED_BODY()

	UPROPERTY()
	bool bActive = false;

	UPROPERTY(EditAnywhere)
	float Distance = 600.f;

	UPROPERTY(EditAnywhere)
	int UsageLimit = 3;

	UPROPERTY(EditAnywhere)
	float CooldownDuration = 5.f;

	UPROPERTY()
	float InitialCooldownDuration = 0.f;

	UPROPERTY()
	float Timer = 0.f;

	UPROPERTY()
	int Counter = 0;
};

USTRUCT()
struct FDeflect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ToolTip = "How many times Deflect can be used in a level?"))
	int UsageLimit = 2;

	/** Deflect Duration */
	UPROPERTY(EditAnywhere, meta = (Units = "Seconds", UIMin = 0.1f, UIMax = 10.0f))
	float Duration = 0.75f;

	/** Deflect Duration */
	UPROPERTY(EditAnywhere, meta = (UIMin = 0.1f, UIMax = 30.0f))
	float CooldownDuration = 15.0f;

	// The initial amount we start
	UPROPERTY()
	int DeflectCount = 0;

	// The initial cooldown duration we start with
	UPROPERTY()
	float InitialCooldownDuration = 0.f;

	/** Deflect bar time */
	UPROPERTY()
	float Timer = 0.f;

	/** Count how many times deflect used in a level */
	UPROPERTY()
	int Counter = 0;
};

UCLASS(config = Game)
class AArenaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AArenaCharacter();

	void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const
	{
		return CameraBoom;
	}

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const
	{
		return FollowCamera;
	}

	/** Returns Health Component **/
	FORCEINLINE class UHealthComponent* GetHealthComponent() const
	{
		return HealthComp;
	}

	/** Returns Deflect Progress as percentage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDeflectProgressPercentage() const
	{
		return Deflect.Timer / Deflect.CooldownDuration;
	}

	/** Returns Stamina Progress as percentage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetStaminaProgressPercentage() const
	{
		return Stamina.Current / Stamina.Max;
	}

	/** Returns remaining deflect usage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetDeflectCounter() const
	{
		return Deflect.Counter;
	}

	/** Returns Dash Progress as percentage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDashProgressPercentage() const
	{
		return Dash.Timer / Dash.CooldownDuration;
	}

	/** Returns remaining Dash usage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetDashCounter() const
	{
		return Dash.Counter;
	}

	/** Returns Flash Progress as percentage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetFlashProgressPercentage() const
	{
		return Flash.Timer / Flash.CooldownDuration;
	}

	/** Returns remaining Flash usage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetFashCounter() const
	{
		return Flash.Counter;
	}

	/** Returns Darkness Value (0.f to 1.f) **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDarknessValue() const
	{
		return DarknessValue;
	}

	UFUNCTION()
	void UpdateBlackHoleArray();

	//UFUNCTION()
	//void UnregisterBlackHole(ABlackHoleProjectile* BlackHole)
	//{
	//	BlackHoles.Remove(BlackHole);
	//	bBlackHoleArrayChanged = true;
	//}

	/** Checks Health from the Component. If the Health is 0 or lower, starts destruction **/
	void HandleDestruction();

	/*----------------------------------------------------------------------
		Apply Affects
	----------------------------------------------------------------------*/

	UFUNCTION()
	void OnTurtlePermHit();

	UFUNCTION()
	void OnTurtleTempHit();

	UFUNCTION()
	void OnExhaustionHit();

	UFUNCTION()
	void OnDarknessHit();

	/*----------------------------------------------------------------------
		Buff Functions
	----------------------------------------------------------------------*/

	UFUNCTION()
	FORCEINLINE void ActivateFlash()
	{
		Flash.bActive = true;

		Flash.Timer = Flash.CooldownDuration;
		Flash.Counter = Flash.UsageLimit;
	}

	/** Decreases flash cooldown duration by given percentage */
	UFUNCTION()
	FORCEINLINE void DecreaseFlashCooldownDuration(const float DecreasePercentage)
	{
		Flash.CooldownDuration -= Flash.CooldownDuration * (DecreasePercentage / 100);
	}

	UFUNCTION()
	FORCEINLINE void ActivateDash()
	{
		Dash.bActive = true;

		Dash.Timer = Dash.CooldownDuration;
		Dash.Counter = Dash.UsageLimit;
	}

	/** Decreases Dash cooldown duration by given percentage */
	UFUNCTION()
	FORCEINLINE void DecreaseDashCooldownDuration(const float DecreasePercentage)
	{
		Dash.CooldownDuration -= Dash.CooldownDuration * (DecreasePercentage / 100);
	}

	/** Decreases Deflect cooldown duration by given percentage */
	UFUNCTION()
	FORCEINLINE void DecreaseDeflectCooldownDuration(const float DecreasePercentage)
	{
		Deflect.CooldownDuration -= Deflect.CooldownDuration * (DecreasePercentage / 100);
	}

	/** Decreases Deflect Charge by given Amount */
	UFUNCTION()
	FORCEINLINE void IncreaseDeflectCharge(const int8 IncreaseAmount)
	{
		Deflect.UsageLimit += IncreaseAmount;
	}

	/** Increase Max Stamina by given percentage */
	UFUNCTION()
	FORCEINLINE void IncreaseMaxStamina(const float IncreasePercentage)
	{
		Stamina.Max += Stamina.Max * (IncreasePercentage / 100);
	}

	/** Increase Stamina Base Regeneration by given percentage */
	UFUNCTION()
	FORCEINLINE void IncreaseStaminaRegen(const float IncreasePercentage)
	{
		Stamina.BaseIncrease += Stamina.BaseIncrease * (IncreasePercentage / 100);
	}

	/** Increase Sprint Speed by given percentage */
	UFUNCTION()
	FORCEINLINE void IncreaseSprintSpeed(const float IncreasePercentage)
	{
		SprintSpeed += SprintSpeed * (IncreasePercentage / 100);
	}

	/** Increase Sprint Speed by given percentage */
	UFUNCTION()
	FORCEINLINE void IncreaseWalkSpeed(const float IncreasePercentage)
	{
		WalkSpeed += WalkSpeed * (IncreasePercentage / 100);
		SprintSpeed += SprintSpeed * (IncreasePercentage / 100);
	}

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for sprint input */
	void Sprint();
	void StopSprint();

	/** Called for deflect input */
	void EnableDeflect();
	void DisableDeflect();

	/** Called for Dash input */
	void StartDash();
	void FinishDash();

	/** Called for Flash input */
	void StartFlash();

	/** Called for looking input */
	// Disabled for Top-Down --> void Look(const FInputActionValue& Value);

	/** APawn interface */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** To add mapping context */
	virtual void BeginPlay();

	/** Overrided version of jump, expenses stamina on jump */
	virtual void Jump();

private:

	/*----------------------------------------------------------------------
		Properties
	----------------------------------------------------------------------*/

	UPROPERTY()
	AArenaGameMode* GameMode;

	UPROPERTY()
	EGameStates CurrentGameState;

	bool bPreventStaminaToFill = false;

	float DarknessValue = 0.f;
	float DarknessTimer = 0.f;

	/*----------------------------------------------------------------------
		Base Functions
	----------------------------------------------------------------------*/

	UFUNCTION()
	void OnGameStateChange(EGameStates NewState);

	/*----------------------------------------------------------------------
		Components
	----------------------------------------------------------------------*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComp;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Deflect Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class Camera", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DeflectMesh;

	/*----------------------------------------------------------------------
		Inputs
	----------------------------------------------------------------------*/

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	/** Deflect Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DeflectAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	/** Flash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* FlashAction;

	/** Look Input Action */
	// Disabled for Top-Down 
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class Input", meta = (AllowPrivateAccess = "true"))
	//UInputAction* LookAction;

	/*----------------------------------------------------------------------
		Basics
	----------------------------------------------------------------------*/

	/** Walk Speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 200.f;

	/** Sprint Speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 500.f;

	UPROPERTY()
	float InitialWalkSpeed = 0.f;

	UPROPERTY()
	float InitialSprintSpeed = 0.f;

	UPROPERTY()
	bool bSprinting = false;

	UPROPERTY()
	bool bCanSprint = false;

	/*----------------------------------------------------------------------
		Stamina, Deflect, Dash, Flash
	----------------------------------------------------------------------*/

	UPROPERTY(EditAnywhere, Category = "Arena")
	FStamina Stamina = FStamina();

	UPROPERTY(EditAnywhere, Category = "Arena")
	FDeflect Deflect = FDeflect();

	UPROPERTY(EditAnywhere, Category = "Arena")
	FDash Dash = FDash();

	UPROPERTY(EditAnywhere, Category = "Arena")
	FFlash Flash = FFlash();

	UPROPERTY(EditDefaultsOnly, Category = "Class Effects")
	UParticleSystem* FlashInitialParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Class Effects")
	UParticleSystem* FlashTargetParticle;

	/*----------------------------------------------------------------------
		Black Hole Affect
	----------------------------------------------------------------------*/

	UPROPERTY()
	TArray<ABlackHoleProjectile*> BlackHoles;

	UPROPERTY()
	bool bBlackHoleArrayChanged = false;

	UPROPERTY()
	ABlackHoleProjectile* TargetBlackHole;

	/*----------------------------------------------------------------------
		Handlers
	----------------------------------------------------------------------*/

	UFUNCTION()
	void HandleBlackHoleAffect(const float DeltaTime);
	UFUNCTION()
	void HandleDarkness(const float DeltaTime);
	UFUNCTION()
	void HandleDeflect(const float DeltaTime);
	UFUNCTION()
	void HandleDash(const float DeltaTime);
	UFUNCTION()
	void HandleFlash(const float DeltaTime);
	UFUNCTION()
	void HandleStamina(const float DeltaTime);
	UFUNCTION()
	void HandleDebugLines() const;

	/*----------------------------------------------------------------------
		Utils
	----------------------------------------------------------------------*/

	FVector GetDashLocation(float TargetDistance, bool& SameLocation);

	UFUNCTION()
	void OnRestart();
};