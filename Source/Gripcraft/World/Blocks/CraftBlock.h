// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "CraftBlockMaterial.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

#define FACTORY_METHOD(PropType, PropName) \
	FCraftBlock* _##PropName##(const PropType##& In##PropName) { \
		PropName = In##PropName; \
		return this; \
	}

UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftTags_Block_Unbreakable);

class GRIPCRAFT_API FCraftBlock: public TSharedFromThis<FCraftBlock>
{
public:
	FCraftBlock();
	virtual ~FCraftBlock();

	const uint8& GetIndex() const { return Index; };
	const ECraftBlockMaterial& GetMaterial() const;
	const FGameplayTag& GetNameTag() const;
	uint8 GetDurability() const { return Durability; }
	
	virtual uint8 GetTextureRowIndex() const { return TextureRowIndex; }
	virtual uint8 HasSingleTexture() const { return bSingleTexture; }
	void AddTag(const FGameplayTag& Tag);
	bool HasTag(const FGameplayTag& Tag) const;
	const FGameplayTagContainer& GetTags() const;

	FACTORY_METHOD(uint8, Index)
	FACTORY_METHOD(FGameplayTag, NameTag)
	FACTORY_METHOD(FString, Name)
	FACTORY_METHOD(ECraftBlockMaterial, Material)
	FACTORY_METHOD(float, Durability)
	FACTORY_METHOD(uint8, TextureRowIndex)
	FACTORY_METHOD(bool, bSingleTexture)
	
private:
	uint8 Index;
	FGameplayTag NameTag;
	FString Name;
	ECraftBlockMaterial Material;
	float Durability;

	//~ texture
	uint8 TextureRowIndex;
	bool bSingleTexture;
	//~ texture

	FGameplayTagContainer Tags;
};

inline const FGameplayTagContainer& FCraftBlock::GetTags() const
{
	return Tags;
}
