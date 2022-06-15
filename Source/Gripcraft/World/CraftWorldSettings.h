// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CraftWorldSettings.generated.h"

class ACraftWorldChunk;

UCLASS(BlueprintType, Const)
class GRIPCRAFT_API UCraftWorldSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UCraftWorldSettings();
	static const UCraftWorldSettings& Get();

	UPROPERTY(EditDefaultsOnly, Category="Craft|World")
	TSubclassOf<ACraftWorldChunk> ChunkClass;

	UPROPERTY(EditDefaultsOnly, Category="Craft|World")
	uint8 RenderDistance = 3;

	UPROPERTY(EditDefaultsOnly, Category="Craft|World")
	int32 ChunksUpdateLocationOffset = 500;
	
	UPROPERTY(EditDefaultsOnly, Category="Craft|World")
    TObjectPtr<UMaterialInterface> BlocksMaterial;
};
