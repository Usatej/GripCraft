#pragma once

namespace CraftMath
{
	template<typename T>
	static T  Modulo(const T& A, const T& B)
	{
		return (A % B + B) % B; 
	}

	template<typename T>
	static T Sign(const T& A)
	{
		return A >= 0 ? 1 : -1; 
	}
}