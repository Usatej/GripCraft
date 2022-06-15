// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftWorldChunk.h"

#include "CraftWorldSettings.h"
#include "CraftWorldSubsystem.h"
#include "Blocks/BlocksRegistry.h"
#include "Blocks/CraftBlock.h"
#include "Components/LineBatchComponent.h"
#include "Gripcraft/CraftLogChannels.h"
#include "Gripcraft/Utils/CraftSaveUtils.h"

#define ADD_USED_NEIGHBOUR(Chunk, Set) if(IsValid(Chunk)) Set.Add(Chunk);

// Sets default values
ACraftWorldChunk::ACraftWorldChunk()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMeshComponent");
	ProceduralMesh->bUseAsyncCooking = true;
	ProceduralMesh->CastShadow = false;
	SetRootComponent(ProceduralMesh);
	ProceduralMesh->SetRelativeLocation(FVector::Zero());

	State = EChunkState::Spawned;
}

void ACraftWorldChunk::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	FVector Pivot = Coords.ToWorldVector();
	BoundingBox = FBox(Pivot, Pivot + FVector(CraftWorldConstants::WORLD_CHUNK_SIZE, CraftWorldConstants::WORLD_CHUNK_SIZE, CraftWorldConstants::WORLD_CHUNK_SIZE * CraftWorldConstants::NUM_OF_SECTIONS_IN_CHUNK));
}

void ACraftWorldChunk::Initialize()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("ACraftWorldChunk::Initialize"))
	
	MeshSections.Init(FCraftMeshSection(), CraftWorldConstants::NUM_OF_SECTIONS_IN_CHUNK);
	
	TSharedRef<const FCraftBlock> AirBlock = FBlocksRegistry::GetBlockStatic(CraftTags_Block_Type_Air).AsShared();
	Blocks.Init([&AirBlock]()
	{
		return AirBlock;
	});
}

EChunkState ACraftWorldChunk::GetState() const
{
	return State;
}

const FCraftChunkCoords& ACraftWorldChunk::GetCoors() const
{
	return Coords;
}

const TSharedRef<const FCraftBlock>& ACraftWorldChunk::GetBlock(const FCraftRelativeCoords& BlockCoords) const
{
	return Blocks.At(BlockCoords.X, BlockCoords.Y, BlockCoords.Z);
}

void ACraftWorldChunk::SetBlock(const FCraftRelativeCoords& RCoords, const FGameplayTag& BlockTag, const bool& bRegenerateMesh)
{
	if(RCoords.Z >= CraftWorldConstants::NUM_OF_BLOCK_IN_CHUNK_HEIGHT)
	{
		UE_LOG(LogCraft, Warning, TEXT("Cannot set block with tag %s to %s coods -> Z coordinate is greater then max height."), *RCoords.ToString(), *BlockTag.ToString());
		return;
	}
	
	TSharedRef<const FCraftBlock> Block = Blocks.At(RCoords.X, RCoords.Y, RCoords.Z);
	if(Block->GetNameTag() == BlockTag)
	{
		//nosense
		return;
	}

	//update block
	Blocks.At(RCoords.X, RCoords.Y, RCoords.Z) = FBlocksRegistry::GetBlockStatic(BlockTag).AsShared();

	//update max height
	if(Heights.At(RCoords.X, RCoords.Y) < RCoords.Z)
	{
		Heights.At(RCoords.X, RCoords.Y) = RCoords.Z;
	}

	if(bRegenerateMesh)
	{
		const uint8 SectionIndex = FCraftGenerationUtils::GetMeshSectionIndex(RCoords.Z); 
		RegenerateMeshSection(SectionIndex);

		//check if the removed block is on the edge -- X/Y
		if(RCoords.X % (CraftWorldConstants::SECTION_DIMENSION - 1) == 0 || RCoords.Y % (CraftWorldConstants::SECTION_DIMENSION - 1) == 0)
		{
			//we need to regenerate neighbours
			for(auto& Neighbour:  GetWorldSubsystem()->GetChunkNeighboursArray(Coords))
			{
				Neighbour->RegenerateMeshSection(SectionIndex);
			}
		}

		//check if the removed block is on the edge -- Z
		if(RCoords.Z % (CraftWorldConstants::SECTION_DIMENSION) == 0)
		{
			if(SectionIndex > 0)
			{
				RegenerateMeshSection(SectionIndex - 1);
			}
		} else if(RCoords.Z % (CraftWorldConstants::SECTION_DIMENSION) == CraftWorldConstants::SECTION_DIMENSION - 1)
		{
			if(SectionIndex < CraftWorldConstants::NUM_OF_SECTIONS_IN_CHUNK - 1)
			{
				RegenerateMeshSection(SectionIndex + 1);
			}
		}
	}
}

void ACraftWorldChunk::SetCoords(const FCraftChunkCoords& NewCoords)
{
	Coords = NewCoords;

#if WITH_EDITOR
	const FString NewLabel = GetDefaultActorLabel() + "_" + Coords.ToString();
	SetActorLabel(NewLabel);
#endif
}

void ACraftWorldChunk::Initial_FillChunkMaps()
{
	{
		FScopeLock Lock(&StateSection);
		if(State != EChunkState::Spawned)
		{
			return;
		}

		State = EChunkState::GeneratingBlocks;
	}
	
	if(UCraftSaveUtils::DoesChunkSaveFileExist(GetWorldSubsystem()->GetWorldName(), Coords))
	{
		Internal_LoadChunkMaps();
	} else
	{
		Internal_GenerateChunkMaps();
	}

	State = EChunkState::BlocksGenerated;
}


void ACraftWorldChunk::Internal_GenerateChunkMaps()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("ACraftWorldChunk::Initial_GenerateChunkMaps"))
	
	FCraftHeightMapGenerationData HeightMapData;
	HeightMapData.Seed = GetWorldSubsystem()->GetWorldSeed();
	FCraftGenerationUtils::GenerateHeightMap(HeightMapData, Coords, Heights);

	FCraftBlocksMapGenerationData BlocksMapData;
	BlocksMapData.Seed = GetWorldSubsystem()->GetWorldSeed();
	BlocksMapData.ChunkCoords = Coords;
	FCraftGenerationUtils::GenerateBlocksMap(BlocksMapData, Heights, Blocks);
}

void ACraftWorldChunk::Internal_LoadChunkMaps()
{
	if(!UCraftSaveUtils::LoadChunkData(GetWorldSubsystem()->GetWorldName(), GetWorldSubsystem()->GetWorldSeed(), Coords, Blocks, Heights))
	{
		UE_LOG(LogCraft, Error, TEXT("Load of save file for chunk with coords '%s' failed. Starting map generation instead."), *Coords.ToString());
		Internal_GenerateChunkMaps();
	}
}

void ACraftWorldChunk::Initial_GenerateChunkMesh()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("ACraftWorldChunk::Initial_GenerateChunkMesh"))

	FCraftChunkNeighbours Neighbours = GetWorldSubsystem()->GetChunkNeighbours(Coords);
	{
		FScopeLock NeighboursLock(&UsedNeighboursSection);
		ADD_USED_NEIGHBOUR(Neighbours.Right, UsedNeighbours);
		ADD_USED_NEIGHBOUR(Neighbours.Left, UsedNeighbours);
		ADD_USED_NEIGHBOUR(Neighbours.Front, UsedNeighbours);
		ADD_USED_NEIGHBOUR(Neighbours.Back, UsedNeighbours);
	}
	
	{
		FScopeLock Lock(&MeshSectionsMutex);		
		FCraftGenerationUtils::ConstructAllMeshSections(MeshSections, Neighbours, Blocks,Heights);
	}
}

void ACraftWorldChunk::Initial_FinishChunkMeshGeneration(const bool& AsyncCooking)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("ACraftWorldChunk::Initial_FinishChunkMeshGeneration"))
	
	ApplyAllMeshSections(AsyncCooking);
	
	State = EChunkState::Ready;
}

void ACraftWorldChunk::ApplyAllMeshSections(const bool& AsyncCooking)
{	
	FScopeLock Lock(&MeshSectionsMutex);
	for(int SectionIndex = 0; SectionIndex < CraftWorldConstants::NUM_OF_SECTIONS_IN_CHUNK; SectionIndex++)
	{
		const FCraftMeshSection MeshSection = MeshSections[SectionIndex];
		ApplyMeshSection(SectionIndex, MeshSection, AsyncCooking);
	}
}

void ACraftWorldChunk::ApplyMeshSection(const int& SectionIndex, const FCraftMeshSection& InMeshSection, const bool& AsyncCooking)
{
	ProceduralMesh->bUseAsyncCooking = AsyncCooking;
	ProceduralMesh->CreateMeshSection(SectionIndex, InMeshSection.Vertices, InMeshSection.Triangles, InMeshSection.Normals, InMeshSection.UVs, InMeshSection.VertexColors, InMeshSection.Tangents, true);
	ProceduralMesh->SetMaterial(SectionIndex, GetWorldSubsystem()->GetWorldSettings().BlocksMaterial);
}

void ACraftWorldChunk::RegenerateMeshSection(const uint8& SectionIndex)
{
	FCraftChunkNeighbours Neighbours = GetWorldSubsystem()->GetChunkNeighbours(Coords);
	{
		FScopeLock Lock(&MeshSectionsMutex);
		FCraftGenerationUtils::ConstructMeshSection(SectionIndex, MeshSections[SectionIndex], Neighbours, Blocks, Heights);
		ApplyMeshSection(SectionIndex, MeshSections[SectionIndex]);
	}
}

bool ACraftWorldChunk::SaveChunk(const FString& WorldName, const int32& WorldSeed) const
{
	return UCraftSaveUtils::SaveChunkData(WorldName, WorldSeed, Coords, Blocks);
}

TObjectPtr<UCraftWorldSubsystem> ACraftWorldChunk::GetWorldSubsystem() const
{
	if(_WorldSubsystem == nullptr) {
		_WorldSubsystem = GetWorld()->GetSubsystem<UCraftWorldSubsystem>();
	}
	
	return _WorldSubsystem;
}



