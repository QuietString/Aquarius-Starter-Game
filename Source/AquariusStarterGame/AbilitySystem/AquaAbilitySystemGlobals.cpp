// Copyright Epic Games, Inc. All Rights Reserved.


#include "AbilitySystem/AquaAbilitySystemGlobals.h"
#include "AquaGameplayEffectContext.h"

UAquaAbilitySystemGlobals::UAquaAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

FGameplayEffectContext* UAquaAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAquaGameplayEffectContext();
}
