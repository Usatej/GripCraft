// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftTraceUtils.h"

#include "Gripcraft/Character/CraftCharacter.h"

bool UCraftTraceUtils::LineTraceFromCamera(APlayerCameraManager* PlayerCameraManager, float Distance, FHitResult& HitResult)
{
	if(!IsValid(PlayerCameraManager))
	{
		return false;
	}
	
	FVector Start = PlayerCameraManager->GetCameraLocation();
	FVector End = Start + FRotationMatrix(PlayerCameraManager->GetCameraRotation()).GetUnitAxis(EAxis::X) * Distance;
	
	return PlayerCameraManager->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
}

bool UCraftTraceUtils::TraceForCharacterByBlock(ACraftCharacter* Character, const FCraftWorldCoords& WorldCoords, FHitResult& HitResult)
{
	const FVector Start = UCraftCoordsUtils::ConvertWorldCoordsToWorldPosition(Start);
	const FVector End = Start + CraftWorldConstants::WORLD_BLOCK_SIZE;

	FCollisionObjectQueryParams Params;
	Params.AddObjectTypesToQuery(ECC_Pawn);
	
	return Character->GetWorld()->SweepSingleByObjectType(HitResult, Start, End, FQuat::Identity, Params, FCollisionShape::MakeBox(FVector(CraftWorldConstants::WORLD_HALF_BLOCK_SIZE)));
}
