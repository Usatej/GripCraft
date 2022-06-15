#pragma once
#include <array>

struct FSize3D
{
	FSize3D(): X(0), Y(0), Z(0) {}
	FSize3D(const uint32& InX, const uint32& InY, const uint32& InZ): X(InX), Y(InY), Z(InZ) {}
	uint32 X, Y, Z;
};

template<typename T, uint32 SizeX, uint32 SizeY, uint32 SizeZ>
class TArray3D
{
public:
	void Empty() {
		Array.Empty();
	}
	
	void Init(TFunction<T()> Factory)
	{
		T Elem = Factory();
		Array.Init(Elem, SizeX * SizeY * SizeZ);
	}
	
	T& At(const uint32& X, const uint32& Y, const uint32& Z)
	{
		check(X <= SizeX && Y <= SizeY && Z <= SizeZ);
		return Array[GetIndex(X, Y, Z)];
	}

	const T& At(const uint32& X, const uint32& Y, const uint32& Z) const
	{
		check(X <= SizeX && Y <= SizeY && Z <= SizeZ);
		return Array[GetIndex(X, Y, Z)];
	}

	FSize3D Size() const
	{
		return FSize3D(SizeX, SizeY, SizeZ);
	}
	
private:
    uint32 GetIndex(const uint32& X, const uint32& Y, const uint32& Z) const
    {
	    return X + Y * SizeX + Z * SizeX * SizeY;
    }

    TArray<T> Array;
};