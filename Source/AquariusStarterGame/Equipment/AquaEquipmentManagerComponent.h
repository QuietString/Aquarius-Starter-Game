#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "AbilitySystem/AquaAbilitySet.h"
#include "AquaEquipmentManagerComponent.generated.h"


class UAquaEquipmentDefinition;
class UAquaEquipmentInstance;
class UAquaAbilitySystemComponent;
struct FAquaEquipmentList;
class UAquaEquipmentManagerComponent;


/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FAquaAppliedEquipmentEntry
{
	GENERATED_BODY()

	FAquaAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend FAquaEquipmentList;
	friend UAquaEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UAquaEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	UAquaEquipmentInstance* Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY()
	FAquaAbilitySet_GrantedHandles GrantedHandles;
};


/** List of applied equipment */
USTRUCT(BlueprintType)
struct FAquaEquipmentList
{
	GENERATED_BODY()

	FAquaEquipmentList()
		: OwnerComponent(nullptr)
	{}

	FAquaEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

public:
	UAquaEquipmentInstance* AddEntry(TSubclassOf<UAquaEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UAquaEquipmentInstance* Instance);

private:
	UAquaAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UAquaEquipmentManagerComponent;

	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FAquaAppliedEquipmentEntry> Entries;

	UPROPERTY()
	UActorComponent* OwnerComponent;
};

/**
 * Manages equipment applied to a pawn
 */
UCLASS(BlueprintType, Const)
class AQUARIUSSTARTERGAME_API UAquaEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UAquaEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	UAquaEquipmentInstance* EquipItem(TSubclassOf<UAquaEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable)
	void UnequipItem(UAquaEquipmentInstance* ItemInstance);

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAquaEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UAquaEquipmentInstance> InstanceType);

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UAquaEquipmentInstance*> GetEquipmentInstanceOfType(TSubclassOf<UAquaEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY()
	FAquaEquipmentList EquipmentList;
};
