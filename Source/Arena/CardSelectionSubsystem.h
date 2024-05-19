#pragma once

#include "CoreMinimal.h"

#include "TurretSlot.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "CardSelectionSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArnCardSelection, Log, All);

USTRUCT(BlueprintType)
struct FTurretObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ETurretType TurretType = ETurretType();

	// TODO: Add TurretImage
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardSelection, ETurretType, Type);

/**
 *
 */
UCLASS()
class UCardSelectionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnCardSelection OnCardSelection;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void BroadcastSelectedCard(ETurretType Type)
	{
		OnCardSelection.Broadcast(Type);
	}
};
