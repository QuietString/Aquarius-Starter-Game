#include "AquaGameData.h"
#include "AquaAssetManager.h"

UAquaGameData::UAquaGameData()
{
}

const UAquaGameData& UAquaGameData::Get()
{
	return UAquaAssetManager::Get().GetGameData();
}
