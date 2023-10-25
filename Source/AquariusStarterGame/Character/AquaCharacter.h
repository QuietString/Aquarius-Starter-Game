#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "AquaCharacter.generated.h"

class AAquaPlayerController;
class AAquaPlayerState;
class UAbilitySystemComponent;
class UAquaAbilitySystemComponent;
class UAquaPawnExtensionComponent;
class UAquaCameraComponent;


/**
 * FAquaReplicatedAcceleration: Compressed representation of acceleration
 */
USTRUCT()
struct FAquaReplicatedAcceleration
{
	GENERATED_BODY()

		UPROPERTY()
		uint8 AccelXYRadians = 0;	// Direction of XY accel component, quantized to represent [0, 2*pi]

	UPROPERTY()
		uint8 AccelXYMagnitude = 0;	//Accel rate of XY component, quantized to represent [0, MaxAcceleration]

	UPROPERTY()
		int8 AccelZ = 0;	// Raw Z accel rate component, quantized to represent [-MaxAcceleration, MaxAcceleration]
};


/**
 * AAquaCharacter
 *
 *	The base character pawn class used by this project.
 *	Responsible for sending events to pawn components.
 *	New behavior should be added via pawn components when possible.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class AQUARIUSSTARTERGAME_API AAquaCharacter
	: public AModularCharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:

	AAquaCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Aqua|Character")
	AAquaPlayerController* GetAquaPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Aqua|Character")
	AAquaPlayerState* GetAquaPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Aqua|Character")
	UAquaAbilitySystemComponent* GetAquaAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	void ToggleCrouch();

	//~AActor interface
	virtual void PreInitializeComponents() override;
	//~End of AActor interface

protected:

	virtual void OnAbilitySystemInitialized();

	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aqua|Character", Meta = (AllowPrivateAccess = "true"))
	UAquaPawnExtensionComponent* PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aqua|Character", Meta = (AllowPrivateAccess = true))
	UAquaCameraComponent* CameraComponent;

};
