// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns Health Component **/
	FORCEINLINE class UHealthComponent* GetHealthComponent() const { return HealthComp; }

	/** Returns Deflect Timer **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDeflectTimer() const { return DeflectTimer; }

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

	/** Called for looking input */
	// Disabled for Top-Down --> void Look(const FInputActionValue& Value);

	/** APawn interface */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** To add mapping context */
	virtual void BeginPlay();

private:

	AArenaGameMode* GameMode;

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

	/** Look Input Action */
	// Disabled for Top-Down 
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class Input", meta = (AllowPrivateAccess = "true"))
	//UInputAction* LookAction;

	/** Walk Speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 200.f;

	/** Sprint Speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 500.f;

	/** Deflect Duration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true", UIMin = "0.1", UIMax = "10.0"))
	float DeflectDuration = 2.f;

	/** Deflect Duration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true", UIMin = "0.1", UIMax = "30.0"))
	float DeflectCooldownDuration = 10.f;

	float DeflectTimer = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComp;
};