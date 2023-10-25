// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectTypes.h"
#include "AquaGameplayEffectContext.generated.h"

class UAbilitySystemComponent;
class IAquaAbilitySourceInterface;
class UPhysicalMaterial;

USTRUCT()
struct FAquaGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	FAquaGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	FAquaGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	/** Returns the wrapped FAquaGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	static AQUARIUSSTARTERGAME_API FAquaGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	/** Sets the object used as the ability source */
	void SetAbilitySource(const IAquaAbilitySourceInterface* InObject, float InSourceLevel);

	/** Returns the ability source interface associated with the source object. Only valid on the authority. */
	const IAquaAbilitySourceInterface* GetAbilitySource() const;

	virtual FGameplayEffectContext* Duplicate() const override
	{
		FAquaGameplayEffectContext* NewContext = new FAquaGameplayEffectContext();
		*NewContext = *this;
		if ((GetHitResult()))
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FAquaGameplayEffectContext::StaticStruct();
	}

	/** Overridden to serialize new fields */
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

	/** Returns the physical material from the hit result if there is one */
	const UPhysicalMaterial* GetPhysicalMaterial() const;


public:
	/** ID to allow the identification of multiple bullets that were part of the same cartridge */
	UPROPERTY()
	int32 CartridgeID = -1;

protected:
	/** Ability Source object (should implement IAquaAbilitySourceInterface). NOT replicated currently */
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};
