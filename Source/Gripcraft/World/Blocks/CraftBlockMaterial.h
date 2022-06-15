#pragma once
#include "CoreMinimal.h"
#include "CraftBlockMaterial.generated.h"

UENUM(BlueprintType)
enum class ECraftBlockMaterial: uint8
{
	Air,
	Dirt,
	Stone,
	Bedrock,
	Wood,
	Leaf
};
