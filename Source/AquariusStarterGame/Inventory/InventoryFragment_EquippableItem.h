#pragma once

#include "CoreMinimal.h"
#include "Equipment/AquaEquipmentDefinition.h"
#include "InventoryFragment_EquippableItem.generated.h"


UCLASS()
class AQUARIUSSTARTERGAME_API UInventoryFragment_EquippableItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Aqua)
	TSubclassOf<UAquaEquipmentDefinition> EquipmentDefinition;
};
