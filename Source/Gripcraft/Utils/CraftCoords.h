#pragma once
#include "CoreMinimal.h"
#include "CraftChunkCoords.h"
#include "Gripcraft/World/CraftWorldCommon.h"
#include "CraftCoords.generated.h"

USTRUCT(BlueprintType)
struct FCraftRelativeCoords
{
	GENERATED_BODY()

	FCraftRelativeCoords(): X(0), Y(0), Z(0) {}
	FCraftRelativeCoords(const uint8& InX, const uint8& InY, const uint8& InZ): X(InX), Y(InY), Z(InZ) {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Craft|Coords")
	uint8 X;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Craft|Coords")
	uint8 Y;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Craft|Coords")
	uint8 Z;

	FORCEINLINE bool operator ==(const FCraftRelativeCoords& Other) const
	{
		return X == Other.X && Y == Other.Y && Z == Other.Z;
	}

	FORCEINLINE bool operator !=(const FCraftRelativeCoords& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE bool IsValid() const
	{
		return
			FMath::IsWithin(X, 0, CraftWorldConstants::SECTION_DIMENSION) &&
			FMath::IsWithin(Y, 0, CraftWorldConstants::SECTION_DIMENSION) &&
			FMath::IsWithin(Z, 0, CraftWorldConstants::NUM_OF_BLOCK_IN_CHUNK_HEIGHT);
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("[Relative X=%d Y=%d Z=%d]"), X, Y, Z);
	}
};

USTRUCT(BlueprintType)
struct FCraftWorldCoords
{
	GENERATED_BODY()

	FCraftWorldCoords(): X(0), Y(0), Z(0) {}
	FCraftWorldCoords(const int32& InX, const int32& InY, const int32& InZ): X(InX), Y(InY), Z(InZ) {}
	FCraftWorldCoords(const FVector& Position): FCraftWorldCoords(WorldPositionToWorldCoords(Position)) {}
	FCraftWorldCoords(const FCraftWorldCoords& Other): X(Other.X), Y(Other.Y), Z(Other.Z) {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Craft|Coords")
	int32 X;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Craft|Coords")
	int32 Y;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Craft|Coords")
	int32 Z;

	FORCEINLINE bool operator ==(const FCraftWorldCoords& Other) const
	{
		return X == Other.X && Y == Other.Y && Z == Other.Z;
	}

	FORCEINLINE bool operator !=(const FCraftWorldCoords& Other) const
	{
		return !(*this == Other);
	}
	
	FORCEINLINE FCraftWorldCoords operator+(const FIntVector& Other) const
	{
		return FCraftWorldCoords(X + Other.X, Y + Other.Y, Z + Other.Z);
	}

	FORCEINLINE FCraftWorldCoords operator-(const FIntVector& Other) const
	{
		return FCraftWorldCoords(X - Other.X, Y - Other.Y, Z - Other.Z);
	}

	static FCraftWorldCoords WorldPositionToWorldCoords(const FVector& WorldPosition)
	{
		return FCraftWorldCoords(
				WorldAxisToWorldCoord(WorldPosition.X),
				WorldAxisToWorldCoord(WorldPosition.Y),
				WorldAxisToWorldCoord(WorldPosition.Z)
			);
	}

	static int32 WorldAxisToWorldCoord(const float& WorldAxis)
	{
		 return FMathf::Floor(WorldAxis / CraftWorldConstants::WORLD_BLOCK_SIZE);
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("[World X=%d Y=%d Z=%d]"), X, Y, Z);
	}
};


UCLASS()
class UCraftCoordsUtils: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Coords")
	static FVector AlignWorldPosition(const FVector& WorldPosition)
	{
		FCraftWorldCoords Coords = ConvertWorldPositionToWorldCoords(WorldPosition);
		return ConvertWorldCoordsToWorldPosition(Coords);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Coords")
	static FVector ConvertWorldCoordsToWorldPosition(const FCraftWorldCoords& WorldCoords)
	{
		return FVector(WorldCoords.X, WorldCoords.Y, WorldCoords.Z) * CraftWorldConstants::WORLD_BLOCK_SIZE;
	} 

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Coords")
	static FCraftWorldCoords ConvertWorldPositionToWorldCoords(const FVector& WorldPosition)
	{
		return FCraftWorldCoords::WorldPositionToWorldCoords(WorldPosition);
	} 
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Coords")
	static void ConvertWorldToRelativeCoords(const FCraftWorldCoords& WorldCoords, FCraftRelativeCoords& OutRelativeCoords, FCraftChunkCoords& OutChunkCoords) 
	{
		uint8 RelativeX, RelativeY;
		int32 ChunkX, ChunkY;
		
		ConvertWorldToRelativeCoord(WorldCoords.X, ChunkX, RelativeX);
		ConvertWorldToRelativeCoord(WorldCoords.Y, ChunkY, RelativeY);
		
		OutRelativeCoords = FCraftRelativeCoords(RelativeX, RelativeY, WorldCoords.Z);
		OutChunkCoords = FCraftChunkCoords(ChunkX, ChunkY);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Coords")
	static void ConvertWorldToRelativeCoord(const int32& WorldCoord, int32& OutChunkCoord, uint8& OutRelativeCoord) 
	{
		OutChunkCoord = FMathf::Floor(static_cast<float>(WorldCoord) / CraftWorldConstants::SECTION_DIMENSION);
		OutRelativeCoord = CraftMath::Modulo(WorldCoord, CraftWorldConstants::SECTION_DIMENSION);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Coords")
	static FCraftWorldCoords ConvertRelativeToWorldCoords(const FCraftChunkCoords& ChunkCoords, const FCraftRelativeCoords& RelativeCoords) 
	{
		return FCraftWorldCoords(ConvertRelativeToWorldCoord(ChunkCoords.X, RelativeCoords.X), ConvertRelativeToWorldCoord(ChunkCoords.Y, RelativeCoords.Y), RelativeCoords.Z);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Coords")
	static int32 ConvertRelativeToWorldCoord(const int32& ChunkCoords, const uint8& RelativeCoords) 
	{
		return ChunkCoords * CraftWorldConstants::SECTION_DIMENSION + RelativeCoords;
	}
};