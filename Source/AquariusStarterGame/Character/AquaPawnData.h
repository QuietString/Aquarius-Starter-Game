#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AquaPawnData.generated.h"

class APawn;
class UAquaAbilitySet;
class UAquaInputConfig;
class UAquaAbilityTagRelationshipMapping;
class UAquaCameraMode;


/**
 * UAquaPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UAquaPawnData(const FObjectInitializer& ObjectInitializer);

	// Class to instantiate for this pawn (should usually derive from AAquaPawn or AAquaCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aqua|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aqua|Abilities")
	TArray<UAquaAbilitySet*> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aqua|Abilities")
	UAquaAbilityTagRelationshipMapping* TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aqua|Input")
	UAquaInputConfig* InputConfig;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aqua|Camera")
	TSubclassOf<UAquaCameraMode> DefaultCameraMode;
};
