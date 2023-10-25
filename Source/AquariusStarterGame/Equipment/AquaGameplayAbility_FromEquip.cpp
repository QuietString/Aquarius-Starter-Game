// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/AquaGameplayAbility_FromEquip.h"

#include "AquaEquipmentInstance.h"

UAquaGameplayAbility_FromEquip::UAquaGameplayAbility_FromEquip(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAquaEquipmentInstance* UAquaGameplayAbility_FromEquip::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UAquaEquipmentInstance>(Spec->SourceObject);
	}

	return nullptr;
}

UObject* UAquaGameplayAbility_FromEquip::GetAssociatedItem() const
{
	if (UAquaEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Equipment->GetInstigator();
	}
	return nullptr;
}
