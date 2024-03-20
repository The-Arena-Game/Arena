#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretSlot.generated.h"

UENUM(BlueprintType)
enum class ETurretType : uint8
{
	SingleTurret    UMETA(DisplayName = "Single Turret"),
	DualTurret		UMETA(DisplayName = "Dual Turret"),
	ThirdTurret		UMETA(DisplayName = "Third Turret"),
};

UCLASS()
class ARENA_API ATurretSlot : public AActor
{
	GENERATED_BODY()
	
public:	

	ATurretSlot();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

private:	

	UFUNCTION(BlueprintCallable)
	void CardSelectionListener(ETurretType Type);

};
