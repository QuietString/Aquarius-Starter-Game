#pragma once

#include "CoreMinimal.h"
#include "Character/AquaPawnComponent.h"
#include "Camera/AquaCameraMode.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "InputMappingContext.h"
#include "AquaHeroComponent.generated.h"


class AAquaPlayerState;
class UInputComponent;
struct FInputActionValue;
class UAquaInputConfig;

/**
 * UAquaHeroComponent
 *
 *	A component used to create a player controlled pawns (characters, vehicles, etc..).
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class AQUARIUSSTARTERGAME_API UAquaHeroComponent : public UAquaPawnComponent
{
	GENERATED_BODY()

public:

	UAquaHeroComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "Aqua|Hero")
	static UAquaHeroComponent* FindHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UAquaHeroComponent>() : nullptr); }

	void SetAbilityCameraMode(TSubclassOf<UAquaCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle);
	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

	void AddAdditionalInputConfig(const UAquaInputConfig* InputConfig);
	//void RemoveAdditionalInputConfig(const UAquaInputConfig* InputConfig);

	/** True if this has completed OnPawnReadyToInitialize so is prepared for late-added features */
	//bool HasPawnInitialized() const;

	/** True if this player has sent the BindInputsNow event and is prepared for bindings */
	bool IsReadyToBindInputs() const;

	static const FName NAME_BindInputsNow;

protected:

	virtual void OnRegister() override;

	void OnPawnReadyToInitialize();


	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_Crouch(const FInputActionValue& InputActionValue);
	void Input_AutoRun(const FInputActionValue& InputActionValue);

	TSubclassOf<UAquaCameraMode> DetermineCameraMode() const;

	UPROPERTY(EditAnywhere)
	TArray<UInputMappingContext*> DefaultInputMappingContexts;

	// Camera mode set by an ability.
	TSubclassOf<UAquaCameraMode> AbilityCameraMode;

	// Spec handle for the last ability to set a camera mode.
	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;

	// True when the pawn has fully finished initialization
	bool bPawnHasInitialized;

	// True when player input bindings have been applied, will never be true for non-players
	bool bReadyToBindInputs;
};
