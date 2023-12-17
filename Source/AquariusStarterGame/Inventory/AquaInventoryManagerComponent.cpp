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

UAquaInventoryManagerComponent::UAquaInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
{
}

bool UAquaInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UAquaInventoryItemDefinition> ItemDef,
	int32 StackCount)
{
}

UAquaInventoryItemInstance* UAquaInventoryManagerComponent::AddItemDefinition(
	TSubclassOf<UAquaInventoryItemDefinition> ItemDef, int32 StackCount)
{
}

void UAquaInventoryManagerComponent::RemoveItemInstance(UAquaInventoryItemInstance* ItemInstance)
{
}

TArray<UAquaInventoryItemInstance*> UAquaInventoryManagerComponent::GetAllItems() const
{
}

// Called when the game starts
void UAquaInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAquaInventoryManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

