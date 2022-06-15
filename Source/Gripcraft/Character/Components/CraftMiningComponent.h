#pragma once

#include "CoreMinimal.h"
#include "CraftCharacterComponentBase.h"
#include "Components/ActorComponent.h"
#include "Gripcraft/Actors/CraftCrackDecalActor.h"
#include "Gripcraft/Utils/CraftCoords.h"
#include "Gripcraft/World/Blocks/CraftBlock.h"
#include "CraftMiningComponent.generated.h"


class FCraftBlock;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRIPCRAFT_API UCraftMiningComponent : public UCraftCharacterComponentBase
{
	GENERATED_BODY()

public:
	UCraftMiningComponent();

	UFUNCTION(BlueprintCallable, Category="Craft|Mining")
	void TryHitBlockInFrontOf();

	UFUNCTION(BlueprintCallable, Category="Craft|Mining")
	void HitBlock(const FCraftWorldCoords& BlockCoords);

	void ResetMiningState();

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Mining")
	ACraftCrackDecalActor* GetCrackDecalActorChecked() const;

	bool IsBlockFinished() const;
	void UpdateCracks();
	void MineBlock();
protected:
	UPROPERTY()
	mutable TObjectPtr<ACraftCrackDecalActor> CrackDecalActor;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Craft|Mining")
	TSubclassOf<ACraftCrackDecalActor> CrackDecalActorClass;

	//~ MineBlock
	struct FMiningState
	{
		float Damage;
		TSharedPtr<const FCraftBlock> Block;
		FCraftWorldCoords BlockCoords;
	};

	FMiningState MiningState;
	//~ MineBlock
};
