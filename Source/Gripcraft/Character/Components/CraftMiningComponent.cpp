#include "CraftMiningComponent.h"

#include "Gripcraft/Character/CraftCharacter.h"
#include "Gripcraft/Utils/CraftTraceUtils.h"
#include "Gripcraft/World/CraftWorldSubsystem.h"
#include "Gripcraft/World/Blocks/BlocksRegistry.h"
#include "Gripcraft/World/Blocks/CraftBlock.h"
#include "Kismet/GameplayStatics.h"


UCraftMiningComponent::UCraftMiningComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ResetMiningState();
}

void UCraftMiningComponent::TryHitBlockInFrontOf()
{
	FHitResult HitResult;
	if(UCraftTraceUtils::LineTraceFromCamera(UGameplayStatics::GetPlayerCameraManager(GetCraftCharacterChecked(), 0), GetCraftCharacterChecked()->GetBuildingDistanceInWorldUnits(), HitResult))
	{
		HitBlock(UCraftCoordsUtils::ConvertWorldPositionToWorldCoords(HitResult.Location - HitResult.Normal));
	}
}

void UCraftMiningComponent::HitBlock(const FCraftWorldCoords& BlockCoords)
{
	TSharedPtr<const FCraftBlock> Block = GetCraftWorldSubsystemChecked()->GetBlock(BlockCoords);
	//if block is invalid or is undestroyable
	if(!(Block.IsValid() && !Block->HasTag(CraftTags_Block_Unbreakable)))
	{
		ResetMiningState();
		if(IsValid(CrackDecalActor))
		{
			CrackDecalActor->Hide();
		}
		return;
	}

	if(BlockCoords != MiningState.BlockCoords)
	{
		MiningState.Block = Block;
		MiningState.BlockCoords = BlockCoords;
		MiningState.Damage = 1;

		if(IsBlockFinished())
		{
			MineBlock();
			return;
		}

		//set new position
		GetCrackDecalActorChecked()->SetNewLocation(UCraftCoordsUtils::ConvertWorldCoordsToWorldPosition(BlockCoords));

		//get neighbours
		TArray<TSharedPtr<const FCraftBlock>> Neighbours = GetCraftWorldSubsystemChecked()->GetBlockNeighbours(BlockCoords);
		TArray<uint8> DirectionIndexes;
		for(uint8 Index = 0; Index < Neighbours.Num(); Index++) 
		{
			if(Neighbours[Index].IsValid() && Neighbours[Index]->GetNameTag() == CraftTags_Block_Type_Air)
			{
				DirectionIndexes.Push(Index);
			}
		}
		//show only visible decals
		GetCrackDecalActorChecked()->Show(DirectionIndexes);
		
		return;
	}

	MiningState.Damage += 1;
	if(IsBlockFinished())
	{
		MineBlock();
	} else
	{
		UpdateCracks();
	}
}

void UCraftMiningComponent::ResetMiningState()
{
	MiningState.Damage = 0;
	MiningState.BlockCoords.X = TNumericLimits<int32>::Max();
	MiningState.BlockCoords.Y = TNumericLimits<int32>::Max();
	MiningState.Block.Reset();
}

ACraftCrackDecalActor* UCraftMiningComponent::GetCrackDecalActorChecked() const
{
	if(!IsValid(CrackDecalActor))
	{
		CrackDecalActor = GetWorld()->SpawnActor<ACraftCrackDecalActor>(CrackDecalActorClass);
		//hide by default
		CrackDecalActor->Hide();
	}

	return CrackDecalActor;
}

bool UCraftMiningComponent::IsBlockFinished() const
{
	return MiningState.Damage >= MiningState.Block->GetDurability();
}

void UCraftMiningComponent::UpdateCracks()
{
	GetCrackDecalActorChecked()->UpdateCracks(MiningState.Damage / MiningState.Block->GetDurability());
}

void UCraftMiningComponent::MineBlock()
{
	//destroy block
	GetCraftWorldSubsystemChecked()->DestroyBlock(MiningState.BlockCoords);
	
	ResetMiningState();
	GetCrackDecalActorChecked()->Hide();
}

