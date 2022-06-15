// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gripcraft/Utils/CraftSaveUtils.h"
#include "GameFramework/GameModeBase.h"
#include "CraftGameMode.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GRIPCRAFT_API ACraftGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void RestartPlayer(AController* NewPlayer) override;

	FCraftSavedWorldData GetWorldSaveData() const;

protected:
	virtual void BeginPlay() override;
	
private:
	UFUNCTION()
	void OnChunkGenerated(ACraftWorldChunk* Chunk);
private:
	FCraftSavedWorldData WorldSavedData;	
};
