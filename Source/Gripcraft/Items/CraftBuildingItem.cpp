// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftBuildingItem.h"

#include "Gripcraft/CraftLogChannels.h"
#include "Gripcraft/Utils/CraftTraceUtils.h"
#include "Gripcraft/World/CraftWorldSubsystem.h"
#include "Kismet/GameplayStatics.h"

UE_DEFINE_GAMEPLAY_TAG(CraftTag_Item_Usable_Building, "Item.Usable.Building")

UCraftBuildingItem::UCraftBuildingItem()
{
	Tags.AddTag(CraftTag_Item_Usable_Building);
}

void UCraftBuildingItem::Use_Implementation(ACraftCharacter* UsedBy)
{
	check(UsedBy);
	TObjectPtr<UCraftWorldSubsystem> WorldSubsystem = UsedBy->GetWorld()->GetSubsystem<UCraftWorldSubsystem>();

	//try to get location from building view component
	TObjectPtr<const UCraftBuildingViewComponent> BuildingViewComponent = UsedBy->GetBuildingViewComponent();
	FCraftWorldCoords BlockWorldCoords;
	if(!BuildingViewComponent->GetLocationOfPreviewBlock(BlockWorldCoords))
	{
		//if failed, try to do the line trace
		FHitResult HitResult;
		if(UCraftTraceUtils::LineTraceFromCamera(UGameplayStatics::GetPlayerCameraManager(UsedBy, 0), UsedBy->GetBuildingDistanceInWorldUnits(), HitResult))
		{
			BlockWorldCoords = UCraftCoordsUtils::ConvertWorldPositionToWorldCoords(HitResult.Location + HitResult.Normal);
		} else
		{
			UE_LOG(LogCraft, Error, TEXT("Block %s cannot be build due to errors while getting the block's location."), *BlockToBuild.ToString());
			return;
		}
	}

	//create the block
	FHitResult HitResult;
	if(!UCraftTraceUtils::TraceForCharacterByBlock(UsedBy, BlockWorldCoords, HitResult))
	{
		WorldSubsystem->SetBlock(BlockWorldCoords, BlockToBuild);
	}	
}
