#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameFeaturePluginOperationResult.h"
#include "AquaExperienceManagerComponent.generated.h"

class UAquaExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAquaExperienceLoaded, const UAquaExperienceDefinition* /*Experience*/);

enum class EAquaExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	LoadingChaosTestingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};

UCLASS()
class AQUARIUSSTARTERGAME_API UAquaExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	UFUNCTION(BlueprintCallable)
	void DeactivateExperience();

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnExperienceLoaded(FOnAquaExperienceLoaded::FDelegate&& Delegate);


	// This returns the current experience if it is fully loaded, asserting otherwise
	// (i.e., if you called it too soon)
	const UAquaExperienceDefinition* GetCurrentExperienceChecked() const;

	// Returns true if the experience is fully loaded
	bool IsExperienceLoaded() const;
private:

	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();

	void OnActionDeactivationCompleted();
	void OnAllActionDeactivated();

	UPROPERTY()
	const UAquaExperienceDefinition* CurrentExperience;

	EAquaExperienceLoadState LoadState = EAquaExperienceLoadState::Unloaded;

	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;

	/** Delegate called when the experience has finished loading */
	FOnAquaExperienceLoaded OnExperienceLoaded;
};
