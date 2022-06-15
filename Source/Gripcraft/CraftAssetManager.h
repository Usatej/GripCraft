// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftLogChannels.h"
#include "Engine/AssetManager.h"
#include "CraftAssetManager.generated.h"

class UCraftWorldSettings;
/**
 * 
 */
UCLASS(Config=Game)
class GRIPCRAFT_API UCraftAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UCraftAssetManager& Get();
	
	const UCraftWorldSettings& GetWorldSettings();

protected:
	UPROPERTY(Config)
	TSoftObjectPtr<UCraftWorldSettings> CraftWorldSettingsPath;

	UPROPERTY(Transient)
	TMap<UClass*, UPrimaryDataAsset*> LoadedData;

	template<typename AssetClass>
	const AssetClass& GetOrLoadAsset(const TSoftObjectPtr<AssetClass>& DataPath)
	{
		//try to find in cache
		if (const UPrimaryDataAsset* const * FoundedAsset = LoadedData.Find(AssetClass::StaticClass()))
		{
			return *CastChecked<AssetClass>(*FoundedAsset);
		}

		UPrimaryDataAsset* Asset = nullptr;
		const FName PrimaryAssetType = AssetClass::StaticClass()->GetFName();

		//load asset
		if (GIsEditor)
		{
			Asset = DataPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		} else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if(Handle)
			{
				Handle->WaitUntilComplete(0.0f, false);
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}

		if(Asset)
		{
			LoadedData.Add(AssetClass::StaticClass(), Asset);
		} else
		{
			UE_LOG(LogCraft, Fatal, TEXT("Cannot load asset at %s. Type %s. Fix thet data to run."), *DataPath.ToString(), *PrimaryAssetType.ToString());
		}
		
		return *CastChecked<const AssetClass>(Asset);
	}
};
