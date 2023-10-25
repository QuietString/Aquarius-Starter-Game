#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AquaExperienceDefinition.generated.h"

class UGameFeatureAction;
class UAquaPawnData;
class UAquaExperienceActionSet;

/**
 * Definition of an experience
 */
UCLASS(BlueprintType, Const)
class AQUARIUSSTARTERGAME_API UAquaExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UAquaExperienceDefinition();

	// List of Game Feature Plugins this experience wants to have active
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<FString> GameFeatureToEnable;

	/** The default pawn class to spawn for players */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TObjectPtr<const UAquaPawnData> DefaultPawnData;

	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	TArray<UGameFeatureAction*> Actions;

	// List of additional action sets to compose into this experience
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<TObjectPtr<UAquaExperienceActionSet>> ActionSets;
};
