// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/CraftBuildingViewComponent.h"
#include "Components/CraftMiningComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "CraftCharacter.generated.h"


class UCraftInventoryComponent;
UCLASS()
class GRIPCRAFT_API ACraftCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACraftCharacter();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Character")
	int32 GetBuildingDistanceInWorldUnits() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Character")
	const UCraftBuildingViewComponent* GetBuildingViewComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Character")
	UCraftInventoryComponent* GetInventoryComponent() const;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	//~input events
	void OnPrimaryAction();
	void OnSecondaryAction();
	void MoveForward(float Value);
	void MoveRight(float Value);
protected:
	/*
	 * Building distance in num of blocks
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Craft")
	int32 BuildingDistance;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCraftBuildingViewComponent> BuildingViewComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCraftMiningComponent> MiningComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCraftInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SelectedItemMeshComponent;



public:
	UStaticMeshComponent* GetSelectedItemMesh() const { return SelectedItemMeshComponent; }
};
