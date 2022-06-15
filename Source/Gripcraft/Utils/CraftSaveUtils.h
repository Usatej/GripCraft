#pragma once
#include "CoreMinimal.h"
#include "CraftChunkCoords.h"
#include "CraftGenerationUtils.h"
#include "CraftSaveUtils.generated.h"

class ACraftCharacter;
USTRUCT(BlueprintType)
struct FCraftSavedWorldFile
{
	GENERATED_BODY()

	FCraftSavedWorldFile(): Name(), WorldFilePath() {}
	FCraftSavedWorldFile(const FString& InName, const FString& InPath, const FDateTime& InLastModified):
		Name(InName),
		WorldFilePath(InPath),
		LastModified(InLastModified) {}

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Craft|Save")
	FString Name;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Craft|Save")
	FString WorldFilePath;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Craft|Save")
	FDateTime LastModified;
};

USTRUCT(BlueprintType)
struct FCraftSavedWorldData
{
	GENERATED_BODY()

	FCraftSavedWorldData(): WorldName(), WorldSeed(0), PlayerPosition(FVector::ZeroVector) {}
	FCraftSavedWorldData(const FString& InName, const int32& InSeed, const FVector& InPosition):
		WorldName(InName),
		WorldSeed(InSeed),
		PlayerPosition(InPosition) {}

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Craft|Save")
	FString WorldName;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Craft|Save")
	int32 WorldSeed;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Craft|Save")
	FVector PlayerPosition;

	bool IsValid() const
	{
		return !WorldName.IsEmpty() && WorldSeed > 0;
	}
};

FORCEINLINE FArchive& operator<<(FArchive &Ar, FCraftSavedWorldData& Data)
{
	Ar << Data.WorldName;
	Ar << Data.WorldSeed;
	Ar << Data.PlayerPosition;
	return Ar;
}


UCLASS()
class UCraftSaveUtils: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//~ Chunks
	static bool SaveChunkData(const FString& WorldName, const int32& WorldSeed, const FCraftChunkCoords& ChunkCoords, const FCraftBlocksMap& BlocksMap);
	static bool LoadChunkData(const FString& WorldName, const int32& WorldSeed, const FCraftChunkCoords& ChunkCoords, FCraftBlocksMap& BlocksMap, FCraftHeightMap& HeightMap);
	static bool DoesChunkSaveFileExist(const FString& WorldName, const FCraftChunkCoords& ChunkCoords);
	//~ Chunks

	UFUNCTION(BlueprintCallable, Category="Craft|save")
	static bool SaveCurrentWorld(ACraftCharacter* PlayerPawn);

	UFUNCTION(BlueprintCallable, Category="Craft|save")
	static bool LoadWorldData(const FString& WorldPath, FCraftSavedWorldData& WorldData);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Save")
	static FString GetWorldDirectoryPath(const FString& WorldName);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Save")
	static FString GetWorldFilePath(const FString& WorldName);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Save")
	static FString GetChunkFilePath(const FString& WorldName, const FCraftChunkCoords& Coords);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Save")
	static bool DoesWorldFileExists(const FString& WorldName);
	
	UFUNCTION(BlueprintCallable, Category="Craft|Save")
	static TArray<FCraftSavedWorldFile> ListSavedWorlds();
	UFUNCTION(BlueprintCallable, Category="Craft|Save")
	static bool SaveWorldData(const FCraftSavedWorldData& WorldData);
	UFUNCTION(BlueprintCallable, Category="Craft|Save")
	static void RemoveWorld(const FString& WorldName);

private:
	static void SaveLoadWorldData(FArchive& Archive, FCraftSavedWorldData& WorldData);
};


