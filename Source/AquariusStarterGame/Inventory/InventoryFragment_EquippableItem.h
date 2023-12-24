#pragma once

#include "CoreMinimal.h"
#include "AquaInventoryItemDefinition.h"
#include "Equipment/AquaEquipmentDefinition.h"
#include "InventoryFragment_EquippableItem.generated.h"


UCLASS()
class UInventoryFragment_EquippableItem : public UAquaInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Aqua)
	TSubclassOf<UAquaEquipmentDefinition> EquipmentDefinition;
};
