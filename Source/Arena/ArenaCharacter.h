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
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogArena, Log, All);

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

	UFUNCTION(BlueprintCallable)
	void TestShake();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for sprint input */
	void Sprint();
	void StopSprint();

	/** Called for looking input */
	// // Disabled for Top-Down --> void Look(const FInputActionValue& Value);

	/** APawn interface */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** To add mapping context */
	virtual void BeginPlay();

private:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Sprint Speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 200.f;

	/** Sprint Speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena - Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category = "Arena - Combat")
	TSubclassOf<UCameraShakeBase> TestCameraShakeClass;
};