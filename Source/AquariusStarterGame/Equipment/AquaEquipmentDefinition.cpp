#include "AquaEquipmentDefinition.h"
#include "AquaEquipmentInstance.h"

UAquaEquipmentDefinition::UAquaEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UAquaEquipmentInstance::StaticClass();
}
