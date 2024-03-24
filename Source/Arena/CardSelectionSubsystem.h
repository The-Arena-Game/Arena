#pragma once

#include "CoreMinimal.h"

#include "TurretSlot.h"

#include "CardSelectionSubsystem.generated.h"
#include "Subsystems/GameInstanceSubsystem.h"

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
