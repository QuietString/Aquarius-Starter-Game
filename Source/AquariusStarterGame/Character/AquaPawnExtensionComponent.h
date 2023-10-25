#pragma once

#include "CoreMinimal.h"
#include "AquaPawnComponent.h"
#include "AquaPawnExtensionComponent.generated.h"


class UAquaPawnData;
class UAquaAbilitySystemComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAquaDynamicMulticastDelegate);


/**
 * UAquaPawnExtensionComponent
 *
 *	Component used to add functionality to all Pawn classes.
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaPawnExtensionComponent : public UAquaPawnComponent
{
	GENERATED_BODY()

public:

	UAquaPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the pawn extension component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Aqua|Pawn")
	static UAquaPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UAquaPawnExtensionComponent>() : nullptr); }

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UAquaPawnData* InPawnData);

	UFUNCTION(BlueprintPure, Category = "Aqua|Pawn")
	UAquaAbilitySystemComponent* GetAquaAbilitySystemComponent() const { return AbilitySystemComponent; }

	// Should be called by the owning pawn when the input component is setup.
	void SetupPlayerInputComponent();

	// Call this anytime the pawn needs to check if it's ready to be initialized (pawn data assigned, possessed, etc..). 
	bool CheckPawnReadyToInitialize();

	// Should be called by the owning pawn to become the avatar of the ability system.
	void InitializeAbilitySystem(UAquaAbilitySystemComponent* InASC, AActor* InOwnerActor);

	// Should be called by the owning pawn to remove itself as the avatar of the ability system.
	void UninitializeAbilitySystem();

	// Should be called by the owning pawn when the pawn's controller changes.
	void HandleControllerChanged();

	// Register with the OnAbilitySystemInitialized delegate and broadcast if condition is already met.
	void RegisterAndCall_OnAbilitySystemInitialized(FSimpleMulticastDelegate::FDelegate Delegate);

	// Register with the OnPawnReadyToInitialize delegate and broadcast if condition is already met.
	void RegisterAndCall_OnPawnReadyToInitialize(FSimpleMulticastDelegate::FDelegate Delegate);



protected:

	virtual void OnRegister() override;

	// Pawn data used to create the pawn.  Specified from a spawn function or on a placed instance.
	UPROPERTY(EditInstanceOnly, Category = "Aqua|Pawn")
	const UAquaPawnData* PawnData;

	// Delegate fired when pawn has everything needed for initialization.
	FSimpleMulticastDelegate OnPawnReadyToInitialize;

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Pawn Ready To Initialize"))
	FAquaDynamicMulticastDelegate BP_OnPawnReadyToInitialize;

	// Delegate fired when our pawn becomes the ability system's avatar actor
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	// Delegate fired when our pawn is removed as the ability system's avatar actor
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	// Pointer to the ability system component that is cached for convenience.
	UPROPERTY()
	UAquaAbilitySystemComponent* AbilitySystemComponent;

	// True when the pawn has everything needed for initialization.
	int32 bPawnReadyToInitialize : 1;
};
