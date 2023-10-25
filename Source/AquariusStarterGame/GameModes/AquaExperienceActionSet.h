#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AquaExperienceActionSet.generated.h"

class UGameFeatureAction;
/**
 * Definition of a set of actions to perform as part of entering an experience
 */
UCLASS(BlueprintType, NotBlueprintable)
class AQUARIUSSTARTERGAME_API UAquaExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UAquaExperienceActionSet();

	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	UPROPERTY(EditAnywhere, Instanced, Category = "Actions to Perform")
	TArray<UGameFeatureAction*> Actions;

	// List of Game Feature Plugins this experience wants to have active
	UPROPERTY(EditAnywhere, Category = "Feature Dependencies")
	TArray<FString> GameFeaturesToEnable;
};
