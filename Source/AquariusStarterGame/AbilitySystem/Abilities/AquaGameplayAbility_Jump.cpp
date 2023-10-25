// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AquaGameplayAbility_Jump.h"

#include "Character/AquaCharacter.h"

UAquaGameplayAbility_Jump::UAquaGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UAquaGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                                   const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AAquaCharacter* AquaCharacter = Cast<AAquaCharacter>(ActorInfo->AvatarActor.Get());
	if (!AquaCharacter || !AquaCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UAquaGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	// stop jumpoing in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAquaGameplayAbility_Jump::CharacterJumpStart()
{
	if (AAquaCharacter* AquaCharacter = GetAquaCharacterFromActorInfo())
	{
		if (AquaCharacter->IsLocallyControlled() && !GetAquaCharacterFromActorInfo()->bPressedJump)
		{
			AquaCharacter->UnCrouch();
			AquaCharacter->Jump();
		}
	}
}

void UAquaGameplayAbility_Jump::CharacterJumpStop()
{
	if (AAquaCharacter* AquaCharacter = GetAquaCharacterFromActorInfo())
	{
		if (AquaCharacter->IsLocallyControlled() && AquaCharacter->bPressedJump)
		{
			AquaCharacter->StopJumping();
		}
	}
}
