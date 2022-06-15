// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftCharacterComponentBase.h"
#include "Components/ActorComponent.h"
#include "Gripcraft/Items/CraftItem.h"
#include "CraftInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemEventDelegate, const UCraftItem*, Item);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRIPCRAFT_API UCraftInventoryComponent : public UCraftCharacterComponentBase
{
	GENERATED_BODY()

public:
	UCraftInventoryComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Items")
	const TArray<UCraftItem*>& GetItems() const;

	UFUNCTION(BlueprintCallable, Category="Craft|Items")
	void AddItem(UCraftItem* Item);

	UFUNCTION(BlueprintCallable, Category="Craft|Items")
	void SelectItem(UCraftItem* Item);
	
	UFUNCTION(BlueprintCallable, Category="Craft|Items")
	void SelectItemByIndex(const int32 ItemIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Items")
	UCraftItem* GetSelectedItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Craft|Items")
	int32 GetSelectedItemIndex() const;

	//~ Events
	UPROPERTY(BlueprintAssignable, Category="Craft|Items")
	FItemEventDelegate OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category="Craft|Items")
	FItemEventDelegate OnSelectedItemChanged;
	//~ Events

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Craft")
	TArray<TSubclassOf<UCraftItem>> StartingItems;
	
	UPROPERTY()
	TObjectPtr<UCraftItem> SelectedItem;
	
	UPROPERTY()
	TArray<TObjectPtr<UCraftItem>> Items;
};
