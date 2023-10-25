#include "AquaAttributeSet.h"
#include "AbilitySystem/AquaAbilitySystemComponent.h"

UAquaAttributeSet::UAquaAttributeSet()
{

}

UWorld* UAquaAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Super::GetWorld();
}

UAquaAbilitySystemComponent* UAquaAttributeSet::GetAquaAbilitySystemComponent() const
{
	return Cast<UAquaAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
