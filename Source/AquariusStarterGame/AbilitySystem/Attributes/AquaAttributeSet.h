#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AquaAttributeSet.generated.h"


class UAquaAbilitySystemComponent;
struct FGameplayEffectSpec;


/**
 * This macro defines a set of helper functions for accessing and initializing attributes.
 *
 * The following example of the macro:
 *		ATTRIBUTE_ACCESSORS(UAquaHealthSet, Health)
 * will create the following functions:
 *		static FGameplayAttribute GetHealthAttribute();
 *		float GetHealth() const;
 *		void SetHealth(float NewVal);
 *		void InitHealth(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// Delegate used to broadcast attribute events.
DECLARE_MULTICAST_DELEGATE_FourParams(FAquaAttributeEvent, AActor* /*EffectInstigator*/,
	AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /* EffectMagnitude*/);



/**
 * UAquaAttributeSet
 *
 *	Base attribute set class for the project.
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UAquaAttributeSet();

	UWorld* GetWorld() const override;

	UAquaAbilitySystemComponent* GetAquaAbilitySystemComponent() const;
};
