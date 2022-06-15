// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blocks/CraftBlock.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Gripcraft/Utils/CraftCoords.h"
#include "Gripcraft/Utils/CraftGenerationUtils.h"
#include "CraftWorldChunk.generated.h"

struct FCraftMeshSection;
struct FCraftChunkNeighbours;

enum class EChunkState: uint8
{
	Spawned,
	GeneratingBlocks,
	BlocksGenerated,
	GeneratingMesh,
	MeshGenerated,
	Ready
};

UCLASS()
class GRIPCRAFT_API ACraftWorldChunk : public AActor
{
	GENERATED_BODY()

	friend class UCraftWorldSubsystem;

public:
	// Sets default values for this actor's properties
	ACraftWorldChunk();

	virtual void PreInitializeComponents() override;
	void Initialize();

	EChunkState GetState() const;
	const FCraftChunkCoords& GetCoors() const;
	const TSharedRef<const FCraftBlock>& GetBlock(const FCraftRelativeCoords& BlockCoords) const;
	
	void SetBlock(const FCraftRelativeCoords& Coords, const FGameplayTag& BlockTag, const bool& bRegenerateMesh);
	
private:	
	void SetCoords(const FCraftChunkCoords& NewCoords);
	
	/*
	 * Fills block and height maps 
	 * Thread Safe
	 */
	void Initial_FillChunkMaps();
	void Internal_GenerateChunkMaps();
	void Internal_LoadChunkMaps();

	/*
	 * Starts mesh generation 
	 * Thread Safe
	 */
	void Initial_GenerateChunkMesh();

	/*
	 * Called after initial mesh generation to finish the process
	 */
	void Initial_FinishChunkMeshGeneration(const bool& AsyncCooking = true);
	
	/*
	* Updates the procedural mesh component sections based on MeshSections array. MeshSections array has to be filled before the ApplySectionMeshes call.
	*/
	void ApplyAllMeshSections(const bool& AsyncCooking = true);
	void ApplyMeshSection(const int& SectionIndex, const FCraftMeshSection& InMeshSection, const bool& AsyncCooking = true);

	void RegenerateMeshSection(const uint8& SectionIndex);

	//thread safe
	bool SaveChunk(const FString& WorldName, const int32& WorldSeed) const;

	TObjectPtr<UCraftWorldSubsystem> GetWorldSubsystem() const;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UProceduralMeshComponent> ProceduralMesh;
		
	FCraftChunkCoords Coords;

	//~ Maps
	FCraftBlocksMap Blocks;
	FCraftHeightMap Heights;
	//~ Maps

	//~MeshSections
	//TODO: Maybe create critical section for each mesh section - it allows generating each section on different thread
	FCriticalSection MeshSectionsMutex;
	FMeshSectionsArray MeshSections;
	//~MeshSections

	//~ State
	FCriticalSection StateSection;
	std::atomic<EChunkState> State;
	//~ State

	//~ Neighbours
	FCriticalSection UsedNeighboursSection;
	TSet<TObjectPtr<ACraftWorldChunk>> UsedNeighbours;
	//~ Neighbours
	
	mutable TObjectPtr<UCraftWorldSubsystem> _WorldSubsystem;
	FBox BoundingBox;
};
