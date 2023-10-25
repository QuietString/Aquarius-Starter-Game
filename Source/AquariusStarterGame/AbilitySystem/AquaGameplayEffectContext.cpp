// Copyright Epic Games, Inc. All Rights Reserved.


#include "AbilitySystem/AquaGameplayEffectContext.h"

#include "AquaAbilitySourceInterface.h"

FAquaGameplayEffectContext* FAquaGameplayEffectContext::ExtractEffectContext(FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FAquaGameplayEffectContext::StaticStruct()))
	{
		return (FAquaGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

void FAquaGameplayEffectContext::SetAbilitySource(const IAquaAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	// SourceLevel = InSourceLevel;
}

const IAquaAbilitySourceInterface* FAquaGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IAquaAbilitySourceInterface>(AbilitySourceObject.Get());
}

bool FAquaGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// Cartridge ID

	return true;
}

const UPhysicalMaterial* FAquaGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}
