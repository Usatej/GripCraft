// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftCoords.h"
#include "UObject/Object.h"
#include "CraftTraceUtils.generated.h"

class ACraftCharacter;
/**
 * 
 */
UCLASS()
class GRIPCRAFT_API UCraftTraceUtils : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="Craft|Trace")
	static bool LineTraceFromCamera(APlayerCameraManager* PlayerCameraManager, float Distance, FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category="Craft|Trace")
	static bool TraceForCharacterByBlock(ACraftCharacter* Character, const FCraftWorldCoords& WorldCoords, FHitResult& HitResult);
};
