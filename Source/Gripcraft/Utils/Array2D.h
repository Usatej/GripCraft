#pragma once
#include <array>

struct FSize2D
{
	FSize2D(): X(0), Y(0) {}
	FSize2D(const uint32& InX, const uint32& InY): X(InX), Y(InY) {}
	uint32 X, Y;
};

template<typename T, int SizeX, int SizeY>
class TArray2D
{
private:
	std::array<std::array<T, SizeY>, SizeX> Array;
public:
	T& At(const uint32& X, const uint32& Y)
	{
		check(X <= SizeX && Y <= SizeY);
		return Array[X][Y];
	}

	const T& At(const uint32& X, const uint32& Y) const
	{
		check(X <= SizeX && Y <= SizeY );
		return Array[X][Y];
	}

	FSize2D Size() const
	{
		return FSize2D(SizeX, SizeY);
	}
};