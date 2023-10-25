#include "AquaAnimInstance.h"
#include "Character/AquaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/AquaCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AnimationStateMachineLibrary.h"
#include "KismetAnimationLibrary.h"

UAquaAnimInstance::UAquaAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAquaAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);
}

float UAquaAnimInstance::GetGroundDistance() const
{
	return GroundDistance;
}

void UAquaAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UAquaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const AAquaCharacter* Character = Cast<AAquaCharacter>(GetOwningActor());
	if (!Character)
	{
		return;
	}

	UAquaCharacterMovementComponent* CharMoveComp = CastChecked<UAquaCharacterMovementComponent>(Character->GetCharacterMovement());
	const FAquaCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
}

void UAquaAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!TryGetPawnOwner())
	{
		return;
	}

	UpdateLocationData(DeltaSeconds);
	UpdateRotationData(DeltaSeconds);

	// ignore first tick
	if (IsFirstUpdate)
	{
		DisplacementSinceLastUpdate = 0.f;
		DisplacementSpeed = 0.f;
		YawDeltaSinceLastUpdate = 0.f;
		AdditiveLeanAngle = 0.f;
	}

	UpdateVelocityData();
	UpdateAccelerationData();
	UpdateWallDetectionHeuristic();
	UpdateCharacterStateData(DeltaSeconds);
	UpdateBlendWeightData(DeltaSeconds);
	UpdateRootYawOffset(DeltaSeconds);
	UpdateAimingData();
	UpdateJumpFallData();

	DisableControlRig = GetCurveValue(FName("DisableLegIK")) > 0.f;

	IsFirstUpdate = false;
}

UCharacterMovementComponent* UAquaAnimInstance::GetMovementComponent()
{
	if (APawn* Pawn = TryGetPawnOwner())
	{
		if (UCharacterMovementComponent* CharMoveComp = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent()))
		{
			return CharMoveComp;
		}
	}

	return nullptr;
}

void UAquaAnimInstance::UpdateLocationData(const float DeltaSeconds)
{
	const FVector PositionDiff = GetOwningActor()->GetActorLocation() - WorldLocation;
	DisplacementSinceLastUpdate = UKismetMathLibrary::VSizeXY(PositionDiff);

	WorldLocation = GetOwningActor()->GetActorLocation();

	DisplacementSpeed = UKismetMathLibrary::SafeDivide(DisplacementSinceLastUpdate, DeltaSeconds);
}

void UAquaAnimInstance::UpdateRotationData(const float DeltaSeconds)
{
	const FRotator RotationDiff = GetOwningActor()->GetActorRotation() - WorldRotation;
	YawDeltaSinceLastUpdate = RotationDiff.Yaw;

	YawDeltaSpeed = UKismetMathLibrary::SafeDivide(YawDeltaSinceLastUpdate, DeltaSeconds);
	WorldRotation = GetOwningActor()->GetActorRotation();

	const float LeanWeight = (IsCrouching || GameplayTag_IsADS) ? 0.025f : 0.0375;
	AdditiveLeanAngle = YawDeltaSpeed * LeanWeight;

}

void UAquaAnimInstance::UpdateVelocityData()
{
	const bool WasMovingLastUpdate = (LocalVelocity2D.IsZero()) ? false : true;

	WorldVelocity = TryGetPawnOwner()->GetVelocity();
	const FVector WorldVelocity2D = FVector(WorldVelocity.X, WorldVelocity.Y, 0.f);

	LocalVelocity2D = WorldRotation.UnrotateVector(WorldVelocity2D);

	LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(WorldVelocity2D, WorldRotation);

	LocalVelocityDirectionAngleWithOffset = LocalVelocityDirectionAngle - RootYawOffset;

	LocalVelocityDirection = SelectCardinalDirectionFromAngle(
		LocalVelocityDirectionAngleWithOffset, CardinalDirectionDeadZone, LocalVelocityDirection, WasMovingLastUpdate);

	const float VelocitySquared = UKismetMathLibrary::VSizeXYSquared(LocalVelocity2D);
	HasVelocity = UKismetMathLibrary::NearlyEqual_FloatFloat(VelocitySquared, 0.f, 0.000001) ? false : true;
}

EAnimEnum_CardinalDirection UAquaAnimInstance::GetOppositeCardinalDirection(
	const EAnimEnum_CardinalDirection CurrentDirection)
{
	switch (CurrentDirection)
	{
	case EAnimEnum_CardinalDirection::Front:
		return EAnimEnum_CardinalDirection::Backward;

	case EAnimEnum_CardinalDirection::Backward:
		return EAnimEnum_CardinalDirection::Front;

	case EAnimEnum_CardinalDirection::Left:
		return EAnimEnum_CardinalDirection::Right;

	case EAnimEnum_CardinalDirection::Right:
		return EAnimEnum_CardinalDirection::Left;

	default: return CurrentDirection;
	}
}

void UAquaAnimInstance::UpdateAccelerationData()
{
	FVector WorldAcceleration2D = GetMovementComponent()->GetCurrentAcceleration();
	WorldAcceleration2D.Z = 0;

	LocalAcceleration2D = WorldRotation.UnrotateVector(WorldAcceleration2D);

	const float AccelerationSquared = UKismetMathLibrary::VSizeXYSquared(LocalAcceleration2D);
	HasAcceleration = UKismetMathLibrary::NearlyEqual_FloatFloat(AccelerationSquared, 0.0, 0.000001) ? false : true;

	PivotDirection2D = UKismetMathLibrary::VLerp(PivotDirection2D, WorldAcceleration2D.GetSafeNormal(0.0001), 0.5);
	PivotDirection2D.Normalize(0.0001);

	const float AccelerationDirection = UKismetAnimationLibrary::CalculateDirection(PivotDirection2D, WorldRotation);

	const EAnimEnum_CardinalDirection CardinalDirection = SelectCardinalDirectionFromAngle(
		AccelerationDirection, CardinalDirectionDeadZone, EAnimEnum_CardinalDirection::Front, false);

	CardinalDirectionFromAcceleration = GetOppositeCardinalDirection(CardinalDirection);
}

void UAquaAnimInstance::UpdateCharacterStateData(const float DeltaSeconds)
{
	IsOnGround = GetMovementComponent()->IsMovingOnGround();

	const bool WasCrouchingLastUpdate = IsCrouching;
	IsCrouching = GetMovementComponent()->IsCrouching();
	CrouchStateChange = (IsCrouching != WasCrouchingLastUpdate) ? true : false;

	ADSStateChanged = (GameplayTag_IsADS != WasADSLastUpdate) ? true : false;
	WasADSLastUpdate = GameplayTag_IsADS;

	TimeSinceFiredWeapon = GameplayTag_IsFiring ? 0.f : TimeSinceFiredWeapon + DeltaSeconds;

	IsJumping = false;
	IsFalling = false;
	if (GetMovementComponent()->MovementMode == MOVE_Falling)
	{
		WorldVelocity.Z > 0.f ? IsJumping = true : IsFalling = true;
	}
}

void UAquaAnimInstance::UpdateBlendWeightData(float DeltaSeconds)
{
	const float NewAdditiveWeight = UKismetMathLibrary::FInterpTo(UpperbodyDynamicAdditiveWeight, 0.f, DeltaSeconds, 6.f);

	UpperbodyDynamicAdditiveWeight = IsAnyMontagePlaying() && IsOnGround ? 1.f : NewAdditiveWeight;

}

void UAquaAnimInstance::ProcessTurnYawCurve()
{
	float PreviousTurnYawCurveValue = TurnYawCurveValue;

	const float TurnYawWeight = GetCurveValue(FName("TurnYawWeight"));
	const bool IsWeightSmall = UKismetMathLibrary::NearlyEqual_FloatFloat(TurnYawWeight, 0.f, 0.0001);

	if (IsWeightSmall)
	{
		TurnYawCurveValue = 0.f;
		PreviousTurnYawCurveValue = 0.f;
	}
	else
	{
		const float RemainingTurnYaw = GetCurveValue(FName("RemainingTurnYaw"));
		TurnYawCurveValue = RemainingTurnYaw / TurnYawWeight;

		if (PreviousTurnYawCurveValue != 0.f)
		{
			const float NewRootYawOffset = RootYawOffset - (TurnYawCurveValue - PreviousTurnYawCurveValue);
			SetRootYawOffset(NewRootYawOffset);
		}
	}
}

void UAquaAnimInstance::SetRootYawOffset(const float InRootYawOffset)
{
	const float RootYawOffsetNormalized = UKismetMathLibrary::NormalizeAxis(InRootYawOffset);
	const FVector2D OffsetClamp = (IsCrouching) ? RootYawOffsetAngleClampCrouched : RootYawOffsetAngleClamp;

	const float ClampedOffset = UKismetMathLibrary::ClampAngle(RootYawOffsetNormalized, OffsetClamp.X, OffsetClamp.Y);
	RootYawOffset = (OffsetClamp.X == OffsetClamp.Y) ? RootYawOffsetNormalized : ClampedOffset;

	//UE_LOG(LogTemp, Warning, TEXT("RootYawOffset: %f"), RootYawOffset);

	AimYaw = -RootYawOffset;
}

void UAquaAnimInstance::UpdateRootYawOffset(const float DeltaSeconds)
{
	if (RootYawOffsetMode == EAnimEnum_RootYawOffsetMode::Accumulate)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Accumulate, RootYawOffset: %f, SinceLast: %f"), RootYawOffset, YawDeltaSinceLastUpdate);
		SetRootYawOffset(RootYawOffset - YawDeltaSinceLastUpdate);
	}

	if (RootYawOffsetMode == EAnimEnum_RootYawOffsetMode::BlendOut || GameplayTag_IsDashing)
	{
		const float OffsetSpringInterp = UKismetMathLibrary::FloatSpringInterp(
			RootYawOffset, 0.f, RootYawOffsetSpringState, 80.f,
			1.f, DeltaSeconds, 1.f, 0.5f);

		SetRootYawOffset(OffsetSpringInterp);
	}

	RootYawOffsetMode = EAnimEnum_RootYawOffsetMode::BlendOut;
}

bool UAquaAnimInstance::IsMovingPerpendicularToInitialPivot() const
{
	const bool IsPivotHorizontal = (PivotInitialDirection == EAnimEnum_CardinalDirection::Front ||
		PivotInitialDirection == EAnimEnum_CardinalDirection::Backward);

	const bool IsVelocityHorizontal = (LocalVelocityDirection == EAnimEnum_CardinalDirection::Front ||
		LocalVelocityDirection == EAnimEnum_CardinalDirection::Backward);

	const bool IsPivotVertical = (PivotInitialDirection == EAnimEnum_CardinalDirection::Right ||
		PivotInitialDirection == EAnimEnum_CardinalDirection::Left);

	const bool IsVelocityVertical = (LocalVelocityDirection == EAnimEnum_CardinalDirection::Right ||
		LocalVelocityDirection == EAnimEnum_CardinalDirection::Left);

	const bool IsPerpendicularToHorizontal = IsPivotHorizontal && !IsVelocityHorizontal;
	const bool IsPerpendicularToVertical = IsPivotVertical && !IsVelocityVertical;

	return IsPerpendicularToHorizontal || IsPerpendicularToVertical;
}

void UAquaAnimInstance::UpdateAimingData()
{
	AimPitch = UKismetMathLibrary::NormalizeAxis(TryGetPawnOwner()->GetBaseAimRotation().Pitch);
}

void UAquaAnimInstance::UpdateIdleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FAnimationStateResultReference AnimationStateResultReference;
	EAnimNodeReferenceConversionResult Result;
	UAnimationStateMachineLibrary::ConvertToAnimationStateResult(Node, AnimationStateResultReference, Result);
	bool IsStateBlendingOut = UAnimationStateMachineLibrary::IsStateBlendingOut(Context, AnimationStateResultReference);

	if (IsStateBlendingOut)
	{
		TurnYawCurveValue = 0.f;
	}
	else
	{
		RootYawOffsetMode = EAnimEnum_RootYawOffsetMode::Accumulate;
		ProcessTurnYawCurve();
	}
}

void UAquaAnimInstance::SetUpStartState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	StartDirection = LocalVelocityDirection;
}

void UAquaAnimInstance::UpdateStartState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FAnimationStateResultReference AnimationStateResultReference;
	EAnimNodeReferenceConversionResult Result;
	UAnimationStateMachineLibrary::ConvertToAnimationStateResult(Node, AnimationStateResultReference, Result);
	const bool IsBlendingOut = UAnimationStateMachineLibrary::IsStateBlendingOut(Context, AnimationStateResultReference);

	if (!IsBlendingOut) RootYawOffsetMode = EAnimEnum_RootYawOffsetMode::Hold;
}

void UAquaAnimInstance::UpdateStopState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FAnimationStateResultReference AnimationStateResultReference;
	EAnimNodeReferenceConversionResult Result;
	UAnimationStateMachineLibrary::ConvertToAnimationStateResult(Node, AnimationStateResultReference, Result);
	const bool IsBlendingOut = UAnimationStateMachineLibrary::IsStateBlendingOut(Context, AnimationStateResultReference);

	if (!IsBlendingOut) RootYawOffsetMode = EAnimEnum_RootYawOffsetMode::Accumulate;
}

void UAquaAnimInstance::SetupPivotState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	PivotInitialDirection = LocalVelocityDirection;
}

void UAquaAnimInstance::UpdatePivotState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if (LastPivotTime > 0.f)
	{
		LastPivotTime = LastPivotTime - Context.GetContext()->GetDeltaTime();
	}
}

void UAquaAnimInstance::UpdateLocomotionStateMachine(const FAnimUpdateContext& Context,
	const FAnimNodeReference& Node)
{
	UAnimInstance* LinkedAnimLayer = GetLinkedAnimGraphInstanceByTag(FName("StartLayerNode"));

	if (!IsFirstUpdate)
	{
		LinkedLayerChanged = UKismetMathLibrary::NotEqual_ObjectObject(LinkedAnimLayer, LastLinkedLayer);
	}

	LastLinkedLayer = LinkedAnimLayer;
}

bool UAquaAnimInstance::IdleToStart() const
{
	return HasAcceleration || (GameplayTag_IsMelee && HasVelocity);
}

bool UAquaAnimInstance::StartToCycle3()
{
	const FName StateMachineName = TEXT("LocomotionSM");
	const int32 MachineIndex = GetStateMachineIndex(StateMachineName);

	const bool Condition1 = StartDirection != LocalVelocityDirection;
	const bool Condition2 = CrouchStateChange;
	const bool Condition3 = ADSStateChanged;
	const bool Condition4 =( GetInstanceCurrentStateElapsedTime(MachineIndex) > 0.15f) && (DisplacementSpeed < 10.f);

	return (Condition1 || Condition2 || Condition3 || Condition4);
}

bool UAquaAnimInstance::StartToCycle() const
{
	// Transition to Cycle early if the offset is too large.
	// E.g. if the camera is on the right side of the character and the user presses forward,
	// we don't want to strafe left for a long period of time, we want to jog forward.
	// This transition uses the SyncGroupNameToRequireValidMarkersRule setting
	// to ensure we wait until there are valid sync markers so we smoothly enter Cycle.
	return FMath::Abs(RootYawOffset) > 60.f;
}

bool UAquaAnimInstance::StopToIdle() const
{
	return CrouchStateChange || ADSStateChanged;
}

bool UAquaAnimInstance::StopRule() const
{
	return !(HasAcceleration || (GameplayTag_IsMelee && HasVelocity));
}

bool UAquaAnimInstance::PivotToCycle() const
{
	return (CrouchStateChange || ADSStateChanged || (IsMovingPerpendicularToInitialPivot() && LastPivotTime <= 0.f));
}

bool UAquaAnimInstance::PivotSourcesToPivot() const
{
	// Check if velocity (where we're moving) is opposite to acceleration (where we want to be moving).
	return ((LocalVelocity2D.Dot(LocalAcceleration2D) < 0.f) && !IsRunningIntoWall);
}

bool UAquaAnimInstance::JumpStartLoopToJumpApex() const
{
	return TimeToJumpApex < 0.4f;
}

bool UAquaAnimInstance::FallLoopToFallLand() const
{
	return GroundDistance < 200.f;
}

void UAquaAnimInstance::UpdateWallDetectionHeuristic()
{
	bool IsAccelerationLargeEnough = (UKismetMathLibrary::VSizeXY(LocalAcceleration2D) > 0.1f) ? true : false;
	bool IsVelocitySmall = (UKismetMathLibrary::VSizeXY(LocalVelocity2D) < 200.f) ? true : false;
	bool IsSameDirection;

	FVector AccelerationNorm = LocalAcceleration2D.GetSafeNormal(0.0001);
	FVector VelocityNorm = LocalVelocity2D.GetSafeNormal(0.0001);
	float AngleDiff = FVector::DotProduct(AccelerationNorm, VelocityNorm);

	IsSameDirection = UKismetMathLibrary::InRange_FloatFloat(AngleDiff, -0.6f, 0.6f, true, true);

	IsRunningIntoWall = (IsAccelerationLargeEnough && IsVelocitySmall && IsSameDirection) ? true : false;
}

void UAquaAnimInstance::UpdateJumpFallData()
{
	float NewTimeToJumpApex = -WorldVelocity.Z / GetMovementComponent()->GetGravityZ();
	TimeToJumpApex = IsJumping ? NewTimeToJumpApex : 0.f;
}

EAnimEnum_CardinalDirection UAquaAnimInstance::SelectCardinalDirectionFromAngle(float Angle, float DeadZone,
	EAnimEnum_CardinalDirection CurrentDirection, bool UseCurrentDirection)
{
	float AbsAngle = FMath::Abs(Angle);
	float FwdDeadZone = DeadZone;
	float BwdDeadZone = FwdDeadZone;

	if (UseCurrentDirection)
	{
		switch (CurrentDirection)
		{
		case EAnimEnum_CardinalDirection::Front:
			FwdDeadZone *= 2;
			break;

		case EAnimEnum_CardinalDirection::Backward:
			BwdDeadZone *= 2;
			break;

		default:
			break;
		}
	}

	if (AbsAngle <= FwdDeadZone + 45.f) return EAnimEnum_CardinalDirection::Front;

	if (AbsAngle >= 135.f - BwdDeadZone) return EAnimEnum_CardinalDirection::Backward;

	if (Angle > 0.f) return EAnimEnum_CardinalDirection::Right;

	return EAnimEnum_CardinalDirection::Left;
}