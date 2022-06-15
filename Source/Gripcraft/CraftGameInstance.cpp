// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftGameInstance.h"
#include "CraftLogChannels.h"
#include "EngineUtils.h"
#include "Character/CraftCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/CraftSaveUtils.h"
#include "World/CraftWorldSubsystem.h"

UCraftGameInstance::UCraftGameInstance():
	UGameInstance()
#if WITH_EDITOR
	, PIESeed(-1), bPIECleanSaves(true), bPIEDummyGeneration(false)
#endif 
{
}

void UCraftGameInstance::Init()
{
	Super::Init();

	//bind tear down delegate -  save the world when world tears down and we are in game world
	WorldTearDownHandle = FWorldDelegates::OnWorldBeginTearDown.AddLambda([this](UWorld* World)
	{
		if(World->GetName() == GetGameLevelName())
		{
			TObjectPtr<ACraftCharacter> Character;
			for(TActorIterator<ACraftCharacter> It(World); It; ++It)
			{
				if(*It != nullptr)
				{
					Character = *It;
					break;
				}
			}

#if WITH_EDITOR
			if(bPIEDummyGeneration && bPIECleanSaves)
			{
				UCraftWorldSubsystem* WorldSubsystem = World->GetSubsystem<UCraftWorldSubsystem>();
				if(WorldSubsystem)
				{
					UCraftSaveUtils::RemoveWorld(WorldSubsystem->GetWorldName());
				}
			} else
			{
				UCraftSaveUtils::SaveCurrentWorld(Character);
			}
#else 
			UCraftSaveUtils::SaveCurrentWorld(Character);
#endif
		}
	});
}

void UCraftGameInstance::Shutdown()
{
	Super::Shutdown();

	FWorldDelegates::OnWorldBeginTearDown.Remove(WorldTearDownHandle);
}

bool UCraftGameInstance::CreateNewWorld(const FString& WorldName, const int32& WorldSeed, FString& WorldPath)
{
	FCraftSavedWorldData WorldData;
	WorldData.WorldName = WorldName;
	WorldData.WorldSeed = WorldSeed;
	
	if(UCraftSaveUtils::SaveWorldData(WorldData))
	{
		WorldPath = UCraftSaveUtils::GetWorldFilePath(WorldData.WorldName);
		return true;
	}
	
	return false;
}

TSoftObjectPtr<UWorld> UCraftGameInstance::GetGameLevel() const
{
	return GameLevel;
}

FString UCraftGameInstance::GetGameLevelName() const
{
	return GameLevel.GetAssetName();
}

void UCraftGameInstance::SelectWorldToPlayIn(const FString& WorldPath)
{
	SelectedWorldPath = WorldPath;
}

const FString& UCraftGameInstance::GetSelectedWorldPath() const
{
#if WITH_EDITOR
	//while PIE we want to provide some WorldData even if game was not started from main menu
	//generate some random data and clean them when tearing down the world
	if(SelectedWorldPath.IsEmpty())
	{
		bPIEDummyGeneration = true;;
		int32 Seed = FMath::RandRange(1, TNumericLimits<int32>::Max());
		FString Name = FString::Printf(TEXT("PIE_%d"), Seed);
		
		const_cast<UCraftGameInstance*>(this)->CreateNewWorld(Name, Seed, SelectedWorldPath);
	}
#endif

	check(!SelectedWorldPath.IsEmpty());
	return SelectedWorldPath;
}
