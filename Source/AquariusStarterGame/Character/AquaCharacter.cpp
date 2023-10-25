#include "AquaCharacter.h"
#include "AquaCharacterMovementComponent.h"
#include "AquaGameplayTags.h"
#include "Character/AquaPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "AbilitySystem/AquaAbilitySystemComponent.h"
#include "Camera/AquaCameraComponent.h"
#include "Player/AquaPlayerController.h"
#include "Player/AquaPlayerState.h"

static FName NAME_AquaCharacterCollisionProfile_Capsule(TEXT("AquaPawnCapsule"));
static FName NAME_AquaCharacterCollisionProfile_Mesh(TEXT("AquaPawnMesh"));

AAquaCharacter::AAquaCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAquaCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetCullDistanceSquared = 900000000.0f;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_AquaCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_AquaCharacterCollisionProfile_Mesh);

	UAquaCharacterMovementComponent* AquaMoveComp = CastChecked<UAquaCharacterMovementComponent>(GetCharacterMovement());
	AquaMoveComp->GravityScale = 1.0f;
	AquaMoveComp->MaxAcceleration = 2400.0f;
	AquaMoveComp->BrakingFrictionFactor = 1.0f;
	AquaMoveComp->BrakingFriction = 6.0f;
	AquaMoveComp->GroundFriction = 8.0f;
	AquaMoveComp->BrakingDecelerationWalking = 1400.0f;
	AquaMoveComp->bUseControllerDesiredRotation = false;
	AquaMoveComp->bOrientRotationToMovement = false;
	AquaMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	AquaMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	AquaMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	AquaMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	AquaMoveComp->SetCrouchedHalfHeight(65.0f);

	PawnExtComponent = CreateDefaultSubobject<UAquaPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->RegisterAndCall_OnAbilitySystemInitialized(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));

	//HealthComponent = CreateDefaultSubobject<UAquaHealthComponent>(TEXT("HealthComponent"));

	CameraComponent = CreateDefaultSubobject<UAquaCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	CameraComponent->SetupAttachment(RootComponent);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

AAquaPlayerController* AAquaCharacter::GetAquaPlayerController() const
{
	return CastChecked<AAquaPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

AAquaPlayerState* AAquaCharacter::GetAquaPlayerState() const
{
	return CastChecked<AAquaPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UAquaAbilitySystemComponent* AAquaCharacter::GetAquaAbilitySystemComponent() const
{
	return Cast<UAquaAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AAquaCharacter::GetAbilitySystemComponent() const
{
	return PawnExtComponent->GetAquaAbilitySystemComponent();
}

void AAquaCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent())
	{
		AquaASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AAquaCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent())
	{
		return AquaASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AAquaCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent())
	{
		return AquaASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool AAquaCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent())
	{
		return AquaASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void AAquaCharacter::ToggleCrouch()
{
	const UAquaCharacterMovementComponent* AquaMoveComp = CastChecked<UAquaCharacterMovementComponent>(GetCharacterMovement());

	if (bIsCrouched || AquaMoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (AquaMoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void AAquaCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AAquaCharacter::OnAbilitySystemInitialized()
{
	UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent();
	check(AquaASC);

	InitializeGameplayTags();
}

void AAquaCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();
}

void AAquaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("aqua character setup input component: %s"), (*PlayerInputComponent->GetClass()->GetName()));
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void AAquaCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent())
	{
		const FAquaGameplayTags& GameplayTags = FAquaGameplayTags::Get();

		for (const TPair<uint8, FGameplayTag>& TagMapping : GameplayTags.MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				AquaASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : GameplayTags.CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				AquaASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		UAquaCharacterMovementComponent* AquaMoveComp = CastChecked<UAquaCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(AquaMoveComp->MovementMode, AquaMoveComp->CustomMovementMode, true);
	}
}

void AAquaCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	UAquaCharacterMovementComponent* AquaMoveComp = CastChecked<UAquaCharacterMovementComponent>(GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(AquaMoveComp->MovementMode, AquaMoveComp->CustomMovementMode, true);
}

void AAquaCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent())
	{
		const FAquaGameplayTags& GameplayTags = FAquaGameplayTags::Get();
		const FGameplayTag* MovementModeTag = nullptr;

		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = GameplayTags.CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = GameplayTags.MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			AquaASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

void AAquaCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent())
	{
		AquaASC->SetLooseGameplayTagCount(FAquaGameplayTags::Get().Status_Crouching, 1);
	}

	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AAquaCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent())
	{
		AquaASC->SetLooseGameplayTagCount(FAquaGameplayTags::Get().Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool AAquaCharacter::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

