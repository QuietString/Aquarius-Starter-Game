#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CommonPlayerController.h"
#include "AquaPlayerController.generated.h"


class UAquaPlayerState;
class UAquaAbilitySystemComponent;
class APawn;


/**
 * AAquaPlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class AQUARIUSSTARTERGAME_API AAquaPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:

	AAquaPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Aqua|PlayerController")
	AAquaPlayerState* GetAquaPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Aqua|PlayerController")
	UAquaAbilitySystemComponent* GetAquaAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Aqua|Character")
	void SetIsAutoRunning(const bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Aqua|Character")
	bool GetIsAutoRunning() const;

protected:

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	void OnStartAutoRun();
	void OnEndAutoRun();
};
