#pragma once

#include "CoreMinimal.h"
#include "AquaAttributeSet.h"
#include "AquaCombatSet.generated.h"

/**
 * UAquaCombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaCombatSet : public UAquaAttributeSet
{
	GENERATED_BODY()

public:

	UAquaCombatSet();

	ATTRIBUTE_ACCESSORS(UAquaCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UAquaCombatSet, BaseHeal);

protected:

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

private:

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Aqua|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Aqua|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;

};
