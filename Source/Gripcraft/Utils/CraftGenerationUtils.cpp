#include "CraftGenerationUtils.h"

#include "CraftCoords.h"
#include "CraftMath.h"
#include "CraftMeshUtils.h"
#include "Gripcraft/ThirdParty/FastNoise.h"
#include "Gripcraft/World/CraftWorldChunk.h"
#include "Gripcraft/World/Blocks/BlocksRegistry.h"
#include "Gripcraft/World/Blocks/CraftBlock.h"

void FCraftGenerationUtils::GenerateHeightMap(const FCraftHeightMapGenerationData& Data, const FCraftChunkCoords& ChunkCoords, FCraftHeightMap& HeightMap)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("FCraftGenerationUtils::GenerateHeightMap"))

	const FastNoise Noise(Data.Seed);
	
	for(uint8 X = 0; X < CraftWorldConstants::SECTION_DIMENSION; X++)
	{
		for(uint8 Y = 0; Y < CraftWorldConstants::SECTION_DIMENSION; Y++)
		{
			const int32 WorldX = UCraftCoordsUtils::ConvertRelativeToWorldCoord(ChunkCoords.X, X);
			const int32 WorldY = UCraftCoordsUtils::ConvertRelativeToWorldCoord(ChunkCoords.Y, Y);
			float CalculatedNoise = Noise.GetSimplexFractal(WorldX, WorldY);

			uint8 Z;
			if(WorldX < -20 && WorldY < -20)
			{
				float Distance = FMath::Min(FMath::Abs(WorldX + 20), FMath::Abs(WorldY + 20));
				float Additve = 20 * FMathf::Clamp(Distance / 20, 0, 1);
				float Coeficient = FMath::Max(16, 40 * FMathf::Clamp(Distance / 35, 0, 1));
				Z = CraftWorldConstants::SEA_LEVEL + static_cast<uint8>(CalculatedNoise * Coeficient) + Additve; 
			} else
			{
				Z = CraftWorldConstants::SEA_LEVEL + static_cast<uint8>(CalculatedNoise * 16);
			}
			HeightMap.At(X, Y) = Z;
		}
	}
}

void FCraftGenerationUtils::GenerateBlocksMap(const FCraftBlocksMapGenerationData& Data, FCraftHeightMap& HeightMap, FCraftBlocksMap& BlocksMap)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("FCraftGenerationUtils::GenerateBlocksMap"))

	FCraftHeightMap OriginalMap = HeightMap;
	
	//generate blocks
	FRandomStream RandomStream(Data.Seed + FMath::Abs(Data.ChunkCoords.X) * 10 + FMath::Abs(Data.ChunkCoords.Y) * 5);
	for(uint8 X = 0; X < CraftWorldConstants::SECTION_DIMENSION; X++)
	{
		for(uint8 Y = 0; Y < CraftWorldConstants::SECTION_DIMENSION; Y++)
		{
			const uint8 MaxHeight = FMath::Min(OriginalMap.At(X, Y), static_cast<uint8>(CraftWorldConstants::NUM_OF_BLOCK_IN_CHUNK_HEIGHT - 1));

			//generate blocks
			for(uint8 Z = 0; Z <= MaxHeight; Z++)
			{
				BlocksMap.At(X, Y, Z) = GenerateBlock(X, Y, Z, MaxHeight, RandomStream).AsShared();
			}
			
			// //generate blocks
			const uint8 StructureHeight = OriginalMap.At(X, Y) + 1;
			//check if we can spawn on the surface
			if(StructureHeight < CraftWorldConstants::NUM_OF_BLOCK_IN_CHUNK_HEIGHT)
			{
				//TODO: cannot spawn trees cross-chunk (do not spawn tree on the chunk's edge) - but would be nice to allow it in the future
				if(X > 1 && Y > 1 && X < CraftWorldConstants::SECTION_DIMENSION - 2 && Y < CraftWorldConstants::SECTION_DIMENSION - 2)
				{
					if(RandomStream.GetFraction() < 0.012)
					{
						GenerateTree(X, Y, StructureHeight, HeightMap, BlocksMap, RandomStream);
					}
				}
			}
		}
	}
}

const FCraftBlock& FCraftGenerationUtils::GenerateBlock(const uint8& X, const uint8& Y, const uint8& Z, const uint8& SurfaceHeight, const FRandomStream& RandomStream)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("ACraftWorldChunk::GenerateBlock"))

	if(Z == SurfaceHeight)
	{
		if(Z + RandomStream.RandRange(-1, 1) >= CraftWorldConstants::SNOW_LEVEL)
		{
			return FBlocksRegistry::Get().GetBlock(CraftTags_Block_Type_SnowGrass);
		}

		return FBlocksRegistry::Get().GetBlock(CraftTags_Block_Type_Grass);
	}

	if(Z - RandomStream.RandRange(0, 1) <= 0)
	{
		return  FBlocksRegistry::Get().GetBlock(CraftTags_Block_Type_Bedrock); 
	}

	if(Z + 2 < SurfaceHeight)
	{
		return FBlocksRegistry::Get().GetBlock(CraftTags_Block_Type_Stone); 
	}
	
	return FBlocksRegistry::Get().GetBlock(CraftTags_Block_Type_Dirt);
}

void FCraftGenerationUtils::GenerateTree(const uint8& X, const uint8& Y, const uint8& Z, FCraftHeightMap& HeightMap, FCraftBlocksMap& BlocksMap, const FRandomStream& RandomStream)
{
	const int32 TreeHeight = RandomStream.RandRange(4, 7);
	if(Z + TreeHeight >= CraftWorldConstants::NUM_OF_BLOCK_IN_CHUNK_HEIGHT)
	{
		//tree does not fit, skip it
		return;
	}

	if(Z + 7 > CraftWorldConstants::SNOW_LEVEL)
	{
		//do not spawn tree near snow
		return;
	}

	int32 Size = 2;
	if(FMath::IsWithinInclusive(X, 3, 13) && FMath::IsWithinInclusive(Y, 3, 13))
	{
		//tree can be bigger
		if(RandomStream.GetFraction() < 0.2)
		{
			Size = 3;
		}
	}

	int32 RandSize = RandomStream.RandRange(1, Size);
	
	for(int Height = 0; Height < TreeHeight; Height++)
	{
		for(int TreeX = (RandSize * -1); TreeX <= RandSize; TreeX++)
		{
			for(int TreeY = (RandSize * -1); TreeY <= RandSize; TreeY++)
			{				
				if(TreeX == 0 && TreeY == 0)
				{
					if(Height == TreeHeight - 1)
					{
						HeightMap.At(X, Y) = Z + Height;
						BlocksMap.At(X, Y, Z + Height) = FBlocksRegistry::GetBlockStatic(CraftTags_Block_Type_OakLeaf).AsShared();
					} else
					{
						//trunk
						BlocksMap.At(X, Y, Z + Height) = FBlocksRegistry::GetBlockStatic(CraftTags_Block_Type_Oak).AsShared();
					}
					continue;
				}

				//do not spawn leafs near surface
				if(Height < 2) continue;

				if((FMath::Abs(TreeX) <= 1 && FMath::Abs(TreeY) <= 1) || RandomStream.GetFraction() < 0.8)
				{
					//spawn leafs only when there is a free slot -- air
					if(BlocksMap.At(X + TreeX, Y + TreeY, Z + Height)->GetNameTag() == CraftTags_Block_Type_Air)
					{
						if(HeightMap.At(X + TreeX, Y + TreeY) < Z + Height)
						{
							HeightMap.At(X + TreeX, Y + TreeY) = Z + Height;
						}
						BlocksMap.At(X + TreeX, Y + TreeY, Z + Height) = FBlocksRegistry::GetBlockStatic(CraftTags_Block_Type_OakLeaf).AsShared();
					}
				}	
			}
		}
	}
}

void FCraftGenerationUtils::ConstructAllMeshSections(FMeshSectionsArray& MeshSections, const FCraftChunkNeighbours& Neighbours, const FCraftBlocksMap& Blocks, const FCraftHeightMap& Heights, const bool& bClearSections)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("ACraftWorldChunk::ConstructAllMeshSections"))

	for(int SectionIndex = 0; SectionIndex < CraftWorldConstants::NUM_OF_SECTIONS_IN_CHUNK; SectionIndex++)
	{
		ConstructMeshSection(SectionIndex, MeshSections[SectionIndex], Neighbours, Blocks, Heights, bClearSections);
	}
}

void FCraftGenerationUtils::ConstructMeshSection(const int& SectionIndex, FCraftMeshSection& InMeshSection, const FCraftChunkNeighbours& Neighbours, const FCraftBlocksMap& Blocks, const FCraftHeightMap& Heights, const bool& bClearSection)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("ACraftWorldChunk::ConstructMeshSection"))

	if(bClearSection)
	{
		InMeshSection.Reset();
	}
	
	const TObjectPtr<ACraftWorldChunk> NextChunks[6] = {
		Neighbours.Right,
		Neighbours.Left,
		Neighbours.Front,
		Neighbours.Back,
		nullptr,
		nullptr,
	};

	int MinZ = SectionIndex * CraftWorldConstants::SECTION_DIMENSION;
	int MaxZ = MinZ + CraftWorldConstants::SECTION_DIMENSION - 1;
	
	//rest of blocks
	for(int X = 0; X < CraftWorldConstants::SECTION_DIMENSION; X++)
	{
		for(int Y = 0; Y < CraftWorldConstants::SECTION_DIMENSION ; Y++)
		{
			for(int Z = MinZ; Z <= FMath::Min(Heights.At(X, Y), static_cast<uint8>(MaxZ)); Z++)
			{
				const TSharedRef<const FCraftBlock>& Block = Blocks.At(X, Y, Z);
				if(Block->GetMaterial() != ECraftBlockMaterial::Air)
				{
					for(int DirIndex = 0; DirIndex < 6; DirIndex++)
					{
						FIntVector NextBlock = FIntVector(X, Y, Z) + CraftMeshUtils::Directions[DirIndex];
						
						if(
							FMath::IsWithin(NextBlock.X, 0, CraftWorldConstants::SECTION_DIMENSION) &&
							FMath::IsWithin(NextBlock.Y, 0, CraftWorldConstants::SECTION_DIMENSION) &&
							FMath::IsWithin(NextBlock.Z, 0, CraftWorldConstants::NUM_OF_BLOCK_IN_CHUNK_HEIGHT)
						  )
						{
							
							if(Blocks.At(NextBlock.X, NextBlock.Y, NextBlock.Z)->GetMaterial() == ECraftBlockMaterial::Air)
							{
								AddFace(InMeshSection, FVector(X, Y, Z), DirIndex, Block);
							}
						} else
						{
							//we need to check next chunk;
							if(NextChunks[DirIndex] != nullptr)
							{
								int NextChunkBlockX = CraftMath::Modulo(NextBlock.X, CraftWorldConstants::SECTION_DIMENSION);
								int NextChunkBlockY = CraftMath::Modulo(NextBlock.Y, CraftWorldConstants::SECTION_DIMENSION);
								
								if(NextChunks[DirIndex]->GetBlock(FCraftRelativeCoords(NextChunkBlockX, NextChunkBlockY, NextBlock.Z))->GetMaterial() == ECraftBlockMaterial::Air)
								{
									AddFace(InMeshSection, FVector(X, Y, Z), DirIndex, Block);
								}
							}
						}
					}
				}
			}
		}
	}
}

void FCraftGenerationUtils::AddFace(FCraftMeshSection& MeshSection, const FVector& UnitLocation, const int& DirectionIndex, const TSharedRef<const FCraftBlock>& Block)
{
	//TODO: Fix to add only 8 vertexes for cube - right know it is 24 (in worst case) = 6 * 4 [num of faces * 4]
	const int VectexCount = MeshSection.Vertices.Num();
	//add 4 vertexes
	for(int i = 0; i < 4; i++)
	{
		const FVector BlockUnitPosition = UnitLocation + CraftMeshUtils::VertexPositions[CraftMeshUtils::Faces[DirectionIndex][i]] * 0.5;
		MeshSection.Vertices.Add(BlockUnitPosition * CraftWorldConstants::WORLD_BLOCK_SIZE + CraftWorldConstants::WORLD_HALF_BLOCK_SIZE);

		//add normals
		MeshSection.Normals.Add(FVector(CraftMeshUtils::Directions[DirectionIndex]));
	}

	//add face - two triangles
	MeshSection.Triangles.Append({
		VectexCount, VectexCount + 1, VectexCount + 2,
		VectexCount, VectexCount + 2, VectexCount + 3
	});
	
	const float TextureCoordYMin = 0.125 * Block->GetTextureRowIndex();
	const float TextureCoordYMax = TextureCoordYMin + 0.125;
	// Texture image has 8 columns (textures are 3-8)
	float TextureCoordXMin, TextureCoordXMax;
	if(Block->HasSingleTexture())
	{
		TextureCoordXMin = 0.25;
		TextureCoordXMax = 0.375;
	} else
	{
		TextureCoordXMin = 0.25 + DirectionIndex * 0.125; 
		TextureCoordXMax = TextureCoordXMin + 0.125;
	}

	MeshSection.UVs.Append({FVector2D(TextureCoordXMin, TextureCoordYMin), FVector2D(TextureCoordXMin, TextureCoordYMax), FVector2D(TextureCoordXMax, TextureCoordYMax), FVector2D(TextureCoordXMax, TextureCoordYMin) });
}

uint8 FCraftGenerationUtils::GetMeshSectionIndex(const uint8& Z)
{
	return Z / CraftWorldConstants::SECTION_DIMENSION;
}

