// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftGameMode.h"

#include "CraftGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "World/CraftWorldChunk.h"
#include "World/CraftWorldSubsystem.h"

void ACraftGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UCraftGameInstance* GameInstance = GetWorld()->GetGameInstance<UCraftGameInstance>();
	check(GameInstance);

	UCraftSaveUtils::LoadWorldData(GameInstance->GetSelectedWorldPath(), WorldSavedData);
	check(WorldSavedData.IsValid());

	UCraftWorldSubsystem* WorldSubsystem = GetWorld()->GetSubsystem<UCraftWorldSubsystem>();
	check(WorldSubsystem);

	WorldSubsystem->OnChunkGenerationFinished.AddDynamic(this, &ACraftGameMode::OnChunkGenerated);
}

void ACraftGameMode::RestartPlayer(AController* NewPlayer)
{
	UCraftWorldSubsystem* WorldSubsystem = GetWorld()->GetSubsystem<UCraftWorldSubsystem>();
	check(WorldSubsystem);
	
	if(WorldSubsystem->IsChunkGenerated(FCraftChunkCoords(WorldSavedData.PlayerPosition)))
	{
		FHitResult HitResult;
		FVector Start(WorldSavedData.PlayerPosition.X, WorldSavedData.PlayerPosition.Y, CraftWorldConstants::WORLD_BLOCK_SIZE * CraftWorldConstants::NUM_OF_BLOCK_IN_CHUNK_HEIGHT);
		FVector End(WorldSavedData.PlayerPosition.X, WorldSavedData.PlayerPosition.Y, -1000);
		if(GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
		{
			RestartPlayerAtTransform(NewPlayer, FTransform(HitResult.Location + FVector(0, 0, CraftWorldConstants::WORLD_BLOCK_SIZE)));
		} else
		{
			//There is no chunk surface, how?
			checkNoEntry();
		}
	}	
}

FCraftSavedWorldData ACraftGameMode::GetWorldSaveData() const
{
	return WorldSavedData;
}

void ACraftGameMode::BeginPlay()
{
	Super::BeginPlay();

	UCraftWorldSubsystem* WorldSubsystem = GetWorld()->GetSubsystem<UCraftWorldSubsystem>();
	check(WorldSubsystem);

	WorldSubsystem->BuildWorldAroundLocation(WorldSavedData.PlayerPosition);
}

void ACraftGameMode::OnChunkGenerated(ACraftWorldChunk* Chunk)
{
	FCraftChunkCoords PawnChunk(WorldSavedData.PlayerPosition);
	if(PawnChunk == Chunk->GetCoors())
	{
		RestartPlayer(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
}
