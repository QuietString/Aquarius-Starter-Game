#include "AquaPawnExtensionComponent.h"
#include "AquaLogChannels.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "AquaPawnData.h"
#include "AbilitySystem/AquaAbilitySystemComponent.h"

UAquaPawnExtensionComponent::UAquaPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
	bPawnReadyToInitialize = false;
}

void UAquaPawnExtensionComponent::SetPawnData(const UAquaPawnData* InPawnData)
{
	check(InPawnData);

	bPawnReadyToInitialize = false;

	APawn* Pawn = GetPawnChecked<APawn>();

	if (PawnData)
	{
		UE_LOG(LogAqua, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	CheckPawnReadyToInitialize();
}

void UAquaPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckPawnReadyToInitialize();
}

bool UAquaPawnExtensionComponent::CheckPawnReadyToInitialize()
{
	if (bPawnReadyToInitialize)
	{
		return true;
	}

	// Pawn data is required.
	if (!PawnData)
	{
		return false;
	}

	if (!GetController<AController>())
	{
		return false;
	}

	APawn* Pawn = GetPawnChecked<APawn>();

	if (!Pawn->InputComponent)
	{
		return false;
	}

	// Pawn is ready to initialize.
	bPawnReadyToInitialize = true;
	OnPawnReadyToInitialize.Broadcast();
	BP_OnPawnReadyToInitialize.Broadcast();

	return true;
}

void UAquaPawnExtensionComponent::InitializeAbilitySystem(UAquaAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		// The ability system component hasn't changed.
		return;
	}

	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component.
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogAqua, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	/*if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
	}*/

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	if (ensure(PawnData))
	{
		InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	OnAbilitySystemInitialized.Broadcast();
}

void UAquaPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		AbilitySystemComponent->CancelAbilities(nullptr, nullptr);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UAquaPawnExtensionComponent::HandleControllerChanged()
{
	//if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	//{
	//	ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
	//	if (AbilitySystemComponent->GetOwnerActor() == nullptr)
	//	{
	//		//UninitializeAbilitySystem();
	//	}
	//	else
	//	{
	//		AbilitySystemComponent->RefreshAbilityActorInfo();
	//	}
	//}
	CheckPawnReadyToInitialize();
}

void UAquaPawnExtensionComponent::RegisterAndCall_OnAbilitySystemInitialized(
	FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UAquaPawnExtensionComponent::RegisterAndCall_OnPawnReadyToInitialize(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnPawnReadyToInitialize.IsBoundToObject(Delegate.GetUObject()))
	{
		OnPawnReadyToInitialize.Add(Delegate);
	}

	if (bPawnReadyToInitialize)
	{
		Delegate.Execute();
	}
}

void UAquaPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("AquaPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UAquaPawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one AquaPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));
}
