#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "AquaCameraComponent.generated.h"

class UAquaCameraMode;
class UAquaCameraModeStack;
struct FGameplayTag;

DECLARE_DELEGATE_RetVal(TSubclassOf<UAquaCameraMode>, FAquaCameraModeDelegate);


/**
 * UAquaCameraComponent
 *
 *	The base camera component class used by this project.
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:

	UAquaCameraComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the camera component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Aqua|Camera")
	static UAquaCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UAquaCameraComponent>() : nullptr); }

	// Returns the target actor that the camera is looking at.
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	// Delegate used to query for the best camera mode.
	FAquaCameraModeDelegate DetermineCameraModeDelegate;

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;
protected:

	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	virtual void UpdateCameraModes();

	// Stack used to blend the camera modes.
	UPROPERTY()
	UAquaCameraModeStack* CameraModeStack;

	// Offset applied to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	float FieldOfViewOffset;
};
