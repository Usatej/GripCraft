#include "BlocksRegistry.h"

UE_DEFINE_GAMEPLAY_TAG(CraftTags_Block_Type_Air, "Block.Type.Air")
UE_DEFINE_GAMEPLAY_TAG(CraftTags_Block_Type_Dirt, "Block.Type.Dirt")
UE_DEFINE_GAMEPLAY_TAG(CraftTags_Block_Type_Grass, "Block.Type.Grass")
UE_DEFINE_GAMEPLAY_TAG(CraftTags_Block_Type_SnowGrass, "Block.Type.SnowGrass")
UE_DEFINE_GAMEPLAY_TAG(CraftTags_Block_Type_Oak, "Block.Type.Oak")
UE_DEFINE_GAMEPLAY_TAG(CraftTags_Block_Type_OakLeaf, "Block.Type.OakLeaf")
UE_DEFINE_GAMEPLAY_TAG(CraftTags_Block_Type_Stone, "Block.Type.Stone")
UE_DEFINE_GAMEPLAY_TAG(CraftTags_Block_Type_Bedrock, "Block.Type.Bedrock")

#define REG_BLOCK(Tag, Class) \
	{ \
		int32 Index = RegisteredBlocksArray.Add(MakeShared<Class>()); \
		RegisteredBlocksArray.Last()->_Index(Index); \
	} \
	RegisteredBlocksArray.Last()->_Name(TEXT(#Tag))->_NameTag(CraftTags_Block_Type_##Tag)

#define MAT(Type) _Material(ECraftBlockMaterial::##Type)
#define T_INDEX(Index) _TextureRowIndex(Index)
#define T_SINGLE(Bool) _bSingleTexture(Bool)
#define DUR(Dur) _Durability(Dur)

#define AIR_Block_Type_INDEX 0

TUniquePtr<FBlocksRegistry> FBlocksRegistry::Instance = nullptr;

FBlocksRegistry::FBlocksRegistry()
{
	RegisterBlocks();

	//init translate map (tag to index)
	for(const auto& Block: RegisteredBlocksArray)
	{
		RegisteredBlocksMap.Add(Block->GetNameTag(), Block->GetIndex());
	}
}

FBlocksRegistry::~FBlocksRegistry()
{
}

void FBlocksRegistry::Initialize()
{
	Instance = MakeUnique<FBlocksRegistry>();
}

void FBlocksRegistry::Deinitialize()
{
	Instance.Reset();
}

FBlocksRegistry& FBlocksRegistry::Get()
{
	//if you are getting error here, you are calling Get() before the registry was actually initialized
	check(Instance.IsValid());
	return *Instance;
}

void FBlocksRegistry::RegisterBlocks()
{
	REG_BLOCK(Air, FCraftBlock)->MAT(Air); // Index = 0
	REG_BLOCK(Dirt, FCraftBlock)->MAT(Dirt)->T_INDEX(3)->T_SINGLE(true)->DUR(4);
	REG_BLOCK(Grass, FCraftBlock)->MAT(Dirt)->T_INDEX(1)->DUR(4);
	REG_BLOCK(SnowGrass, FCraftBlock)->MAT(Dirt)->T_INDEX(2)->DUR(4);
	REG_BLOCK(Oak, FCraftBlock)->MAT(Wood)->T_INDEX(4)->DUR(6);
	REG_BLOCK(OakLeaf, FCraftBlock)->MAT(Leaf)->T_INDEX(5)->T_SINGLE(true)->DUR(1);
	REG_BLOCK(Stone, FCraftBlock)->MAT(Stone)->T_INDEX(6)->T_SINGLE(true)->DUR(8);
	REG_BLOCK(Bedrock, FCraftBlock)->MAT(Bedrock)->T_INDEX(7)->T_SINGLE(true)->AddTag(CraftTags_Block_Unbreakable);
}

const FCraftBlock& FBlocksRegistry::GetBlockStatic(const FGameplayTag& BlockTag)
{
	return Get().GetBlock(BlockTag);
}

const FCraftBlock& FBlocksRegistry::GetBlock(const FGameplayTag& BlockTag) const
{
	const int32* IndexPtr = RegisteredBlocksMap.Find(BlockTag);
	if(IndexPtr != nullptr)
	{
		return *RegisteredBlocksArray[*IndexPtr];
	}

	//return air
	return *RegisteredBlocksArray[AIR_Block_Type_INDEX];
}

const FCraftBlock& FBlocksRegistry::GetBlockByIndexStatic(const uint8& Index)
{
	return Get().GetBlockByIndex(Index);
}

const FCraftBlock& FBlocksRegistry::GetBlockByIndex(const uint8& Index) const
{
	if(Index < RegisteredBlocksArray.Num())
	{
		return *RegisteredBlocksArray[Index];
	}

	return *RegisteredBlocksArray[AIR_Block_Type_INDEX];
}
