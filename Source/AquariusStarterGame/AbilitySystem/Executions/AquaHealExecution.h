// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AquaHealExecution.generated.h"

/**
 * UAquaHealExecution
 *
 *	Execution used by gameplay effects to apply healing to the health attributes.
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UAquaHealExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
