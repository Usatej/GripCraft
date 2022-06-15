#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "CraftItem.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UCraftItem : public UObject
{
	GENERATED_BODY()

public:
	UCraftItem();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Items")
	bool HasTag(const FGameplayTag Tag) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Items")
	const FGameplayTagContainer& GetTags() const;
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Craft")
	FGameplayTagContainer Tags;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Craft")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Craft")
	TObjectPtr<UStaticMesh> Mesh;
};
