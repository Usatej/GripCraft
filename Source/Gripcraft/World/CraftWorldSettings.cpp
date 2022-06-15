#include "CraftWorldSettings.h"

#include "Gripcraft/CraftAssetManager.h"

UCraftWorldSettings::UCraftWorldSettings()
{
	
}

const UCraftWorldSettings& UCraftWorldSettings::Get()
{
	return UCraftAssetManager::Get().GetWorldSettings();
}
