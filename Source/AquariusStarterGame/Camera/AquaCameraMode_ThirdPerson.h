#pragma once

#include "CoreMinimal.h"
#include "Camera/AquaCameraMode.h"
#include "Curves/CurveFloat.h"
#include "AquaCameraMode_ThirdPerson.generated.h"

class UCurveVector;

/**
 * UAquaCameraMode_ThirdPerson
 *
 *	A basic third person camera mode.
 */
UCLASS(Abstract, Blueprintable)
class UAquaCameraMode_ThirdPerson : public UAquaCameraMode
{
	GENERATED_BODY()

public:

	UAquaCameraMode_ThirdPerson();

protected:

	virtual void UpdateView(float DeltaTime) override;

	void UpdateForTarget(float DeltaTime);

	// Curve that defines local-space offsets from the target using the view pitch to evaluate the curve.
	UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "!bUseRuntimeFloatCurves"))
	const UCurveVector* TargetOffsetCurve;

	// Alters the speed that a crouch offset is blended in or out
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Third Person")
	float CrouchOffsetBlendMultiplier = 5.f;

	FVector InitialCrouchOffset = FVector::ZeroVector;
	FVector TargetCrouchOffset = FVector::ZeroVector;
	float CrouchOffsetBlendPct = 1.f;
	FVector CurrentCrouchOffset = FVector::ZeroVector;


	void SetTargetCrouchOffset(FVector NewTargetOffset);
	void UpdateCrouchOffset(float DeltaTime);
};
