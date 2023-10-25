#include "AquaHealthSet.h"
#include "AquaGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/AquaAbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Messages/AquaVerbMessage.h"
#include "GameplayEffectTypes.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage, "Gameplay.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity, "Gameplay.DamageImmunity");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_FellOutOfWorld, "Gameplay.Damage.FellOutOfWorld");
UE_DEFINE_GAMEPLAY_TAG(TAG_Aqua_Damage_Message, "Aqua.Damage.Message");

UAquaHealthSet::UAquaHealthSet()
	: Health(100.f), MaxHealth(100.f)
{
	bOutOfHealth = false;
}

void UAquaHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAquaHealthSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAquaHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);
}
void UAquaHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAquaHealthSet, Health, OldValue);
}

void UAquaHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAquaHealthSet, MaxHealth, OldValue);
}

bool UAquaHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);

		if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
		{
			// Do not take away any health
			Data.EvaluatedData.Magnitude = 0.f;
			return false;
		}

#if !UE_BUILD_SHIPPING
		// Check GodMode cheat.
		if (Data.Target.HasMatchingGameplayTag(FAquaGameplayTags::Get().Cheat_GodMode) && !bIsDamageFromSelfDestruct)
		{
			// Do not take away any health.
			Data.EvaluatedData.Magnitude = 0.f;
			return false;
		}

		// Check UnlimitedHealth cheat.
		if (Data.Target.HasMatchingGameplayTag(FAquaGameplayTags::Get().Cheat_UnlimitedHealth) && !bIsDamageFromSelfDestruct)
		{
			// Do not drop below 1 health.
			Data.EvaluatedData.Magnitude = FMath::Max(Data.EvaluatedData.Magnitude, (-GetHealth() + 1.0f));
		}
#endif // #if !UE_BUILD_SHIPPING
	}

	return true;
}

void UAquaHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Send a standardized verb message that other systems can observe
	if (Data.EvaluatedData.Magnitude < 0.f)
	{
		FAquaVerbMessage Message;
		Message.Verb = TAG_Aqua_Damage_Message;
		Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
		Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
		Message.Target = GetOwningActor();
		Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
		Message.Magnitude = Data.EvaluatedData.Magnitude;

		// Message subsystem is not implemented yet
		//UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
		//MessageSystem.BroadcastMessage(Message.Verb, Message);

		if ((GetHealth() <= 0.f) && !bOutOfHealth)
		{
			if (OnOutOfHealth.IsBound())
			{
				const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
				AActor* Instigator = EffectContext.GetOriginalInstigator();
				AActor* Causer = EffectContext.GetEffectCauser();

				OnOutOfHealth.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
			}
		}

		// Check health again in case an event above changed it.
		bOutOfHealth = (GetHealth() <= 0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(GetHealth() + GetHealing());
		SetHealing(0.f);
	}
}

void UAquaHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UAquaHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		// Make sure current health is not greater than the new max health;
		UAquaAbilitySystemComponent* AquaASC = GetAquaAbilitySystemComponent();
		check(AquaASC);

		AquaASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
	}

	if (bOutOfHealth && (GetHealth() > 0.f))
	{
		bOutOfHealth = false;
	}
}

void UAquaHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealingAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below 1.
		NewValue = FMath::Max(NewValue, 1.f);
	}
}
