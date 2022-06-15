#pragma once
#include "MathUtil.h"
#include "CoreMinimal.h"
#include "CraftWorldCommon.generated.h"

namespace CraftWorldConstants
{
	//even numbers
	constexpr int SECTION_DIMENSION = 16;
	constexpr int NUM_OF_SECTIONS_IN_CHUNK = 10;
	
	constexpr int HALF_SECTION_DIMENSION = SECTION_DIMENSION * 0.5;
	constexpr int NUM_OF_BLOCK_IN_CHUNK_HEIGHT = SECTION_DIMENSION * NUM_OF_SECTIONS_IN_CHUNK;
	constexpr int NUM_OF_BLOCKS_IN_SECTION = SECTION_DIMENSION * SECTION_DIMENSION * SECTION_DIMENSION;

	constexpr int NUM_OF_BLOCKS_IN_CHUNK = NUM_OF_BLOCKS_IN_SECTION * NUM_OF_SECTIONS_IN_CHUNK;
	
	constexpr int WORLD_BLOCK_SIZE = 200;
	constexpr int WORLD_HALF_BLOCK_SIZE = WORLD_BLOCK_SIZE / 2;
	constexpr int WORLD_CHUNK_SIZE = WORLD_BLOCK_SIZE * SECTION_DIMENSION;
	constexpr int WORLD_HALF_CHUNK_SIZE = WORLD_CHUNK_SIZE * 0.5;

	constexpr int SEA_LEVEL = 50;
	constexpr int SNOW_LEVEL = 85;
}

UCLASS()
class UCraftWorldConstants : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Constants")
	static int32 GetSectionDimension()
	{
		return CraftWorldConstants::SECTION_DIMENSION;
	}

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Constants")
	static int32 GetWorldBlockSize()
	{
		return CraftWorldConstants::WORLD_BLOCK_SIZE;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Constants")
	static int32 GetWorldHalfBlockSize()
	{
		return CraftWorldConstants::WORLD_HALF_BLOCK_SIZE;
	}
};
