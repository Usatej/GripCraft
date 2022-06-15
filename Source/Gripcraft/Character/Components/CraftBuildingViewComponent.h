// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftCharacterComponentBase.h"
#include "CraftBuildingViewComponent.generated.h"

class UCraftItem;
struct FCraftWorldCoords;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRIPCRAFT_API UCraftBuildingViewComponent : public UCraftCharacterComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCraftBuildingViewComponent();
	
	virtual void PostInitProperties() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category="Craft|Building")
	void StartBuildingView();
	UFUNCTION(BlueprintCallable, Category="Craft|Building")
	void StopBuildingView();

	UFUNCTION(BlueprintCallable, Category="Craft|Building")
	bool GetLocationOfPreviewBlock(FCraftWorldCoords& OutCoords) const;

private:
	void DrawBoxAtPoitingLocation();

	UFUNCTION()
	void OnSelectedItemChanged(const UCraftItem* Item);
private:
	

	UPROPERTY(EditDefaultsOnly, Category="Settings")
	TSubclassOf<AActor> WireframeActorClass;

	UPROPERTY()
	TObjectPtr<AActor> WireframeActor;
	
	UPROPERTY()
	TObjectPtr<APlayerCameraManager> PlayerCameraManager; 
	
	bool bBuildingViewActive;
};
