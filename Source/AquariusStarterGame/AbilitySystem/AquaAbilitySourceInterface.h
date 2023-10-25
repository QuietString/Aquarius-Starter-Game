// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "AquaAbilitySourceInterface.generated.h"

class AActor;
struct FGameplayTagContainer;
class UPhysicalMaterial;

/** Base interface for anything acting as a ability calculation source */
UINTERFACE()
class UAquaAbilitySourceInterface : public UInterface
{
	GENERATED_BODY()
};

class IAquaAbilitySourceInterface
{
	GENERATED_BODY()

public:

	/**
	 * Compute the multiplier for effect falloff with distance
	 *
	 * @param Distance			Distance from source to target for ability calculations (distance bullet traveled for a gun, etc...)
	 * @param SourceTags		Aggregated Tags from the source
	 * @param TargetTags		Aggregated Tags currently on the target
	 *
	 * @return Multiplier to apply to the base attribute value due to distance
	 */
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr
		, const FGameplayTagContainer* TargetTags = nullptr) const = 0;

	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr
		, const FGameplayTagContainer* TargetTags = nullptr) const = 0;
};
