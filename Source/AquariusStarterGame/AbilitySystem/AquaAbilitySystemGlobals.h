// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AbilitySystemGlobals.h"
#include "AquaAbilitySystemGlobals.generated.h"

class UAbilitySystemComponent;

UCLASS(Config=Game)
class AQUARIUSSTARTERGAME_API UAquaAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	UAquaAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	//~UAbilitySystemGlobals interface
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	//~End of UAbilitySystemGlobals interface

};
