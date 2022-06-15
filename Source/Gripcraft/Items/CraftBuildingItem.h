// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftItem.h"
#include "CraftItemUsable.h"
#include "CraftBuildingItem.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftTag_Item_Usable_Building)

/**
 * 
 */
UCLASS()
class GRIPCRAFT_API UCraftBuildingItem : public UCraftItemUsable
{
	GENERATED_BODY()

public:
	UCraftBuildingItem();
		
	virtual void Use_Implementation(ACraftCharacter* UsedBy) override;
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Craft")
	FGameplayTag BlockToBuild;
};
