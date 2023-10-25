#include "AquaPlayerController.h"

#include "AquaGameplayTags.h"
#include "AquaLogChannels.h"
#include "AquaPlayerState.h"
#include "Camera/AquaPlayerCameraManager.h"
#include "AbilitySystem/AquaAbilitySystemComponent.h"


AAquaPlayerController::AAquaPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AAquaPlayerCameraManager::StaticClass();
}

AAquaPlayerState* AAquaPlayerController::GetAquaPlayerState() const
{
	return CastChecked<AAquaPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UAquaAbilitySystemComponent* AAquaPlayerController::GetAquaAbilitySystemComponent() const
{
	const AAquaPlayerState* AquaPS = GetAquaPlayerState();
	return (AquaPS ? AquaPS->GetAquaAbilitySystemComponent() : nullptr);
}

void AAquaPlayerController::SetIsAutoRunning(const bool bEnabled)
{
	const bool bIsAutoRunning = GetIsAutoRunning();
	if (bEnabled != bIsAutoRunning)
	{
		if (!bEnabled)
		{
			OnEndAutoRun();
		}
		else
		{
			OnStartAutoRun();
		}
	}
}

bool AAquaPlayerController::GetIsAutoRunning() const
{
	bool bIsAutoRunning = false;
	if (const UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent())
	{
		bIsAutoRunning = AquaASC->GetTagCount(FAquaGameplayTags::Get().Status_AutoRunning) > 0;
	}
	return bIsAutoRunning;
}

void AAquaPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent())
	{
		AquaASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AAquaPlayerController::OnStartAutoRun()
{
	
}

void AAquaPlayerController::OnEndAutoRun()
{
}
