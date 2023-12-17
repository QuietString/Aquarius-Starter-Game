#pragma once

#include "CoreMinimal.h"
#include "System/GameplayTagStack.h"
#include "AquaInventoryItemDefinition.h"
#include "AquaInventoryItemInstance.generated.h"

/**
 * UAquaInventoryItemInstance
 */
UCLASS(BlueprintType)
class AQUARIUSSTARTERGAME_API UAquaInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UAquaInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

	TSubclassOf<UAquaInventoryItemDefinition> GetItemDef() const { return ItemDef; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	const UAquaInventoryItemFragment* FindFragmentByClass(TSubclassOf<UAquaInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

private:
	void SetItemDef(TSubclassOf<UAquaInventoryItemDefinition> InDef);

	friend struct FAquaInventoryList;

	UPROPERTY()
	FGameplayTagStackContainer StatTags;

	// The item definition
	UPROPERTY()
	TSubclassOf<UAquaInventoryItemDefinition> ItemDef;
};
