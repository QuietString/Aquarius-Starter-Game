#pragma once

#include "CoreMinimal.h"
#include "ModularGameState.h"
#include "AbilitySystemInterface.h"
#include "AquaGameState.generated.h"

class UAquaExperienceManagerComponent;
class UAquaAbilitySystemComponent;
class UAbilitySystemComponent;

/**
 * AAquaGameState
 *
 *	The base game state class used by this project.
 */
UCLASS(Config = Game)
class AQUARIUSSTARTERGAME_API AAquaGameState : public AModularGameState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AAquaGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface


private:
	UPROPERTY()
	UAquaExperienceManagerComponent* ExperienceManagerComponent;

	// The ability system component subobject for game-wide things (primarily gameplay cues)
	UPROPERTY(VisibleAnywhere, Category = "Aqua|GameState")
	UAquaAbilitySystemComponent* AbilitySystemComponent;
};
