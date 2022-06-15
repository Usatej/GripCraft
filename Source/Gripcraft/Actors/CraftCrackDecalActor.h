// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CraftCrackDecalActor.generated.h"

UCLASS()
class GRIPCRAFT_API ACraftCrackDecalActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACraftCrackDecalActor();

	virtual void PostInitProperties() override;

	void SetNewLocation(const FVector& NewLocation, const bool& bResetTexture = true);
	void UpdateCracks(const float& CracksRatio);
	
	void Show(const TArray<uint8>& VisibleDirectionIndexes = TArray<uint8>());
	UFUNCTION(BlueprintCallable, Category="Craft", meta=(DisplayName="Show"))
	void ShowBP(const TArray<uint8> VisibleDirectionIndexes);
	UFUNCTION(BlueprintCallable, Category="Craft")
	void Hide();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Craft")
	float DecalOffset;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Craft")
	TArray<TObjectPtr<UTexture2D>> CrackTextures;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Craft")
	TObjectPtr<UMaterialInterface> ParentMaterial;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Craft")
	FName MaterialTextureParamName;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category="Craft")
	TArray<TObjectPtr<UDecalComponent>> DecalComponents;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category="Craft")
	TObjectPtr<UMaterialInstanceDynamic> CrackMaterialDynamic;
};
