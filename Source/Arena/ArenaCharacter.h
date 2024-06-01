// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ArenaBaseData.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ArenaCharacter.generated.h"

class AArenaGameMode;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCameraShakeBase;
class UHealthComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogArnCharacter, Log, All);

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
		return DeflectTimer / DeflectCooldownDuration;
	}

	/** Returns Stamina Progress as percentage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetStaminaProgressPercentage() const
	{
		return CurrentStamina / MaxStamina;
	}

	/** Returns remaining deflect usage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetDeflectCounter() const
	{
		return DeflectCounter;
	}

	/** Returns Dash Progress as percentage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDashProgressPercentage() const
	{
		return DashTimer / DashCooldownDuration;
	}

	/** Returns remaining Dash usage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetDashCounter() const
	{
		return DashCounter;
	}

	/** Returns Flash Progress as percentage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetFlashProgressPercentage() const
	{
		return FlashTimer / FlashCooldownDuration;
	}

	/** Returns remaining Flash usage **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetFlashCounter() const
	{
		return FlashCounter;
	}

	/** Checks Health from the Component. If the Health is 0 or lower, starts destruction **/
	void HandleDestruction();

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

	AArenaGameMode* GameMode;

	UFUNCTION()
	void OnGameStateChange(EGameStates NewState);

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Deflect Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class Camera", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DeflectMesh;

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

	////////////////////////////////	Stamina

	float CurrentStamina = 100.f;
	bool bSprinting = false;
	bool bCanSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float MaxStamina = 100.f;

	bool IsStaminaActive = true;

	/** Stamina increase per second in stand by position */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true", ToolTip = "Per second"))
	float BaseStaminaIncrease = 10.f;

	/** Stamina decrease per second when walking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true", ToolTip = "Should be positive value! Per second"))
	float WalkingStaminaDecrease = 5.f;

	/** Stamina decrease per second when running */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true", ToolTip = "Should be positive value! Per second"))
	float SprintStaminaDecrease = 20.f;

	/** Turn ON sprinting on stamina value of */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true", ToolTip = "Sprinting will be turned on after this stamina level"))
	float SprintOnStaminaLevel = 30.f;

	/** Turn OFF sprinting on stamina value of */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true", ToolTip = "Sprinting will be turned off after this stamina level"))
	float SprintOffStaminaLevel = 10.f;

	/** Turn OFF sprinting on stamina value of */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	float JumpStaminaCost = 20.f;

	////////////////////////////////	Basics

	/** Walk Speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 200.f;

	/** Sprint Speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 500.f;

	////////////////////////////////	Deflect

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true", ToolTip = "How many times Deflect can be used in a level?"))
	int DeflectUsageLimit = 2;

	/** Deflect Duration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true", UIMin = "0.1", UIMax = "10.0"))
	float DeflectDuration = 2.0f;

	/** Deflect Duration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true", UIMin = "0.1", UIMax = "30.0"))
	float DeflectCooldownDuration = 10.f;

	/** Deflect bar time */
	float DeflectTimer = 0.f;

	/** Count how many times deflect used in a level */
	int DeflectCounter = 0;

	////////////////////////////////	Dash

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	float DashDistance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	float DashSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	int DashUsageLimit = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	float DashCooldownDuration = 5.f;

	float DashTimer = 0.f;
	// Used for calculating time if the character gets stucked.
	float DashDebugTimer = 0.f;
	int DashCounter = 0;

	bool IsDashing = false;
	FVector DashTargetLocation = FVector::Zero();

	// The distance that is acceptable as reached
	float DashReachThreshold = 20.f;
	float DashObstacleOffset = 50.f;
	float CharacterFeetDistanceFromCenter = 85.f;
	float CollisionLineOffset = 50.f;

	////////////////////////////////	Flash

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	float FlashDistance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	int FlashUsageLimit = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena", meta = (AllowPrivateAccess = "true"))
	float FlashCooldownDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Class Effects")
	UParticleSystem* FlashInitialParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Class Effects")
	UParticleSystem* FlashTargetParticle;

	float FlashTimer = 0.f;
	int FlashCounter = 0;

	////////////////////////////////	Component

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComp;

	////////////////////////////////	Internal Functions

	FVector GetDashLocation(float TargetDistance, bool& SameLocation);
};