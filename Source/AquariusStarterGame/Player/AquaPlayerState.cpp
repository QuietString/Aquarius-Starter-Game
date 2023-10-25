#include "AquaPlayerState.h"
#include "AquaLogChannels.h"
#include "Net/UnrealNetwork.h"
#include "Character/AquaPawnExtensionComponent.h"
#include "Character/AquaPawnData.h"
#include "AbilitySystem/AquaAbilitySystemComponent.h"
#include "AbilitySystem/AquaAbilitySet.h"
#include "AbilitySystem/Attributes/AquaHealthSet.h"
#include "AbilitySystem/Attributes/AquaCombatSet.h"
#include "Components/GameFrameworkComponentManager.h"
#include "AquaPlayerController.h"
#include "GameModes/AquaExperienceDefinition.h"
#include "GameModes/AquaExperienceManagerComponent.h"
#include "GameModes/AquaGameMode.h"

const FName AAquaPlayerState::NAME_AquaAbilityReady("AquaAbilitiesReady");

AAquaPlayerState::AAquaPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UAquaAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CreateDefaultSubobject<UAquaHealthSet>(TEXT("HealthSet"));
	CreateDefaultSubobject<UAquaCombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;

}

AAquaPlayerController* AAquaPlayerState::GetAquaPlayerController() const
{
	return Cast<AAquaPlayerController>(GetOwner());


}

UAbilitySystemComponent* AAquaPlayerState::GetAbilitySystemComponent() const
{
	return GetAquaAbilitySystemComponent();
}

void AAquaPlayerState::SetPawnData(const UAquaPawnData* InPawnData)
{
	check(InPawnData);

	if (PawnData)
	{
		UE_LOG(LogAqua, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	for (const UAquaAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		UE_LOG(LogTemp, Warning, TEXT("loading ability: %s"), *AbilitySet->GetName());
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_AquaAbilityReady);
}

void AAquaPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AAquaPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);
	UAquaExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UAquaExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnAquaExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AAquaPlayerState::OnExperienceLoaded(const UAquaExperienceDefinition* /*CurrentExperience*/)
{
	if (AAquaGameMode* AquaGameMode = GetWorld()->GetAuthGameMode<AAquaGameMode>())
	{
		if (const UAquaPawnData* NewPawnData = AquaGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
			UE_LOG(LogAqua, Warning, TEXT("pawn data set as %s"), *NewPawnData->GetName());
		}
		else
		{
			UE_LOG(LogAqua, Error, TEXT("AAquaPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}
