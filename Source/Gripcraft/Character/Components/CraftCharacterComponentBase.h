// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftCharacterComponentBase.generated.h"

class UCraftWorldSubsystem;
class ACraftCharacter;
class UInventoryComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Within=CraftCharacter)
class GRIPCRAFT_API UCraftCharacterComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UCraftCharacterComponentBase();
	
protected:
	UFUNCTION(BlueprintCallable, Category="Craft|Character")
	ACraftCharacter* GetCraftCharacterChecked() const;

	TObjectPtr<UCraftWorldSubsystem> GetCraftWorldSubsystemChecked() const;
};
