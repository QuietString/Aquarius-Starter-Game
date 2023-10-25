#include "AquaCombatSet.h"
#include "Net/UnrealNetwork.h"


UAquaCombatSet::UAquaCombatSet()
	: BaseDamage(0.f), BaseHeal(0.f)
{
}

void UAquaCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAquaCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAquaCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}
void UAquaCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAquaCombatSet, BaseDamage, OldValue);
}

void UAquaCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAquaCombatSet, BaseHeal, OldValue);
}
