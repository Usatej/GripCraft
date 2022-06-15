#pragma once

#include "CoreMinimal.h"
#include "Array2D.h"
#include "Array3D.h"
#include "CraftChunkCoords.h"
#include "CraftChunkNeighbours.h"
#include "ProceduralMeshComponent.h"
#include "Gripcraft/World/CraftWorldCommon.h"

class FCraftBlock;

struct FCraftHeightMapGenerationData
{
	int32 Seed;
};

struct FCraftBlocksMapGenerationData
{
	int32 Seed;
	FCraftChunkCoords ChunkCoords;
};

struct FCraftMeshSection
{	
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;

	void Reset()
	{
		Vertices.Empty();
		Triangles.Empty();
		Normals.Empty();
		UVs.Empty();
		Tangents.Empty();
		VertexColors.Empty();
	}
};

typedef TArray2D<uint8,	CraftWorldConstants::SECTION_DIMENSION, CraftWorldConstants::SECTION_DIMENSION> FCraftHeightMap;
typedef TArray3D<TSharedRef<const FCraftBlock>, CraftWorldConstants::SECTION_DIMENSION, CraftWorldConstants::SECTION_DIMENSION, CraftWorldConstants::NUM_OF_BLOCK_IN_CHUNK_HEIGHT> FCraftBlocksMap;
typedef TArray<FCraftMeshSection> FMeshSectionsArray;

class FCraftGenerationUtils
{
public:
	//~ Chunk generation
	static void GenerateHeightMap(const FCraftHeightMapGenerationData& Data, const FCraftChunkCoords& ChunkCoords, FCraftHeightMap& HeightMap);
	static void GenerateBlocksMap(const FCraftBlocksMapGenerationData& Data, FCraftHeightMap& HeightMap, FCraftBlocksMap& BlocksMap);
	static const FCraftBlock& GenerateBlock(const uint8& X, const uint8& Y, const uint8& Z, const uint8& SurfaceHeight, const FRandomStream& RandomStream);

	static void GenerateTree(const uint8& X, const uint8& Y, const uint8& Z, FCraftHeightMap& HeightMap, FCraftBlocksMap& BlocksMap, const FRandomStream& RandomStream);
	//~ Chunk generation
	
	//~ Chunk mesh generation
	static void ConstructAllMeshSections(FMeshSectionsArray& MeshSections, const FCraftChunkNeighbours& Neighbours, const FCraftBlocksMap& Blocks, const FCraftHeightMap& Heights, const bool& bClearSections = true);
	static void ConstructMeshSection(const int& SectionIndex, FCraftMeshSection& MeshSection, const FCraftChunkNeighbours& Neighbours, const FCraftBlocksMap& Blocks, const FCraftHeightMap& Heights, const bool& bClearSection = true);
	static void AddFace(FCraftMeshSection& MeshSection, const FVector& UnitLocation, const int& DirectionIndex, const TSharedRef<const FCraftBlock>& Block);
	//~ Chunk mesh generation

	static uint8 GetMeshSectionIndex(const uint8& Z);


};