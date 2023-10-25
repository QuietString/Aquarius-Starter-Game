#include "AquaGameState.h"

#include "AquaExperienceManagerComponent.h"
#include "AbilitySystem/AquaAbilitySystemComponent.h"

AAquaGameState::AAquaGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UAquaAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));

	ExperienceManagerComponent = CreateDefaultSubobject<UAquaExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}

void AAquaGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/ this, /*Avatar=*/ this);
}

UAbilitySystemComponent* AAquaGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
