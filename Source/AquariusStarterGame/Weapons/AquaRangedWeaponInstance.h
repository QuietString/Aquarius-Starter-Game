#pragma once

#include "CoreMinimal.h"
#include "Weapons/AquaWeaponInstance.h"
#include "AquaRangedWeaponInstance.generated.h"

/**
 * UAquaRangedWeaponInstance
 *
 * A piece of equipment representing a ranged weapon spawned and applied to a pawn
 */
UCLASS()
class AQUARIUSSTARTERGAME_API UAquaRangedWeaponInstance : public UAquaWeaponInstance
{
	GENERATED_BODY()

public:
	UAquaRangedWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	float GetMaxDamageRange() const { return MaxDamageRange; }

protected:

	// The maximum distance at which this weapon can deal damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Config", meta = (ForceUnits = cm))
	float MaxDamageRange = 25000.0f;

private:
	// Time since this weapon was last fired (relative to world time)
	double LastFireTime = 0.0;

};
