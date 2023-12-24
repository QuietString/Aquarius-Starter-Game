#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "AquaWeaponSpawner.generated.h"

class UAquaInventoryItemDefinition;
class UAquaWeaponPickupDefinition;
class UCapsuleComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable, BlueprintType)
class AQUARIUSSTARTERGAME_API AAquaWeaponSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAquaWeaponSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnConstruction(const FTransform& Transform) override;

protected:
	//Data asset used to configure a Weapon Spawner
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Aqua|WeaponPickup")
	UAquaWeaponPickupDefinition* WeaponPickupDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aqua|WeaponPickup")
	bool bIsWeaponAvailable;

	//The amount of time between weapon pickup and weapon spawning in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aqua|WeaponPickup")
	float CoolDownTime;

	//Delay between when the weapon is made available and when we check for a pawn standing in the spawner. Used to give the bIsWeaponAvailable OnRep time to fire and play FX.
	float CheckExistingOverlapDelay;

	//Used to drive weapon respawn time indicators 0-1
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Aqua|WeaponPickup")
	float CoolDownPercentage;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aqua|WeaponPickup")
	UCapsuleComponent* CollisionVolume;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aqua|WeaponPickup")
	UStaticMeshComponent* PadMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Aqua|WeaponPickup")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aqua|WeaponPickup")
	float WeaponMeshRotationSpeed;

	FTimerHandle CoolDownTimerHandle;

	FTimerHandle CheckOverlapDelayTimerHandle;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	//CHeck for pawns standing on pad when the weapon is spawned.
	void CheckForExistingOverlaps();

	UFUNCTION(BlueprintNativeEvent)
	void AttemptPickupWeapon(APawn* Pawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "Aqua|WeaponPickup")
	bool GiveWeapon(TSubclassOf<UAquaInventoryItemDefinition> WeaponItemClass, APawn* ReceivingPawn);

	void StartCoolDown();

	UFUNCTION(BlueprintCallable, Category = "Aqua|WeaponPickup")
	void ResetCoolDown();

	UFUNCTION()
	void OnCoolDownTimerComplete();

	void SetWeaponPickupVisibility(bool bShouldBeVisible);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aqua|WeaponPickup")
	static int32 GetDefaultStatFromItemDef(const TSubclassOf<UAquaInventoryItemDefinition> WeaponItemClass, FGameplayTag StatTag);
};
