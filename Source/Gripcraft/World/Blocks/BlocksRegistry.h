#pragma once
#include "CoreMinimal.h"
#include "CraftBlock.h"
#include "NativeGameplayTags.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftTags_Block_Type_Air);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftTags_Block_Type_Dirt);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftTags_Block_Type_Grass);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftTags_Block_Type_SnowGrass);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftTags_Block_Type_Oak);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftTags_Block_Type_OakLeaf);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftTags_Block_Type_Stone);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftTags_Block_Type_Bedrock);

class FBlocksRegistry
{
public:
	FBlocksRegistry();
	~FBlocksRegistry();
	
	static void Initialize();
	static void Deinitialize();
	static FBlocksRegistry& Get();

	static const FCraftBlock& GetBlockStatic(const FGameplayTag& BlockTag);
	const FCraftBlock& GetBlock(const FGameplayTag& BlockTag) const;

	static const FCraftBlock& GetBlockByIndexStatic(const uint8& Index);
	const FCraftBlock& GetBlockByIndex(const uint8& Index) const;
private:
	void RegisterBlocks();
	
	static TUniquePtr<FBlocksRegistry> Instance;
	
	TMap<FGameplayTag, int32> RegisteredBlocksMap;
	TArray<TSharedRef<FCraftBlock>> RegisteredBlocksArray;
};
