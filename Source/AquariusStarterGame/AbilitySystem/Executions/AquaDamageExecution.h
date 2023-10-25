// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AquaDamageExecution.generated.h"

/**
 * UAquaDamageExecution
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UAquaDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
