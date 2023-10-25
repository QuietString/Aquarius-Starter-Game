#pragma once

#include "GameFramework/GameStateBase.h"
#include "ModularGameState.generated.h"


UCLASS(Blueprintable)
class MODULARGAMEPLAYACTORS_API AModularGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	//~ Begin AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor interface
};
