#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "Abilities/AquaGameplayAbility.h"
#include "AquaAbilitySystemComponent.generated.h"


class UAquaGameplayAbility;
class UAquaAbilityTagRelationshipMapping;
struct FGameplayTag;
struct FGameplayAbilitySpec;

AQUARIUSSTARTERGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);

/**
 * UAquaAbilitySystemComponent
 *
 *	Base ability system component class used by this project.
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	UAquaAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	//typedef TFunctionRef<bool(const UAquaGameplayAbility* AquaAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	bool IsActivationGroupBlocked(EAquaAbilityActivationGroup Group) const;
	void AddAbilityToActivationGroup(EAquaAbilityActivationGroup Group, UAquaGameplayAbility* AquaAbility);
	void RemoveAbilityFromActivationGroup(EAquaAbilityActivationGroup Group, UAquaGameplayAbility* AquaAbility);
	//void CancelActivationGroupAbilities(EAquaAbilityActivationGroup Group, UAquaGameplayAbility* IgnoreAquaAbility, bool bReplicateCancelAbility);


	/** Sets the current tag relationship mapping, if null it will clear it out */
	void SetTagRelationshipMapping(UAquaAbilityTagRelationshipMapping* NewMapping);

protected:

	void TryActivateAbilitiesOnSpawn();

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	//virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	//virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;

	/** Notify client that an ability failed to activate */
	UFUNCTION(Client, Unreliable)
	void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

	// If set, this table is used to look up tag relationships for activate and cancel
	UPROPERTY()
	UAquaAbilityTagRelationshipMapping* TagRelationshipMapping;

	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// Number of abilities running in each activation group.
	int32 ActivationGroupCounts[(uint8)EAquaAbilityActivationGroup::MAX];
};
