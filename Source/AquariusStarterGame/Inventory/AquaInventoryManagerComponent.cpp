#include "AquaInventoryManagerComponent.h"
#include "NativeGameplayTags.h"
#include "AquaInventoryItemInstance.h"
#include "AquaInventoryItemDefinition.h"


UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Aqua_Inventory_Message_StackChanged, "Aqua.Inventory.Message.StackChanged");

FString FAquaInventoryEntry::GetDebugString() const
{
	TSubclassOf<UAquaInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

TArray<UAquaInventoryItemInstance*> FAquaInventoryList::GetAllItems() const
{
	TArray<UAquaInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FAquaInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr)
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

UAquaInventoryItemInstance* FAquaInventoryList::AddEntry(TSubclassOf<UAquaInventoryItemDefinition> ItemDef,
                                                         int32 StackCount)
{
	UAquaInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FAquaInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UAquaInventoryItemInstance>(OwnerComponent->GetOwner());
	NewEntry.Instance->SetItemDef(ItemDef);
	for (UAquaInventoryItemFragment* Fragment : GetDefault<UAquaInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	return Result;
}

void FAquaInventoryList::RemoveEntry(UAquaInventoryItemInstance* Instance)
{

}

UAquaInventoryManagerComponent::UAquaInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), InventoryList(this)
{
}

bool UAquaInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UAquaInventoryItemDefinition> ItemDef,
	int32 StackCount)
{
	return true;
}

UAquaInventoryItemInstance* UAquaInventoryManagerComponent::AddItemDefinition(
	TSubclassOf<UAquaInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UAquaInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
	}
	return Result;
}

void UAquaInventoryManagerComponent::RemoveItemInstance(UAquaInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);
}

TArray<UAquaInventoryItemInstance*> UAquaInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UAquaInventoryItemInstance* UAquaInventoryManagerComponent::FindFirstItemStackByDefinition(
	TSubclassOf<UAquaInventoryItemDefinition> ItemDef) const
{
	for (const FAquaInventoryEntry& Entry : InventoryList.Entries)
	{
		UAquaInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UAquaInventoryManagerComponent::GetTotalItemCountByDefinition(
	TSubclassOf<UAquaInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FAquaInventoryEntry& Entry : InventoryList.Entries)
	{
		UAquaInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UAquaInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UAquaInventoryItemDefinition> ItemDef,
	int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || OwningActor->HasAuthority())
	{
		return false;
	}

	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UAquaInventoryItemInstance* Instance = UAquaInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}
