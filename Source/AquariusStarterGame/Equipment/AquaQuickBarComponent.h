#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "AquaQuickBarComponent.generated.h"


class UAquaInventoryItemInstance;
class UAquaEquipmentInstance;
class UAquaEquipmentManagerComponent;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class UAquaQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UAquaQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = Aqua)
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category = Aqua)
	void CycleActiveSlotBackward();

	UFUNCTION(BlueprintCallable, Category = Aqua)
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	TArray<UAquaInventoryItemInstance*> GetSlots() const { return Slots; }

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UAquaInventoryItemInstance* GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	void AddItemToSlot(int32 SlotIndex, UAquaInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UAquaInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	virtual void BeginPlay() override;

private:
	void EquipItemInSlot();
	void UnequipItemInSlot();

	UAquaEquipmentManagerComponent* FindEquipmentManager() const;

protected:
	UPROPERTY()
	int32 NumSlots = 3;

private:
	UPROPERTY()
	TArray<TObjectPtr<UAquaInventoryItemInstance>> Slots;

	UPROPERTY()
	int32 ActiveSlotIndex = -1;

	UPROPERTY()
	TObjectPtr<UAquaEquipmentInstance> EquippedItem;
};
