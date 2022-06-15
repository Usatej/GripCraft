// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftBlock.h"

UE_DEFINE_GAMEPLAY_TAG(CraftTags_Block_Unbreakable, "Block.Unbreakable")

FCraftBlock::FCraftBlock():
	Name(TEXT("unknown")),
	Material(ECraftBlockMaterial::Air),
	TextureRowIndex(0),
	bSingleTexture(false)
{
}

FCraftBlock::~FCraftBlock()
{
}

const ECraftBlockMaterial& FCraftBlock::GetMaterial() const
{
	return Material;
}

const FGameplayTag& FCraftBlock::GetNameTag() const
{
	return NameTag;
}

void FCraftBlock::AddTag(const FGameplayTag& Tag)
{
	Tags.AddTag(Tag);
}

bool FCraftBlock::HasTag(const FGameplayTag& Tag) const
{
	return Tags.HasTag(Tag);
}


