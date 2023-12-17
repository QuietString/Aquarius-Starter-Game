#include "Inventory/AquaInventoryItemDefinition.h"

UAquaInventoryItemDefinition::UAquaInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UAquaInventoryItemFragment* UAquaInventoryItemDefinition::FindFragmentByClass(
	TSubclassOf<UAquaInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UAquaInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

const UAquaInventoryItemFragment* UAquaInventoryFunctionLibrary::FindItemDefinitionFragment(
	TSubclassOf<UAquaInventoryItemDefinition> ItemDef, TSubclassOf<UAquaInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UAquaInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
