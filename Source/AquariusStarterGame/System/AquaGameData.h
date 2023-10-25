#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AquaGameData.generated.h"

/**
 * UAquaGameData
 *
 *	Non-mutable data asset that contains global game data.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Aqua Game Data", ShortTooltip = "Data asset containing global game data."))
class AQUARIUSSTARTERGAME_API UAquaGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UAquaGameData();

	// Returns the loaded game data.
	static const UAquaGameData& Get();

};
