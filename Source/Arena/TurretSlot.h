#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretSlot.generated.h"

class ATurretBase;
class UBoxComponent;
class URectLightComponent;

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

	ATurretBase* CurrentTurret;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UBoxComponent* BoxComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	USceneComponent* LightsParent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	TArray<URectLightComponent*> Lights;

	UPROPERTY(VisibleDefaultsOnly, Category = "Class Component")
	USceneComponent* TurretSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "Arena")
	TSubclassOf<ATurretBase> SingleTurretClass;

	UPROPERTY(EditAnywhere, Category = "Arena")
	TSubclassOf<ATurretBase> SingleTurretPreviewClass;

	UPROPERTY(EditAnywhere, Category = "Arena")
	TSubclassOf<ATurretBase> DualTurretClass;

	UPROPERTY(EditAnywhere, Category = "Arena")
	TSubclassOf<ATurretBase> DualTurretPreviewClass;

	UFUNCTION(BlueprintCallable)
	void CardSelectionListener(ETurretType Type);

};
