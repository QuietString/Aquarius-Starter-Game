// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AquaCameraMode.generated.h"

class UAquaCameraComponent;

/**
 * EAquaCameraModeBlendFunction
 *
 *	Blend function used for transitioning between camera modes.
 */
UENUM()
enum class EAquaCameraModeBlendFunction : uint8
{
	// Does a simple linear interpolation.
	Linear,

	// Immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by the exponent.
	EaseIn,

	// Smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by the exponent.
	EaseOut,

	// Smoothly accelerates and decelerates.  Ease amount controlled by the exponent.
	EaseInOut,

	COUNT	UMETA(Hidden)
};



/**
 * FAquaCameraModeView
 *
 *	View data produced by the camera mode that is used to blend camera modes.
 */
struct FAquaCameraModeView
{
public:

	FAquaCameraModeView();

	void Blend(const FAquaCameraModeView& Other, float OtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};


/**
 * UAquaCameraMode
 *
 *	Base class for all camera modes.
 */
UCLASS(Abstract, NotBlueprintable)
class AQUARIUSSTARTERGAME_API UAquaCameraMode : public UObject
{
	GENERATED_BODY()

public:

	UAquaCameraMode();

	UAquaCameraComponent* GetAquaCameraComponent() const;

	//virtual UWorld* GetWorld() const override;

	AActor* GetTargetActor() const;

	const FAquaCameraModeView& GetCameraModeView() const { return View; };

	void UpdateCameraMode(float DeltaTime);

	float GetBlendTime() const { return BlendTime; }
	float GetBlendWeight() const { return BlendWeight; }
	void SetBlendWeight(float Weight);

	FGameplayTag GetCameraTypeTag() const { return CameraTypeTag; }

protected:

	virtual FVector GetPivotLocation() const;
	virtual FRotator GetPivotRotation() const;

	virtual void UpdateView(float DeltaTime);
	//virtual void UpdateBlending(float DeltaTime);



	// A tag that can be queried by gameplay code that cares when a kind of camera mode is active
	// without having to ask about a specific mode (e.g., when aiming downsights to get more accuracy)
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	FGameplayTag CameraTypeTag;

	// View output produced by the camera mode.
	FAquaCameraModeView View;

	// The horizontal field of view (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	// Minimum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	// Maximum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	// How long it takes to blend in this mode.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	// Function used for blending.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	EAquaCameraModeBlendFunction BlendFunction;

	// Exponent used by blend functions to control the shape of the curve.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	// Linear blend alpha used to determine the blend weight.
	float BlendAlpha;

	// Blend weight calculated using the blend alpha and function.
	float BlendWeight;

protected:
	/** If true, skips all interpolation and puts camera in ideal location.  Automatically set to false next frame. */
	UPROPERTY(transient)
	uint32 bResetInterpolation : 1;
};


/**
*UAquaCameraModeStack
*
* Stack used for blending camera modes.
*/
UCLASS()
class UAquaCameraModeStack : public UObject
{
	GENERATED_BODY()

public:

	UAquaCameraModeStack();

	void ActivateStack();
	void DeactivateStack();

	bool IsStackActivate() const { return bIsActive; }

	void PushCameraMode(TSubclassOf<UAquaCameraMode> CameraModeClass);

	bool EvaluateStack(float DeltaTime, FAquaCameraModeView& OutCameraModeView);

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:

	UAquaCameraMode* GetCameraModeInstance(TSubclassOf<UAquaCameraMode> CameraModeClass);

	void UpdateStack(float DeltaTime);
	void BlendStack(FAquaCameraModeView& OutCameraModeView) const;

	bool bIsActive;

	UPROPERTY()
	TArray<UAquaCameraMode*> CameraModeInstances;

	UPROPERTY()
	TArray<UAquaCameraMode*> CameraModeStack;
};

