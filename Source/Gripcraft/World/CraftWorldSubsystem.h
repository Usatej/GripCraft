#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Gripcraft/Utils/CraftChunkCoords.h"
#include "Gripcraft/Utils/CraftChunkNeighbours.h"
#include "Gripcraft/Utils/CraftCoords.h"
#include "Gripcraft/Utils/CraftSaveUtils.h"
#include "CraftWorldSubsystem.generated.h"

class FCraftBlock;
class ACraftWorldChunk;
class UCraftWorldSettings;
class ACraftWorldChunk;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChunkGenerationFinished, ACraftWorldChunk*, Chunk);

/**
 * 
 */
UCLASS()
class GRIPCRAFT_API UCraftWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UCraftWorldSubsystem();

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	//~ UTickableWorldSubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
public:
	virtual void Tick(float DeltaTime) override;
	TStatId GetStatId() const override;
	//~ UTickableWorldSubsystem

	//~Tick substeps
	void SpawnChunks();
	void StartGeneration();
	void FinishGeneration();
	//~Tick substeps

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|World")
	const FString& GetWorldName() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|World")
	const int32& GetWorldSeed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|World")
	const FCraftSavedWorldData& GetWorldData() const;
	
	UFUNCTION(BlueprintCallable, Category="Craft|World")
	bool SaveLoadedChunks();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|World")
	bool IsChunkGenerated(const FCraftChunkCoords& Coords);

	//~ GetBlock
	TSharedPtr<const FCraftBlock> GetBlock(const FCraftWorldCoords& BlockCoords);
	/*
	 * Order: Right, Left, Front, Back, Top, Bottom
	 */
	TArray<TSharedPtr<const FCraftBlock>> GetBlockNeighbours(const FCraftWorldCoords& BlockCoords);
	//~ GetBlock
	
	//~ SetBlock
	UFUNCTION(BlueprintCallable, Category="Craft|World")
	void DestroyBlock(const FCraftWorldCoords& BlockCoords, const bool bRegenerateMesh = true);
	void SetBlock(const FCraftWorldCoords& BlockCoords, const FGameplayTag& BlockTag, const bool& bRegenerateMesh = true);
	UFUNCTION(BlueprintCallable, Category="Craft|World", meta=(DisplayName="SetBlock"))
	void SetBlockBP(const FCraftWorldCoords BlockCoords, const FGameplayTag BlockTag, const bool bRegenerateMesh = true);
	//~ SetBlock

	UFUNCTION(BlueprintCallable, Category="Craft|World")
	void BuildWorldAroundLocation(const FVector& Location);
	
	UFUNCTION(BlueprintCallable, Category="Craft|World")
	void WatchAndBuildWorldAroundPawn(const APawn* InTargetPawn);

	//~ Events
	UPROPERTY(BlueprintAssignable, Category="Craft|World")
	FOnChunkGenerationFinished OnChunkGenerationFinished;
	//~ Events

	//thread safe version
	TObjectPtr<ACraftWorldChunk> GetChunk(const FCraftChunkCoords& ChunkCoords) const;
	//thread unsafe version
	TObjectPtr<ACraftWorldChunk> GetChunkUnsafe(const FCraftChunkCoords& ChunkCoords) const;
	FCraftChunkNeighbours GetChunkNeighbours(const FCraftChunkCoords& ChunkCoords) const;
	TArray<TObjectPtr<ACraftWorldChunk>> GetChunkNeighboursArray(const FCraftChunkCoords& ChunkCoords) const;
	
	const UCraftWorldSettings& GetWorldSettings() const;	
private:	
	//spawn new chunk
	TObjectPtr<ACraftWorldChunk> SpawnChunk(const FCraftChunkCoords& Coords);

	//check if some chunks should be spawn/despawn
	void CheckChunks(const FVector& Location);

	//unload unnecessary chunks outside render range
	void UnloadChunks(const FVector& Location);

	//load new chunks in render range
	void LoadChunks(const FVector& Location);
	
	void Task_GenerateChunkMesh(TObjectPtr<ACraftWorldChunk> Chunk);
	void Task_TryStartMeshGeneration(TObjectPtr<ACraftWorldChunk> Chunk, const TArray<TObjectPtr<ACraftWorldChunk>>& Neighbours);
	
	//check if chunk is in render radius
	static bool IsChunkInRadius(const FVector& Location, const FCraftChunkCoords& ChunkCoords, const uint8& RenderDistance);
	
private:
	FCraftSavedWorldData WorldSavedData;
	
	TWeakObjectPtr<const APawn> TargetPawn;
	FVector LastCalculatedPawnPosition;

	//~ Main chunks storage
	TMap<FCraftChunkCoords, TObjectPtr<ACraftWorldChunk>> LoadedChunks;
	mutable FCriticalSection LoadedChunksMutex;

	//~ Async containers and helpers
	TQueue<FCraftChunkCoords> ChunksToSpawn;
	TQueue<TObjectPtr<ACraftWorldChunk>> ChunksToGenerate;
	TQueue<TObjectPtr<ACraftWorldChunk>> ChunksToFinishGeneration;
	//~ Async containers and helpers
	
	UPROPERTY(EditDefaultsOnly, Category="Craft|World")
	TObjectPtr<const UCraftWorldSettings> WorldSettings;

	
};
