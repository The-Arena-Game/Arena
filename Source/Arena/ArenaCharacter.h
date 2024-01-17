// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ArenaCharacter.generated.h"

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

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns Health from the Health Component **/
	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	/* Triggers a check to Health Component. If the Health is 0 or lower, triggers destruction */
	void CheckHealth();

	UFUNCTION(BlueprintCallable)
	void TestShake();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for sprint input */
	void Sprint();
	void StopSprint();

	/** Called for looking input */
	// Disabled for Top-Down --> void Look(const FInputActionValue& Value);

	/** APawn interface */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** To add mapping context */
	virtual void BeginPlay();

private:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

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

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Sprint Speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 200.f;

	/** Sprint Speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category = "Class Effect")
	TSubclassOf<UCameraShakeBase> TestCameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arena Combat", meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComp;

	void HandleDestruction();
};