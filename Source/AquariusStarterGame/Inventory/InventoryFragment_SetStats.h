// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AquaInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "InventoryFragment_SetStats.generated.h"

UCLASS()
class UInventoryFragment_SetStats : public UAquaInventoryItemFragment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TMap<FGameplayTag, int32> InitialItemStats;
	
public:
	virtual void OnInstanceCreated(UAquaInventoryItemInstance* Instance) override;
	int32 GetItemStatByTag(FGameplayTag Tag) const;

};
