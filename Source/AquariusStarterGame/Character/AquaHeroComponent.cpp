#include "AquaHeroComponent.h"
#include "AquaLogChannels.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/AquaPlayerController.h"
#include "Player/AquaPlayerState.h"
#include "AquaPawnExtensionComponent.h"
#include "AquaPawnData.h"
#include "AquaCharacter.h"
#include "AbilitySystem/AquaAbilitySystemComponent.h"
#include "Input/AquaInputConfig.h"
#include "Input/AquaInputComponent.h"
#include "Camera/AquaCameraComponent.h"
#include "AquaGameplayTags.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "Components/GameFrameworkComponentManager.h"
#include "System/AquaAssetManager.h"

const FName UAquaHeroComponent::NAME_BindInputsNow("BindInputsNow");

UAquaHeroComponent::UAquaHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityCameraMode = nullptr;
	bPawnHasInitialized = false;
	bReadyToBindInputs = false;
}

void UAquaHeroComponent::SetAbilityCameraMode(TSubclassOf<UAquaCameraMode> CameraMode,
	const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void UAquaHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

void UAquaHeroComponent::AddAdditionalInputConfig(const UAquaInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	UAquaInputComponent* AquaIC = Pawn->FindComponentByClass<UAquaInputComponent>();
	check(AquaIC);

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UAquaPawnExtensionComponent* PawnExtComp = UAquaPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		AquaIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
	}

}

bool UAquaHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UAquaHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (UAquaPawnExtensionComponent* PawnExtComp = UAquaPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnExtComp->RegisterAndCall_OnPawnReadyToInitialize(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnPawnReadyToInitialize));
		}
	}
	else
	{
		UE_LOG(LogAqua, Error, TEXT("[UAquaHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));
	}
}

void UAquaHeroComponent::OnPawnReadyToInitialize()
{
	if (!ensure(!bPawnHasInitialized))
	{
		// Don't initialize twice
		return;
	}

	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	AAquaPlayerState* AquaPS = GetPlayerState<AAquaPlayerState>();
	check(AquaPS);

	const UAquaPawnData* PawnData = nullptr;

	if (UAquaPawnExtensionComponent* PawnExtComp = UAquaPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		PawnData = PawnExtComp->GetPawnData<UAquaPawnData>();

		// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
		// The ability system component and attribute sets live on the player state.
		PawnExtComp->InitializeAbilitySystem(AquaPS->GetAquaAbilitySystemComponent(), AquaPS);
	}

	if (AAquaPlayerController* AquaPC = GetController<AAquaPlayerController>())
	{
		if (Pawn->InputComponent != nullptr)
		{
			InitializePlayerInput(Pawn->InputComponent);
		}
	}

	if (PawnData)
	{
		if (UAquaCameraComponent* CameraComponent = UAquaCameraComponent::FindCameraComponent(Pawn))
		{
			CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		}
	}

	bPawnHasInitialized = true;
}

void UAquaHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* InputSystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSystem);

	InputSystem->ClearAllMappings();

	if (const UAquaPawnExtensionComponent* PawnExtComp = UAquaPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UAquaPawnData* PawnData = PawnExtComp->GetPawnData<UAquaPawnData>())
		{
			if (const UAquaInputConfig* InputConfig = PawnData->InputConfig)
			{
				const FAquaGameplayTags& GameplayTags = FAquaGameplayTags::Get();

				// Register any default input configs with the settings so that they will be applied to the player during AddInputMappings
				for (const UInputMappingContext* Mapping : DefaultInputMappingContexts)
				{
					InputSystem->AddMappingContext(Mapping, 0);
				}

				UAquaInputComponent* AquaIC = CastChecked<UAquaInputComponent>(PlayerInputComponent);

				TArray<uint32> BindHandles;
				AquaIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

				AquaIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
				AquaIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
				AquaIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
				AquaIC->BindNativeAction(InputConfig, GameplayTags.InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);

			}
		}
	}
}

void UAquaHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("ability input tag pressed: %s"), *InputTag.ToString());
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UAquaPawnExtensionComponent* PawnExtComp = UAquaPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UAquaAbilitySystemComponent* AquaASC = PawnExtComp->GetAquaAbilitySystemComponent())
			{
				AquaASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UAquaHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UAquaPawnExtensionComponent* PawnExtComp = UAquaPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UAquaAbilitySystemComponent* AquaASC = PawnExtComp->GetAquaAbilitySystemComponent())
			{
				AquaASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

void UAquaHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	if (AAquaPlayerController* AquaController = Cast<AAquaPlayerController>(Controller))
	{
		AquaController->SetIsAutoRunning(false);
	}

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UAquaHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	if (APawn* Pawn = GetPawn<APawn>())
	{
		const FVector2D Value = InputActionValue.Get<FVector2d>();

		if (Value.X != 0.0f)
		{
			Pawn->AddControllerYawInput(Value.X);
		}

		if (Value.Y != 0.0f)
		{
			Pawn->AddControllerPitchInput(-Value.Y);
		}
	}
}

void UAquaHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	if (AAquaCharacter* Character = GetPawn<AAquaCharacter>())
	{
		Character->ToggleCrouch();
	}
}

void UAquaHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)
{
	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (AAquaPlayerController* Controller = Cast<AAquaPlayerController>(Pawn->GetController()))
		{
			// Toggle auto running
			Controller->SetIsAutoRunning(!Controller->GetIsAutoRunning());
		}
	}
}

TSubclassOf<UAquaCameraMode> UAquaHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UAquaPawnExtensionComponent* PawnExtComp = UAquaPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UAquaPawnData* PawnData = PawnExtComp->GetPawnData<UAquaPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

