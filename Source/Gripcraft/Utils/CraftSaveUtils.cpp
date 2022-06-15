#include "CraftSaveUtils.h"

#include "Gripcraft/CraftGameInstance.h"
#include "Gripcraft/CraftGameMode.h"
#include "Gripcraft/World/CraftWorldSubsystem.h"
#include "Gripcraft/World/Blocks/BlocksRegistry.h"
#include "Gripcraft/World/Blocks/CraftBlock.h"
#include "Serialization/BufferArchive.h"
#include "Gripcraft/CraftLogChannels.h"
#include "Gripcraft/Character/CraftCharacter.h"
#include "HAL/FileManagerGeneric.h"

#define WORLD_FILE_NAME TEXT("world.wcrt")
#define WORLD_DIRECTORY_PREFIX TEXT("CraftWorld_")

bool UCraftSaveUtils::SaveChunkData(const FString& WorldName, const int32& WorldSeed, const FCraftChunkCoords& ChunkCoords, const FCraftBlocksMap& BlocksMap)
{
	FBufferArchive Buffer;

	FCraftChunkCoords& Coords = const_cast<FCraftChunkCoords&>(ChunkCoords);

	//serialize seed
	Buffer.Serialize(&const_cast<int32&>(WorldSeed), sizeof(WorldSeed));
	
	//serialize coords
	Buffer.Serialize(&Coords.X, sizeof(Coords.X));
	Buffer.Serialize(&Coords.Y, sizeof(Coords.Y));

	//serialize blocks
	for(uint8 X = 0; X < CraftWorldConstants::SECTION_DIMENSION; X++)
	{
		for(uint8 Y = 0; Y < CraftWorldConstants::SECTION_DIMENSION; Y++)
		{
			for(uint8 Z = 0; Z < CraftWorldConstants::NUM_OF_BLOCK_IN_CHUNK_HEIGHT; Z++)
			{
				uint8 Index = BlocksMap.At(X, Y, Z)->GetIndex();
				Buffer.Serialize(&Index, sizeof(Index));
			}
		}
	}

	//save to filesystem
	FString Path = GetChunkFilePath(WorldName, ChunkCoords);
	if (FFileHelper::SaveArrayToFile(Buffer, *GetChunkFilePath(WorldName, ChunkCoords))) 
	{	
		return true;
	}
	return false;
}

bool UCraftSaveUtils::LoadChunkData(const FString& WorldName, const int32& WorldSeed, const FCraftChunkCoords& ChunkCoords, FCraftBlocksMap& BlocksMap, FCraftHeightMap& HeightMap)
{
	TArray<uint8> BinaryArray;
	if (!FFileHelper::LoadFileToArray(BinaryArray, *GetChunkFilePath(WorldName, ChunkCoords)))
	{
		return false;
	}
	
	if(BinaryArray.Num() <= 0)
	{
		return false;
	}
	
	FMemoryReader Reader(BinaryArray, true);
	Reader.Seek(0);

	//get seed
	int32 FileWorldSeed;
	Reader.Serialize(&FileWorldSeed, sizeof(FileWorldSeed));
	if(FileWorldSeed != WorldSeed)
	{
		UE_LOG(LogCraft, Warning, TEXT("Trying to read chunk from world with different world seed."));
		return false;
	}
	
	//Get coords
	FCraftChunkCoords Coords;
	Reader.Serialize(&Coords.X, sizeof(Coords.X));
	Reader.Serialize(&Coords.Y, sizeof(Coords.Y));

	if(ChunkCoords != Coords)
	{
		UE_LOG(LogCraft, Warning, TEXT("Trying to read chunk with different coords."));
		return false;
	}

	//read blocks and height
	for(uint8 X = 0; X < CraftWorldConstants::SECTION_DIMENSION; X++)
	{
		for(uint8 Y = 0; Y < CraftWorldConstants::SECTION_DIMENSION; Y++)
		{
			uint8 MaxHeight = 0;
			for(uint8 Z = 0; Z < CraftWorldConstants::NUM_OF_BLOCK_IN_CHUNK_HEIGHT; Z++)
			{
				uint8 Index;
				Reader.Serialize(&Index, sizeof(Index));

				//set block
				const FCraftBlock& Block = FBlocksRegistry::GetBlockByIndexStatic(Index);
				BlocksMap.At(X, Y, Z) = Block.AsShared();
				
				if(Block.GetNameTag() != CraftTags_Block_Type_Air)
				{
					MaxHeight = Z;	
				}
			}
			//set max height
			HeightMap.At(X, Y) = MaxHeight;
		}
	}

	return true;
}

bool UCraftSaveUtils::DoesChunkSaveFileExist(const FString& WorldName, const FCraftChunkCoords& ChunkCoords)
{
	//TODO: create some check sums - it can be invalid even if it exists
	return FPaths::FileExists(GetChunkFilePath(WorldName, ChunkCoords));
}

bool UCraftSaveUtils::SaveCurrentWorld(ACraftCharacter* PlayerPawn)
{
	if(!PlayerPawn)
	{
		UE_LOG(LogCraft, Error, TEXT("Cannot save current world due to invalid pawn."));
		return false;
	}
	
	UWorld* World = PlayerPawn->GetWorld();
	
	TObjectPtr<ACraftGameMode> GameMode = World->GetAuthGameMode<ACraftGameMode>();
	FCraftSavedWorldData SaveData = GameMode->GetWorldSaveData();
	SaveData.PlayerPosition = PlayerPawn->GetActorLocation();
	if(!SaveWorldData(SaveData))
	{
		UE_LOG(LogCraft, Error, TEXT("Cannot save current world. Saving of the world data failed."));
		return false;
	}

	UCraftWorldSubsystem* WorldSubsystem = World->GetSubsystem<UCraftWorldSubsystem>();
	if(!WorldSubsystem)
	{
		UE_LOG(LogCraft, Error, TEXT("Cannot save current world due to invalid world subsystem."));
		return false;
	}
	
	if(!WorldSubsystem->SaveLoadedChunks())
	{
		UE_LOG(LogCraft, Error, TEXT("Cannot save current world. Saving of the chunks failed."));
		return false;	
	}

	return true;
}

bool UCraftSaveUtils::LoadWorldData(const FString& WorldPath, FCraftSavedWorldData& WorldData)
{
	TArray<uint8> BinaryArray;
	if (!FFileHelper::LoadFileToArray(BinaryArray, *WorldPath))
	{
		return false;
	}
	
	if(BinaryArray.Num() <= 0)
	{
		return false;
	}
	
	FMemoryReader Reader(BinaryArray, true);
	Reader.Seek(0);

	SaveLoadWorldData(Reader, WorldData);
	return true;
}

FString UCraftSaveUtils::GetWorldDirectoryPath(const FString& WorldName)
{
	return FString::Printf(TEXT("%s%s%s/"), *FPaths::ProjectSavedDir(), WORLD_DIRECTORY_PREFIX,*WorldName);
}

FString UCraftSaveUtils::GetWorldFilePath(const FString& WorldName)
{
	return FString::Printf(TEXT("%s%s"), *GetWorldDirectoryPath(WorldName), WORLD_FILE_NAME);
}

FString UCraftSaveUtils::GetChunkFilePath(const FString& WorldName, const FCraftChunkCoords& Coords)
{
	return FString::Printf(TEXT("%schunks/c_%d_%d.ccrt"), *GetWorldDirectoryPath(WorldName), Coords.X, Coords.Y);
}

bool UCraftSaveUtils::DoesWorldFileExists(const FString& WorldName)
{
	return FFileManagerGeneric::Get().FileExists(*GetWorldFilePath(*WorldName));
}

TArray<FCraftSavedWorldFile> UCraftSaveUtils::ListSavedWorlds()
{
	FString DirectoryName = FString::Printf(TEXT("%s%s*"), *FPaths::ProjectSavedDir(), WORLD_DIRECTORY_PREFIX);
	TArray<FString> Directories;

	IFileManager& FileManager = FFileManagerGeneric::Get();
	FileManager.FindFiles(Directories, *DirectoryName, false, true);

	TArray<FCraftSavedWorldFile> SavedWorlds;
	for(const FString& Directory: Directories)
	{
		FString WorldFilePath = FString::Printf(TEXT("%s%s/%s"), *FPaths::ProjectSavedDir(), *Directory, WORLD_FILE_NAME);
		//TODO: create some check-sums to see if the world is valid. Checking the chunks would be nice too.
		if(FileManager.FileExists(*WorldFilePath))
		{
			FDateTime Modified = FFileManagerGeneric::Get().GetTimeStamp(*WorldFilePath);
			//chop the "CraftWorld_" substring
			FString WorldName = Directory.RightChop(11);
			SavedWorlds.Add(FCraftSavedWorldFile(WorldName, WorldFilePath, Modified));
		}
	}

	return SavedWorlds;
}

void UCraftSaveUtils::RemoveWorld(const FString& WorldName)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	const FString WorldDirPath = GetWorldDirectoryPath(WorldName).LeftChop(1);
	if(PlatformFile.DirectoryExists(*(WorldDirPath)))
	{
		PlatformFile.DeleteDirectoryRecursively(*WorldDirPath);
	}
}

bool UCraftSaveUtils::SaveWorldData(const FCraftSavedWorldData& WorldData)
{
	if(WorldData.WorldName.IsEmpty())
	{
		UE_LOG(LogCraft, Error, TEXT("Cannot save current world, because world name is empty."));
		return false;
	}
	
	FBufferArchive Buffer;
	SaveLoadWorldData(Buffer, const_cast<FCraftSavedWorldData&>(WorldData));
	
	if(Buffer.Num() <= 0)
	{
		UE_LOG(LogCraft, Error, TEXT("Cannot save world data, buffer is empty after serialization"));
		return false;
	}

	if(!FFileHelper::SaveArrayToFile(Buffer, *GetWorldFilePath(WorldData.WorldName)))
	{
		UE_LOG(LogCraft, Error, TEXT("Cannot save world data, saving failed"));
		return false;
	}

	return true;
}

void UCraftSaveUtils::SaveLoadWorldData(FArchive& Archive, FCraftSavedWorldData& WorldData)
{
	Archive << WorldData;
}


