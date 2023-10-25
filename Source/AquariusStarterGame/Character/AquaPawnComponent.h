#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "AquaPawnComponent.generated.h"


/**
 * UAquaPawnComponent
 *
 *	An actor component that can be used for adding custom behavior to pawns.
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class AQUARIUSSTARTERGAME_API UAquaPawnComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UAquaPawnComponent(const FObjectInitializer& ObjectInitializer);
};
