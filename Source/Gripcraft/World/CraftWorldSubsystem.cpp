#include "CraftWorldSubsystem.h"

#include "CraftWorldChunk.h"
#include "CraftWorldSettings.h"
#include "Blocks/BlocksRegistry.h"
#include "Gripcraft/CraftGameInstance.h"
#include "Gripcraft/CraftGameMode.h"
#include "Gripcraft/CraftLogChannels.h"
#include "Gripcraft/Utils/CraftMeshUtils.h"
#include "Gripcraft/Utils/CraftSaveUtils.h"

namespace CraftWorldSubsystemConst
{
	constexpr int SPAWN_CHUNKS_PER_TICK = 10;
	constexpr int APPLY_CHUNK_MESHES_PER_TICK = 2;
}

UCraftWorldSubsystem::UCraftWorldSubsystem(): TargetPawn(nullptr), LastCalculatedPawnPosition(TNumericLimits<float>::Max())
{
}

bool UCraftWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	bool bParentCall = Super::ShouldCreateSubsystem(Outer);
	if(!bParentCall)
	{
		return false;
	}

	const UWorld* World = Cast<UWorld>(Outer);
	UCraftGameInstance* GameInstance = World->GetGameInstance<UCraftGameInstance>();
	if(!GameInstance)
	{
		return false;
	}
	
	return World->GetName() == GameInstance->GetGameLevelName();
}

void UCraftWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	WorldSettings = &UCraftWorldSettings::Get();
	FBlocksRegistry::Initialize();
	
	Super::Initialize(Collection);
}

void UCraftWorldSubsystem::Deinitialize()
{
	FBlocksRegistry::Deinitialize();
	Super::Deinitialize();
}

void UCraftWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	TObjectPtr<ACraftGameMode> GameMode = GetWorld()->GetAuthGameMode<ACraftGameMode>();
	check(GameMode);

	WorldSavedData = GameMode->GetWorldSaveData();
}

void UCraftWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(TargetPawn.IsValid())
	{
		CheckChunks(TargetPawn->GetActorLocation());
	}

	SpawnChunks();
	StartGeneration();
	FinishGeneration();
}

TStatId UCraftWorldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCraftWorldSubsystem, STATGROUP_Tickables); 
}

void UCraftWorldSubsystem::SpawnChunks()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("UCraftWorldSubsystem::SpawnChunks"))
	//Spawn limited number of chunks per tick - we dont want to freeze game thread for long time
	if(!ChunksToSpawn.IsEmpty())
	{
		int Counter = 0;
		while(!ChunksToSpawn.IsEmpty()) { // && Counter < CraftWorldSubsystemConst::SPAWN_CHUNKS_PER_TICK) {
			Counter++;
			FCraftChunkCoords ChunkCoords;
			ChunksToSpawn.Dequeue(ChunkCoords);
			TObjectPtr<ACraftWorldChunk> Chunk = SpawnChunk(ChunkCoords);

			ChunksToGenerate.Enqueue(Chunk);
		}
	}
}

void UCraftWorldSubsystem::StartGeneration()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("UCraftWorldSubsystem::StartGeneration"))
	//generate blocks
	while(!ChunksToGenerate.IsEmpty())
	{
		TObjectPtr<ACraftWorldChunk> Chunk;
		ChunksToGenerate.Dequeue(Chunk);

		AsyncTask(ENamedThreads::AnyThread,[Subsystem = this, Chunk]()
			{
				//initialize chunk
				Chunk->Initialize();
			
				//generate chunk's maps
				Chunk->Initial_FillChunkMaps();
			
				//try start mesh generation
				TArray<TObjectPtr<ACraftWorldChunk>> Neighbours = Subsystem->GetChunkNeighboursArray(Chunk->GetCoors());
				Subsystem->Task_TryStartMeshGeneration(Chunk, Neighbours);

				//try start mesh generation for chunk's neighbours
				TQueue<TObjectPtr<ACraftWorldChunk>> ToBeGenerated;
				for(auto& Neighbour: Neighbours)
				{
					TArray<TObjectPtr<ACraftWorldChunk>> NeighbourNeighbours = Subsystem->GetChunkNeighboursArray(Neighbour->GetCoors());
					Subsystem->Task_TryStartMeshGeneration(Neighbour, NeighbourNeighbours);
				}
			});
	}
}

void UCraftWorldSubsystem::FinishGeneration()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("UCraftWorldSubsystem::FinishGeneration"))
	if(!ChunksToFinishGeneration.IsEmpty())
	{
		int Counter = 0;
		while(!ChunksToFinishGeneration.IsEmpty() && Counter < CraftWorldSubsystemConst::APPLY_CHUNK_MESHES_PER_TICK)
		{
			Counter++;
	
			TObjectPtr<ACraftWorldChunk> Chunk;
			ChunksToFinishGeneration.Dequeue(Chunk);

			//we need to finish collision cooking of chunks under the player's pawn as soon as possible - do not do cook async;
			Chunk->Initial_FinishChunkMeshGeneration(Chunk->Coords != WorldSavedData.PlayerPosition);
			OnChunkGenerationFinished.Broadcast(Chunk);
		}
	}
}

const FString& UCraftWorldSubsystem::GetWorldName() const
{
	return WorldSavedData.WorldName;
}

const int32& UCraftWorldSubsystem::GetWorldSeed() const
{
	return WorldSavedData.WorldSeed;
}

const FCraftSavedWorldData& UCraftWorldSubsystem::GetWorldData() const
{
	return WorldSavedData;
}

TSharedPtr<const FCraftBlock> UCraftWorldSubsystem::GetBlock(const FCraftWorldCoords& BlockCoords)
{
	FCraftRelativeCoords RelativeCoords;
	FCraftChunkCoords ChunkCoords;
	UCraftCoordsUtils::ConvertWorldToRelativeCoords(BlockCoords, RelativeCoords, ChunkCoords);

	TObjectPtr<ACraftWorldChunk> Chunk = GetChunk(ChunkCoords);
	if(IsValid(Chunk))
	{
		return Chunk->GetBlock(RelativeCoords);
	}
	
	return nullptr;
}

TArray<TSharedPtr<const FCraftBlock>> UCraftWorldSubsystem::GetBlockNeighbours(const FCraftWorldCoords& BlockCoords)
{
	TArray<TSharedPtr<const FCraftBlock>> Neighbours;
	for (int DirIndex = 0; DirIndex < 6; DirIndex++)
	{
		FCraftWorldCoords NeighbourCoords = BlockCoords + CraftMeshUtils::Directions[DirIndex];
		Neighbours.Push(GetBlock(NeighbourCoords));
	}

	return Neighbours;
}

bool UCraftWorldSubsystem::SaveLoadedChunks()
{	
	FScopeLock Lock(&LoadedChunksMutex);
	for(auto& Pair: LoadedChunks)
	{
		if(!UCraftSaveUtils::SaveChunkData(WorldSavedData.WorldName, WorldSavedData.WorldSeed, Pair.Key, Pair.Value->Blocks))
		{
			return false;
		}
	}

	return true;
}

bool UCraftWorldSubsystem::IsChunkGenerated(const FCraftChunkCoords& Coords)
{
	TObjectPtr<ACraftWorldChunk>* ChunkPtr = nullptr;
	{
		FScopeLock Lock(&LoadedChunksMutex);
		ChunkPtr = LoadedChunks.Find(Coords);
	}
	
	if(ChunkPtr)
	{
		return (*ChunkPtr)->GetState() == EChunkState::Ready;
	}

	return false;
}

void UCraftWorldSubsystem::DestroyBlock(const FCraftWorldCoords& BlockCoords, const bool bRegenerateMesh)
{
	SetBlock(BlockCoords, CraftTags_Block_Type_Air, bRegenerateMesh);
}

void UCraftWorldSubsystem::SetBlock(const FCraftWorldCoords& BlockCoords, const FGameplayTag& BlockTag, const bool& bRegenerateMesh)
{
	if(BlockCoords.Z >= CraftWorldConstants::NUM_OF_BLOCK_IN_CHUNK_HEIGHT)
	{
		UE_LOG(LogCraft, Warning, TEXT("Cannot set block with tag %s to %s coods -> Z coordinate is greater then max height."), *BlockCoords.ToString(), *BlockTag.ToString());
		return;
	}
	
	FCraftRelativeCoords RelativeCoords;
	FCraftChunkCoords ChunkCoords;
	UCraftCoordsUtils::ConvertWorldToRelativeCoords(BlockCoords, RelativeCoords, ChunkCoords);

	TObjectPtr<ACraftWorldChunk> Chunk = nullptr;
	{
		FScopeLock Lock(&LoadedChunksMutex);
		TObjectPtr<ACraftWorldChunk>* ChunkPtr = LoadedChunks.Find(ChunkCoords);
		if(ChunkPtr == nullptr)
		{
			//TODO: should we allow this?
			//trying to set block in non-existing chunk
			return;
		}
		
		Chunk = *ChunkPtr;
	}

	Chunk->SetBlock(RelativeCoords, BlockTag, bRegenerateMesh);
}

void UCraftWorldSubsystem::SetBlockBP(const FCraftWorldCoords BlockCoords, const FGameplayTag BlockTag, const bool bRegenerateMesh)
{
	SetBlock(BlockCoords, BlockTag, bRegenerateMesh);
}

void UCraftWorldSubsystem::BuildWorldAroundLocation(const FVector& Location)
{
	CheckChunks(Location);
}

void UCraftWorldSubsystem::WatchAndBuildWorldAroundPawn(const APawn* InTargetPawn)
{
	TargetPawn = MakeWeakObjectPtr(InTargetPawn);
	if(TargetPawn.IsValid())
	{
		CheckChunks(TargetPawn->GetActorLocation());
	}
}

TObjectPtr<ACraftWorldChunk> UCraftWorldSubsystem::GetChunk(const FCraftChunkCoords& ChunkCoords) const
{
	const TObjectPtr<ACraftWorldChunk>* ChunkPtr = nullptr;
	{
		FScopeLock Lock(&LoadedChunksMutex);
		ChunkPtr = LoadedChunks.Find(ChunkCoords);
	}
	
	if(ChunkPtr != nullptr)
	{
		return *ChunkPtr;
	}
	
	return nullptr;
}

TObjectPtr<ACraftWorldChunk> UCraftWorldSubsystem::GetChunkUnsafe(const FCraftChunkCoords& ChunkCoords) const
{
	const TObjectPtr<ACraftWorldChunk>* ChunkPtr = LoadedChunks.Find(ChunkCoords);
	if(ChunkPtr != nullptr)
	{
		return *ChunkPtr;
	}
	
	return nullptr;
}

FCraftChunkNeighbours UCraftWorldSubsystem::GetChunkNeighbours(const FCraftChunkCoords& ChunkCoords) const
{
	FScopeLock Lock(&LoadedChunksMutex);
	
	FCraftChunkNeighbours Neighbours;
	Neighbours.Right = GetChunkUnsafe(FCraftChunkCoords(ChunkCoords.X + 1, ChunkCoords.Y));
	Neighbours.Left = GetChunkUnsafe(FCraftChunkCoords(ChunkCoords.X - 1, ChunkCoords.Y));
	Neighbours.Front = GetChunkUnsafe(FCraftChunkCoords(ChunkCoords.X, ChunkCoords.Y + 1));
	Neighbours.Back = GetChunkUnsafe(FCraftChunkCoords(ChunkCoords.X, ChunkCoords.Y - 1));
	
	return Neighbours;
}

TArray<TObjectPtr<ACraftWorldChunk>> UCraftWorldSubsystem::GetChunkNeighboursArray(const FCraftChunkCoords& ChunkCoords) const
{
	TArray<FCraftChunkCoords> Coords =
	{
		FCraftChunkCoords(ChunkCoords.X + 1, ChunkCoords.Y),
		FCraftChunkCoords(ChunkCoords.X - 1, ChunkCoords.Y),
		FCraftChunkCoords(ChunkCoords.X, ChunkCoords.Y + 1),
		FCraftChunkCoords(ChunkCoords.X, ChunkCoords.Y - 1)
	};
	
	TArray<TObjectPtr<ACraftWorldChunk>> Out;
	{
		FScopeLock Lock(&LoadedChunksMutex);
		for(const auto& Coord: Coords)
		{
			const TObjectPtr<ACraftWorldChunk>* Ptr = LoadedChunks.Find(Coord);
			if(Ptr != nullptr)
			{
				Out.Push(*Ptr);
			}
		}
	}
	
	return Out;
}

const UCraftWorldSettings& UCraftWorldSubsystem::GetWorldSettings() const
{
	return *WorldSettings;
}

TObjectPtr<ACraftWorldChunk> UCraftWorldSubsystem::SpawnChunk(const FCraftChunkCoords& Coords)
{
	FTransform SpawnTransform = FTransform(FVector(Coords.ToWorldVector()));
	
	TObjectPtr<ACraftWorldChunk> NewChunk = GetWorld()->SpawnActorDeferred<ACraftWorldChunk>(WorldSettings->ChunkClass, SpawnTransform);
	NewChunk->SetCoords(Coords);
	NewChunk->FinishSpawning(SpawnTransform);
	{
		FScopeLock Lock(&LoadedChunksMutex);
		LoadedChunks.Add(Coords, NewChunk);
	}
				
#if WITH_EDITOR
	NewChunk->SetFolderPath("/Chunks");
#endif
	return NewChunk;
}

void UCraftWorldSubsystem::CheckChunks(const FVector& Location)
{
	if(FVector::Distance(Location, LastCalculatedPawnPosition) > WorldSettings->ChunksUpdateLocationOffset) 
	{
		LastCalculatedPawnPosition = Location;

		UnloadChunks(Location);
		LoadChunks(Location);
	}
}

void UCraftWorldSubsystem::UnloadChunks(const FVector& Location)
{
	TArray<TObjectPtr<ACraftWorldChunk>> ChunksToUnload;
	for(const auto& Pair: LoadedChunks)
	{
		if(!IsChunkInRadius(Location, Pair.Key, WorldSettings->RenderDistance))
		{
			ChunksToUnload.Push(Pair.Value);
		}
	}
	
	for(auto& Chunk: ChunksToUnload)
	{
		{
			FScopeLock Lock(&LoadedChunksMutex);
			LoadedChunks.Remove(Chunk->GetCoors());
		}
		
		AsyncTask(ENamedThreads::AnyThread, [WorldName = WorldSavedData.WorldName, WorldSeed = WorldSavedData.WorldSeed, Chunk]()
		{
			if(!Chunk->SaveChunk(WorldName, WorldSeed))
			{
				UE_LOG(LogCraft, Error, TEXT("Saving the chunk with coords '%s' failed."), *Chunk->Coords.ToString());
			};
			
			//destroy chunk on game thread
			AsyncTask(ENamedThreads::GameThread, [Chunk]()
			{
				Chunk->Destroy();
			});
		});
		
	}
}

void UCraftWorldSubsystem::LoadChunks(const FVector& Location)
{
	const FCraftChunkCoords CurrentChunk(Location);
	const FCraftChunkCoords Min = CurrentChunk - WorldSettings->RenderDistance;
	const FCraftChunkCoords Max = CurrentChunk + WorldSettings->RenderDistance;

	{
		for(int32 XCoord = Min.X; XCoord <= Max.X; XCoord++)
		{
			for(int32 YCoord = Min.Y; YCoord <= Max.Y; YCoord++)
			{
				const FCraftChunkCoords Coords (XCoord, YCoord);
				if(!LoadedChunks.Contains(Coords) && IsChunkInRadius(Location, Coords, WorldSettings->RenderDistance))
				{
					ChunksToSpawn.Enqueue(Coords);
				}
			}
		}
	}
}

bool UCraftWorldSubsystem::IsChunkInRadius(const FVector& Location, const FCraftChunkCoords& ChunkCoords, const uint8& RenderDistance)
{
	FCraftChunkCoords CenterCoords(Location);
	float Distance = FCraftChunkCoords::Distance(CenterCoords, ChunkCoords);
	return Distance <= RenderDistance;
}

void UCraftWorldSubsystem::Task_GenerateChunkMesh(TObjectPtr<ACraftWorldChunk> Chunk)
{
	FCraftChunkCoords ChunkCoords = Chunk->GetCoors();

}

void UCraftWorldSubsystem::Task_TryStartMeshGeneration(TObjectPtr<ACraftWorldChunk> Chunk, const TArray<TObjectPtr<ACraftWorldChunk>>& Neighbours)
{
	//basic checks before locking
	if(Chunk->GetState() < EChunkState::BlocksGenerated)
	{
		return;	
	}
	
	bool bNeighboursGenerated = true;
	for(auto& Neighbour: Neighbours)
	{
		bNeighboursGenerated = bNeighboursGenerated && Neighbour->State >= EChunkState::BlocksGenerated;
	}
	
	if(bNeighboursGenerated)
	{
		{
			FScopeLock UsedChunksLock(&Chunk->UsedNeighboursSection);

			TArray<TObjectPtr<ACraftWorldChunk>> NotUsed;
			for(auto& Neighbour: Neighbours)
			{
				if(!Chunk->UsedNeighbours.Contains(Neighbour))
				{
					NotUsed.Push(Neighbour);
				}
			}

			
			if(NotUsed.Num() == 0)
			{
				//all chunks were used in generation
				return;
			}

			//some chunks have not been used yet, continue
			Chunk->UsedNeighbours.Append(NotUsed);
		}
		
		Chunk->State = EChunkState::GeneratingMesh;
		
		Chunk->Initial_GenerateChunkMesh();
		ChunksToFinishGeneration.Enqueue(Chunk);
	}
}

