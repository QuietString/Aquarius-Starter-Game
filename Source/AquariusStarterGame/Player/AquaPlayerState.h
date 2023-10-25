#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerState.h"
#include "AbilitySystemInterface.h"
#include "AquaPlayerState.generated.h"


class AAquaPlayerController;
class UAquaAbilitySystemComponent;
class UAbilitySystemComponent;
class UAquaPawnData;
class UAquaExperienceDefinition;
/**
 * 
 */
UCLASS()
class AQUARIUSSTARTERGAME_API AAquaPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()


public:

	AAquaPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Aqua|PlayerState")
	AAquaPlayerController* GetAquaPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Aqua|PlayerState")
	UAquaAbilitySystemComponent* GetAquaAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UAquaPawnData* InPawnData);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface


	static const FName NAME_AquaAbilityReady;

private:

	void OnExperienceLoaded(const UAquaExperienceDefinition* CurrentExperience);

protected:

	UPROPERTY()
	const UAquaPawnData* PawnData;

private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Aqua|PlayerState")
	UAquaAbilitySystemComponent* AbilitySystemComponent;


};
