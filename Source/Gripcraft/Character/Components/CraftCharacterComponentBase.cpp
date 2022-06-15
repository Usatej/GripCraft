// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftCharacterComponentBase.h"

#include "Gripcraft/CraftLogChannels.h"
#include "Gripcraft/Character/CraftCharacter.h"
#include "Gripcraft/World/CraftWorldSubsystem.h"

UCraftCharacterComponentBase::UCraftCharacterComponentBase()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

ACraftCharacter* UCraftCharacterComponentBase::GetCraftCharacterChecked() const
{
	return CastChecked<ACraftCharacter>(GetOuter(), ECastCheckedType::NullChecked);
}

TObjectPtr<UCraftWorldSubsystem> UCraftCharacterComponentBase::GetCraftWorldSubsystemChecked() const
{
	TObjectPtr<UCraftWorldSubsystem> Subsystem = GetWorld()->GetSubsystem<UCraftWorldSubsystem>();
	if(!Subsystem)
	{
		UE_LOG(LogCraft, Fatal, TEXT("UCraftCharacterComponentBase::GetCraftWorldSubsystemChecked - failed to get subsystem"));
	}
	return Subsystem;
}
