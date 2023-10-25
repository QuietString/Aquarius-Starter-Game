#include "AquaGameMode.h"

#include "AquaExperienceDefinition.h"
#include "AquaExperienceManagerComponent.h"
#include "AquaGameState.h"
#include "AquaLogChannels.h"
#include "Character/AquaCharacter.h"
#include "Player/AquaPlayerState.h"
#include "Character/AquaPawnData.h"
#include "Character/AquaPawnExtensionComponent.h"
#include "Player/AquaPlayerController.h"
#include "System/AquaAssetManager.h"

class UAquaAssetManager;

AAquaGameMode::AAquaGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AAquaGameState::StaticClass();
	PlayerControllerClass = AAquaPlayerController::StaticClass();
	PlayerStateClass = AAquaPlayerState::StaticClass();
	DefaultPawnClass = AAquaCharacter::StaticClass();
}

const UAquaPawnData* AAquaGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (InController != nullptr)
	{
		if (const AAquaPlayerState* AquaPS = InController->GetPlayerState<AAquaPlayerState>())
		{
			if (const UAquaPawnData* PawnData = AquaPS->GetPawnData<UAquaPawnData>())
			{
				return PawnData;
			}
		}
	}

	// If not, fall back to the the default for the current experience
	check(GameState);
	UAquaExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UAquaExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		const UAquaExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr)
		{
			UE_LOG(LogAqua, Warning, TEXT("Getting default pawn data from experience"));
			return Experience->DefaultPawnData;
		}

		return UAquaAssetManager::Get().GetDefaultPawnData();
	}
	return nullptr;
}

void AAquaGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FPrimaryAssetId ExperienceId = FPrimaryAssetId(FPrimaryAssetType("AquaExperienceDefinition"), FName("BP_AquaShooterCore"));
	FString ExperienceIdSource = TEXT("Default");

	GetWorld()->GetTimerManager().SetTimerForNextTick([this, ExperienceId, ExperienceIdSource]()
	{
		OnExperienceGiven(ExperienceId, ExperienceIdSource);
	});
}

UClass* AAquaGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UAquaPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AAquaGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
                                                                 const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UAquaPawnExtensionComponent* PawnExtComp = UAquaPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UAquaPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogAqua, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		{
			UE_LOG(LogAqua, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
		}
	}

	return nullptr;
}

bool AAquaGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	// We never want to use the start spot, always use the spawn management component.
	return false;
}

void AAquaGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Delay starting new players until the experience has been loaded
	// (players who log in prior to that will be started by OnExperienceLoaded)
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

void AAquaGameMode::InitGameState()
{
	Super::InitGameState();

	// Listen for the experience load to complete	
	UAquaExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UAquaExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnAquaExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AAquaGameMode::OnExperienceLoaded(const UAquaExperienceDefinition* CurrentExperience)
{
	// Spawn any players that are already attached
	// GetDefaultPawnClassForController_Implementation might only be getting called for players anyways
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

bool AAquaGameMode::IsExperienceLoaded() const
{
	check(GameState);
	UAquaExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UAquaExperienceManagerComponent>();
	check(ExperienceComponent);

	return ExperienceComponent->IsExperienceLoaded();
}

void AAquaGameMode::OnExperienceGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogAquaExperience, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		UAquaExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UAquaExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->SetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(LogAquaExperience, Error, TEXT("Failed to identify experience"));
	}
}
