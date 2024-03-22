#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretSlot.generated.h"

class ATurretBase;
class AArenaGameMode;
class APlayerController;
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

	ETurretType LastSelectedType;
	ATurretBase* CurrentTurret;
	AArenaGameMode* ArenaGameMode;

	// Turret Type Dropdown
	UFUNCTION()
	FORCEINLINE TArray<ETurretType> GetTurretTypeOptions() const
	{
		return { ETurretType::SingleTurret, ETurretType::DualTurret, ETurretType::ThirdTurret };
	}

	// Turret Type Dropdown
	UPROPERTY(EditAnywhere, Category = "Arena", meta = (AllowBlueprintAccess = "true", GetOptions = "GetTurretTypeOptions"))
	ETurretType TurretType = ETurretType::SingleTurret;

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

	// Inform the slots about what type selected!
	UFUNCTION(BlueprintCallable)
	void CardSelectionListener(ETurretType Type);

	// Inform the slot the mouse is over. Spawns preview turret
	UFUNCTION(BlueprintCallable)
	void SlotMouseOver();

	// Inform the slot the mouse is over. Destroys preview turret
	UFUNCTION(BlueprintCallable)
	void SlotMouseLeft();

	// Inform the slot the mouse clicked on it. Destroys preview, spawns actual turret
	UFUNCTION(BlueprintCallable)
	void SlotMouseClicked();

	// Returns the preview class based on the type
	TSubclassOf<ATurretBase> GetPreviewClass(ETurretType Type);

	// Returns the actual class based on the type
	TSubclassOf<ATurretBase> GetActualClass(ETurretType Type);

	// Called on Game Mode Restarts
	UFUNCTION()
	void OnRestart();

	// Called on Game Mode changes GameState
	UFUNCTION()
	void OnGameStateChange(EGameStates NewState);

	// Check if all the components and variables are set and good to go!
	bool AreAllComponentsSet();

};
