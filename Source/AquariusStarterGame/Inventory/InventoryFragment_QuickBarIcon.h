#pragma once

#include "CoreMinimal.h"
#include "AquaInventoryItemDefinition.h"
#include "InventoryFragment_QuickBarIcon.generated.h"

UCLASS()
class UInventoryFragment_QuickBarIcon : public UAquaInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush Brush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush AmmoBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FText DisplayNameWhenEquipped;
};
