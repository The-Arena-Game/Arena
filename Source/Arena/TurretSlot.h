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
	TwinTurret		UMETA(DisplayName = "Twin Turret"),
	Chonky			UMETA(DisplayName = "Chonky Turret"),
	Gatling			UMETA(DisplayName = "Gatling Turret"),
	Triplet			UMETA(DisplayName = "Triplet Turret"),
};

UCLASS()
class ARENA_API ATurretSlot : public AActor
{
	GENERATED_BODY()

public:

	ATurretSlot();

	virtual void Tick(float DeltaTime) override;

	// Spawns turret. Created for editor-time spawning.
	UFUNCTION(BlueprintCallable)
	void SpawnEditorTurret();

protected:

	virtual void BeginPlay() override;

private:

	ETurretType LastSelectedType;
	ATurretBase* CurrentTurret;
	AArenaGameMode* ArenaGameMode;
	bool bTurretInstalled;

	/*----------------------------------------------------------------------------
		Properties
	----------------------------------------------------------------------------*/
	UPROPERTY(EditAnywhere, Category = "Arena", meta = (AllowAbstract = "true", ClampMin = "1", ClampMax = "10"))
	int ActiveLevelNumber = 1;

	// Turret Type Dropdown
	UFUNCTION()
	TArray<ETurretType> GetTurretTypeOptions();

	// Turret Type Dropdown
	UPROPERTY(EditAnywhere, Category = "Arena", meta = (AllowBlueprintAccess = "true", GetOptions = "GetTurretTypeOptions"))
	ETurretType EditorTurretType = ETurretType::SingleTurret;

	/*----------------------------------------------------------------------------
		Components
	----------------------------------------------------------------------------*/
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

	/*----------------------------------------------------------------------------
		Turret Classes
	----------------------------------------------------------------------------*/
	UPROPERTY(EditDefaultsOnly, Category = "Arena")
	TSubclassOf<ATurretBase> SingleTurretClass;

	UPROPERTY(EditDefaultsOnly, Category = "Arena")
	TSubclassOf<ATurretBase> DualTurretClass;

	UPROPERTY(EditDefaultsOnly, Category = "Arena")
	TSubclassOf<ATurretBase> TwinTurretClass;

	UPROPERTY(EditDefaultsOnly, Category = "Arena")
	TSubclassOf<ATurretBase> ChonkyTurretClass;

	UPROPERTY(EditDefaultsOnly, Category = "Arena")
	TSubclassOf<ATurretBase> GatlingTurretClass;

	UPROPERTY(EditDefaultsOnly, Category = "Arena")
	TSubclassOf<ATurretBase> TripletTurretClass;

	UPROPERTY(EditDefaultsOnly, Category = "Arena")
	TSubclassOf<ATurretBase> SingleTurretPreviewClass;

	UPROPERTY(EditDefaultsOnly, Category = "Arena")
	TSubclassOf<ATurretBase> DualTurretPreviewClass;

	/*----------------------------------------------------------------------------
		Turret Placement
	----------------------------------------------------------------------------*/
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

	/*----------------------------------------------------------------------------
		Others
	----------------------------------------------------------------------------*/
	// Returns the preview class based on the type
	UFUNCTION()
	TSubclassOf<ATurretBase> GetPreviewClass(ETurretType Type);

	// Returns the actual class based on the type
	UFUNCTION()
	TSubclassOf<ATurretBase> GetActualClass(ETurretType Type);

	// Called on Game Mode Restarts
	UFUNCTION()
	void OnRestart();

	// Called on Game Mode changes GameState
	UFUNCTION()
	void OnGameStateChange(EGameStates NewState);

	// Check if all the components and variables are set and good to go!
	UFUNCTION()
	bool AreAllComponentsSet();

	// Controls if the current situation is suitable for turret placement action or not
	UFUNCTION()
	bool IsTurretPlacementAvailable();

	// Controls if the current situation is suitable for turret placement action or not
	UFUNCTION()
	bool IsAnyTurretSpawned();

};