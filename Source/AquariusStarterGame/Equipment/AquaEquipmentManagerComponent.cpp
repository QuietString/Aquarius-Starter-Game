#include "AquaEquipmentManagerComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/AquaAbilitySet.h"
#include "AquaEquipmentDefinition.h"
#include "AquaEquipmentInstance.h"
#include "AbilitySystem/AquaAbilitySystemComponent.h"
#include "Engine/ActorChannel.h"

FString FAquaAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

UAquaEquipmentInstance* FAquaEquipmentList::AddEntry(TSubclassOf<UAquaEquipmentDefinition> EquipmentDefinition)
{
	UAquaEquipmentInstance* Result = nullptr;

	check(EquipmentDefinition);
	check(OwnerComponent);

	const UAquaEquipmentDefinition* EquipmentCDO = GetDefault<UAquaEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<UAquaEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UAquaEquipmentInstance::StaticClass();
	}

	FAquaAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UAquaEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
	Result = NewEntry.Instance;

	if (UAquaAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (TObjectPtr<const UAquaAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}

	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

	return Result;
}

void FAquaEquipmentList::RemoveEntry(UAquaEquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FAquaAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (UAquaAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Instance->DestroyEquipmentActors();

			EntryIt.RemoveCurrent();
		}
	}
}

UAquaAbilitySystemComponent* FAquaEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<UAquaAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

UAquaEquipmentManagerComponent::UAquaEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	bWantsInitializeComponent = true;
}

UAquaEquipmentInstance* UAquaEquipmentManagerComponent::EquipItem(
	TSubclassOf<UAquaEquipmentDefinition> EquipmentClass)
{
	UAquaEquipmentInstance* Result = nullptr;
	if (EquipmentClass)
	{
		Result = EquipmentList.AddEntry(EquipmentClass);
		if (Result)
		{
			Result->OnEquipped();
		}
	}
	return Result;
}

void UAquaEquipmentManagerComponent::UnequipItem(UAquaEquipmentInstance* ItemInstance)
{
	if (ItemInstance)
	{
		ItemInstance->OnUnequipped();
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

void UAquaEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UAquaEquipmentManagerComponent::UninitializeComponent()
{
	TArray<UAquaEquipmentInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator
	for (const FAquaAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (UAquaEquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}

UAquaEquipmentInstance* UAquaEquipmentManagerComponent::GetFirstInstanceOfType(
	TSubclassOf<UAquaEquipmentInstance> InstanceType)
{
	for (FAquaAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UAquaEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UAquaEquipmentInstance*> UAquaEquipmentManagerComponent::GetEquipmentInstanceOfType(
	TSubclassOf<UAquaEquipmentInstance> InstanceType) const
{
	TArray<UAquaEquipmentInstance*> Results;
	for (const FAquaAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UAquaEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}
