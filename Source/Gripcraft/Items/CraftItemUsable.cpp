// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftItemUsable.h"

#include "Gripcraft/CraftLogChannels.h"

UE_DEFINE_GAMEPLAY_TAG(CraftTag_Item_Usable, "Item.Usable")

UCraftItemUsable::UCraftItemUsable()
{
	Tags.AddTag(CraftTag_Item_Usable);
}


void UCraftItemUsable::Use_Implementation(ACraftCharacter* UsedBy)
{
	UE_LOG(LogCraft, Warning, TEXT("UCraftItemUsable::Use is not implemented. Override the method in the child!"));
}
