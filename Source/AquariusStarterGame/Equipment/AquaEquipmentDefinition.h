#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AquaEquipmentDefinition.generated.h"

class UAquaAbilitySet;
class UAquaEquipmentInstance;

USTRUCT()
struct FAquaEquipmentActorToSpawn
{
	GENERATED_BODY()

	FAquaEquipmentActorToSpawn(){}

	UPROPERTY(EditAnywhere, Category = Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FTransform AttachTransform;
};


/**
 * AquaEquipmentDefinition
 *
 * Definition of a piece of equipment that can be applied to a pawn
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UAquaEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Class to spawn
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TSubclassOf<UAquaEquipmentInstance> InstanceType;

	// Gameplay ability sets to grant when this is equipped
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<TObjectPtr<const UAquaAbilitySet>> AbilitySetsToGrant;

	// Actors to spawn on the pawn when this is equipped
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<FAquaEquipmentActorToSpawn> ActorsToSpawn;
};
