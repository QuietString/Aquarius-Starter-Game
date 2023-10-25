// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AquaGameplayAbility.h"
#include "AquaGameplayAbility_Jump.generated.h"

/**
 * UAquaGameplayAbility_Jump
 *
 *	Gameplay ability used for character jumping.
 */
UCLASS(Abstract)
class AQUARIUSSTARTERGAME_API UAquaGameplayAbility_Jump : public UAquaGameplayAbility
{
	GENERATED_BODY()

public:

	UAquaGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Aqua|Ability")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Aqua|Ability")
	void CharacterJumpStop();
};
