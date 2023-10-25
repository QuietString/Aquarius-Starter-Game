#pragma once

#include "CoreMinimal.h"
#include "ModularGameModeBase.h"
#include "AquaGameMode.generated.h"

class UAquaExperienceDefinition;
class UAquaPawnData;


/**
 * AAquaGameMode
 *
 *	The base game mode class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class AQUARIUSSTARTERGAME_API AAquaGameMode : public AModularGameModeBase
{
	GENERATED_BODY()

public:

	AAquaGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UAquaPawnData* GetPawnDataForController(const AController* InController) const;

	//~AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	//virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	//virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual void InitGameState() override;
	//~End of AGameModeBase interface

	UFUNCTION(BlueprintCallable)
	void OnExperienceGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);

protected:

	void OnExperienceLoaded(const UAquaExperienceDefinition* CurrentExperience);
	bool IsExperienceLoaded() const;
};
