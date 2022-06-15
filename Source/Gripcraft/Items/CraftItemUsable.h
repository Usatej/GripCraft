// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftItem.h"
#include "NativeGameplayTags.h"
#include "Gripcraft/Character/CraftCharacter.h"
#include "CraftItemUsable.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftTag_Item_Usable)

UCLASS()
class GRIPCRAFT_API UCraftItemUsable : public UCraftItem
{
	GENERATED_BODY()

public:
	UCraftItemUsable();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Craft|Items")
	void Use(ACraftCharacter* UsedBy);
};
