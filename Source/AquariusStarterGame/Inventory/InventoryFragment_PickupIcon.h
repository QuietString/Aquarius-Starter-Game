#pragma once

#include "CoreMinimal.h"
#include "AquaInventoryItemDefinition.h"
#include "InventoryFragment_PickupIcon.generated.h"
UCLASS()
class UInventoryFragment_PickupIcon : public UAquaInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UInventoryFragment_PickupIcon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FLinearColor PadColor;
};
