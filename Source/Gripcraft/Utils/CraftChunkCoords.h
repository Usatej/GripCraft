#pragma once

#include "CoreMinimal.h"
#include "CraftMath.h"
#include "Gripcraft/World/CraftWorldCommon.h"
#include "CraftChunkCoords.generated.h"

USTRUCT(BlueprintType)
struct FCraftChunkCoords
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Craft|Coords")
	int32 X;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Craft|Coords")
	int32 Y;
	
	FCraftChunkCoords(): X(0), Y(0) {}
	FCraftChunkCoords(const int32& InX, const int32& InY): X(InX), Y(InY) {}
	FCraftChunkCoords(const FVector& InVector): X(WorldPositionToChunkCoord(InVector.X)), Y(WorldPositionToChunkCoord(InVector.Y)) {}
	FCraftChunkCoords(const FIntPoint& InPoint): FCraftChunkCoords(InPoint.X, InPoint.Y) {}
	FCraftChunkCoords(const FCraftChunkCoords& Other): FCraftChunkCoords(Other.X, Other.Y) {}

	FORCEINLINE FCraftChunkCoords operator+(const FCraftChunkCoords& Other) const
	{
		return FCraftChunkCoords(X + Other.X, Y + Other.Y);
	}
	
	FORCEINLINE FCraftChunkCoords operator-(const FCraftChunkCoords& Other) const
	{
		return FCraftChunkCoords(X - Other.X, Y - Other.Y);
	}
	
	FORCEINLINE FCraftChunkCoords operator+(const int32& Other) const
	{
		return FCraftChunkCoords(X + Other, Y + Other);
	}
	
	FORCEINLINE FCraftChunkCoords operator-(const int32& Other) const
	{
		return FCraftChunkCoords(X - Other, Y - Other);
	}
	
	FORCEINLINE bool operator==(const FCraftChunkCoords& Other) const
	{
		return Equals(Other);
	}

	FORCEINLINE bool Equals(const FCraftChunkCoords& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	FORCEINLINE bool operator !=(const FCraftChunkCoords& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE FVector ToWorldVector(const double& Z = 0.f) const
	{
		return FVector(ChunkCoordToWorldCoord(X), ChunkCoordToWorldCoord(Y), Z);
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("%d x %d"), X, Y);
	}

	//statics
	FORCEINLINE static int32 WorldPositionToChunkCoord(const double& WorldPosition)
	{
		return FMathf::Floor(WorldPosition / CraftWorldConstants::WORLD_CHUNK_SIZE);
	}

	FORCEINLINE static float Distance(const FCraftChunkCoords& A, const FCraftChunkCoords& B)
	{
		return FMathf::Sqrt(FMath::Square(A.X-B.X) + FMath::Square(A.Y-B.Y));
	}

	FORCEINLINE static int32 ChunkCoordToWorldCoord(const double& ChunkCoord)
	{
		return ChunkCoord * CraftWorldConstants::WORLD_CHUNK_SIZE;
	}
};

FORCEINLINE uint32 GetTypeHash(const FCraftChunkCoords& Coords)
{
	return HashCombine(GetTypeHash(Coords.X), GetTypeHash(Coords.Y));
}

UCLASS()
class UCraftChunkCoordsUtils: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Coords")
	static FCraftChunkCoords WorldPositionToChunkCoords(const FVector& Position)
	{
		return FCraftChunkCoords(FCraftChunkCoords::WorldPositionToChunkCoord(Position.X), FCraftChunkCoords::WorldPositionToChunkCoord(Position.Y));
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Coords")
	static int32 WorldPositionToChunkCoord(const double& WorldPosition)
	{
		return FCraftChunkCoords::WorldPositionToChunkCoord(WorldPosition);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Coords")
	static float Distance(const FCraftChunkCoords& A, const FCraftChunkCoords& B)
	{
		return FCraftChunkCoords::Distance(A, B);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Coords")
	static int32 ChunkCoordToWorldCoord(const double& ChunkCoord)
	{
		return FCraftChunkCoords::ChunkCoordToWorldCoord(ChunkCoord);
	}
};
