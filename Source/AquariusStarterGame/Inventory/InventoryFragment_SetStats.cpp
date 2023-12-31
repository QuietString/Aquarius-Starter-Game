#include "InventoryFragment_SetStats.h"

#include "AquaInventoryItemInstance.h"


void UInventoryFragment_SetStats::OnInstanceCreated(UAquaInventoryItemInstance* Instance)
{
	for (const auto& KVP : InitialItemStats)
	{
		Instance->AddStatTagStack(KVP.Key, KVP.Value);
	}
}

int32 UInventoryFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}
