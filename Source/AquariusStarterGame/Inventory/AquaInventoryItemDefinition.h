#pragma once

#include "CoreMinimal.h"
#include "AquaInventoryItemDefinition.generated.h"

class UAquaInventoryItemInstance;


/**
 * Represents a fragment of an item definition
 */
UCLASS()
class UAquaInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UAquaInventoryItemInstance* Instance) {};
};


/**
 * UAquaInventoryItemDefinition 
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UAquaInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display, Instanced)
	TArray<TObjectPtr<UAquaInventoryItemFragment>> Fragments;

public:
	const UAquaInventoryItemFragment* FindFragmentByClass(TSubclassOf<UAquaInventoryItemFragment> FragmentClass) const;
};


UCLASS()
class UAquaInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = FragmentClass))
		static const UAquaInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UAquaInventoryItemDefinition> ItemDef, TSubclassOf<UAquaInventoryItemFragment> FragmentClass);
};