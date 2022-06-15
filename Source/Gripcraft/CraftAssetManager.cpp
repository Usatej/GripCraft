#include "CraftAssetManager.h"
#include "CraftLogChannels.h"
#include "World/CraftWorldSettings.h"

UCraftAssetManager& UCraftAssetManager::Get()
{
	if (UCraftAssetManager* Instance = Cast<UCraftAssetManager>(GEngine->AssetManager))
	{
		return *Instance;
	}

	UE_LOG(LogCraft, Fatal, TEXT("Invalid AssetManager class. Check DefaultEngine.ini value (it must be set to CraftAssetManager)."));
	
	return *NewObject<UCraftAssetManager>();
}

const UCraftWorldSettings& UCraftAssetManager::GetWorldSettings()
{
	return GetOrLoadAsset<UCraftWorldSettings>(CraftWorldSettingsPath);
}
