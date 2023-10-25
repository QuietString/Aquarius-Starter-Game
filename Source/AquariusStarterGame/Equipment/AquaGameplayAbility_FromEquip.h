#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AquaGameplayAbility.h"
#include "AquaGameplayAbility_FromEquip.generated.h"

class UAquaEquipmentInstance;

/**
 * UAquaGameplayAbility_FromEquip
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaGameplayAbility_FromEquip : public UAquaGameplayAbility
{
	GENERATED_BODY()

public:
	UAquaGameplayAbility_FromEquip(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Aqua|Ability")
	UAquaEquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "Aqua|Ability")
	UObject* GetAssociatedItem() const;
};
