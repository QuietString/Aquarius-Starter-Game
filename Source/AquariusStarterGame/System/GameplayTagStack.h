#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GameplayTagContainer.h"
#include "GameplayTagStack.generated.h"

struct FGameplayTagStackContainer;

/**
 * Represents one stack of a gameplay tag (tag + count)
 */
USTRUCT(BlueprintType)
struct FGameplayTagStack
{
	GENERATED_BODY()

	FGameplayTagStack()
	{}

	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{}

	FString GetDebugString() const;

private:
	friend FGameplayTagStackContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};


/** Container of gameplay tag stacks */
USTRUCT(BlueprintType)
struct FGameplayTagStackContainer
{
	GENERATED_BODY()

	FGameplayTagStackContainer()
	{}

public:
	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	void AddStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	int32 GetStackCount(FGameplayTag Tag) const
	{
		return TagToCountMap.FindRef(Tag);
	}

	// Returns true if there is at least one stack of the specified tag
	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}

private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;

	// Accelerated list of tag stacks for queries
	TMap<FGameplayTag, int32> TagToCountMap;
};