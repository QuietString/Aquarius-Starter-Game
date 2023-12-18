#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AquaInventoryManagerComponent.generated.h"

class UAquaInventoryItemDefinition;
class UAquaInventoryItemInstance;
class UAquaInventoryManagerComponent;
struct FAquaInventoryList;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FAquaInventoryChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	UActorComponent* InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	UAquaInventoryItemInstance* Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 Delta = 0;
};


/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FAquaInventoryEntry
{
	GENERATED_BODY()

	FAquaInventoryEntry(){}

	FString GetDebugString() const;

private:
	friend FAquaInventoryList;
	friend UAquaInventoryManagerComponent;

	UPROPERTY()
	UAquaInventoryItemInstance* Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};


/** List of inventory items */
USTRUCT(BlueprintType)
struct FAquaInventoryList
{
	GENERATED_BODY()

	FAquaInventoryList()
		: OwnerComponent(nullptr){}

	FAquaInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

	TArray<UAquaInventoryItemInstance*> GetAllItems() const;

public:

	UAquaInventoryItemInstance* AddEntry(TSubclassOf<UAquaInventoryItemDefinition> ItemDef, int32 StackCount);

	void RemoveEntry(UAquaInventoryItemInstance* Instance);

private:
	friend UAquaInventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FAquaInventoryEntry> Entries;

	UPROPERTY()
	UActorComponent* OwnerComponent;
};


/**
 * Manages an inventory
 */
UCLASS(BlueprintType)
class UAquaInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAquaInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanAddItemDefinition(TSubclassOf<UAquaInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	UAquaInventoryItemInstance* AddItemDefinition(TSubclassOf<UAquaInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void RemoveItemInstance(UAquaInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure = false)
	TArray<UAquaInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure)
	UAquaInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UAquaInventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<UAquaInventoryItemDefinition> ItemDef) const;
	bool ConsumeItemsByDefinition(TSubclassOf<UAquaInventoryItemDefinition> ItemDef, int32 NumToConsume);

private:
	UPROPERTY()
	FAquaInventoryList InventoryList;

		
};
