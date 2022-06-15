// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftCrackDecalActor.h"

#include "Components/DecalComponent.h"
#include "Gripcraft/Utils/CraftMeshUtils.h"
#include "Gripcraft/World/CraftWorldCommon.h"


// Sets default values
ACraftCrackDecalActor::ACraftCrackDecalActor(): DecalOffset(-0.5f)
{
	PrimaryActorTick.bCanEverTick = false;

	TObjectPtr<USceneComponent> DefaultRootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(DefaultRootComponent);

	DecalComponents.Reserve(6);
	DecalComponents.Push(CreateDefaultSubobject<UDecalComponent>("RightDecal"));
	DecalComponents.Push(CreateDefaultSubobject<UDecalComponent>("LeftDecal"));
	DecalComponents.Push(CreateDefaultSubobject<UDecalComponent>("FrontDecal"));
	DecalComponents.Push(CreateDefaultSubobject<UDecalComponent>("BackDecal"));
	DecalComponents.Push(CreateDefaultSubobject<UDecalComponent>("TopDecal"));
	DecalComponents.Push(CreateDefaultSubobject<UDecalComponent>("BottomDecal"));

	for (auto& Decal : DecalComponents)
	{
		Decal->AttachToComponent(DefaultRootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	}
}

void ACraftCrackDecalActor::PostInitProperties()
{
	Super::PostInitProperties();

	if(IsValid(ParentMaterial))
	{
		CrackMaterialDynamic = UMaterialInstanceDynamic::Create(ParentMaterial, this, FName("Mat_Crack"));
		if (CrackTextures.Num() > 0)
		{
			CrackMaterialDynamic->SetTextureParameterValue(MaterialTextureParamName, CrackTextures[0]);
		}
	}

	for (int DirectionIndex = 0; DirectionIndex < 6; DirectionIndex++)
	{
		FVector DecalPosition(
			CraftMeshUtils::Directions[DirectionIndex] * (CraftWorldConstants::WORLD_HALF_BLOCK_SIZE));
		const float TmpOffset = FMathf::Sign(DecalPosition.X + DecalPosition.Y + DecalPosition.Z) >= 0 ? DecalOffset : -DecalOffset;
		if (FMathf::Abs(DecalPosition.X) > KINDA_SMALL_NUMBER)
		{
			DecalPosition.X += TmpOffset;
		}
		else if (FMathf::Abs(DecalPosition.Y) > KINDA_SMALL_NUMBER)
		{
			DecalPosition.Y += TmpOffset;
		}
		else if (FMathf::Abs(DecalPosition.Z) > KINDA_SMALL_NUMBER)
		{
			DecalPosition.Z += TmpOffset;
		}

		DecalPosition += FVector(CraftWorldConstants::WORLD_HALF_BLOCK_SIZE);

		DecalComponents[DirectionIndex]->DecalSize = FVector(FMathf::Abs(DecalOffset) + 1,CraftWorldConstants::WORLD_HALF_BLOCK_SIZE,CraftWorldConstants::WORLD_HALF_BLOCK_SIZE);
		DecalComponents[DirectionIndex]->SetRelativeLocation(DecalPosition);
		DecalComponents[DirectionIndex]->SetRelativeRotation(FVector(CraftMeshUtils::Directions[DirectionIndex]).Rotation());

		if(IsValid(CrackMaterialDynamic))
		{
			DecalComponents[DirectionIndex]->SetDecalMaterial(CrackMaterialDynamic);
		}
	}
}

void ACraftCrackDecalActor::SetNewLocation(const FVector& NewLocation, const bool& bResetTexture)
{
	SetActorLocation(NewLocation);

	if (bResetTexture && IsValid(CrackMaterialDynamic) && CrackTextures.Num() > 0)
	{
		CrackMaterialDynamic->SetTextureParameterValue(MaterialTextureParamName, CrackTextures[0]);
	}
}

void ACraftCrackDecalActor::UpdateCracks(const float& CracksRatio)
{
	const int Index = FMathf::Floor(FMathf::Clamp(CracksRatio, 0, 0.99) * 10);
	CrackMaterialDynamic->SetTextureParameterValue(MaterialTextureParamName, CrackTextures[Index]);
}

void ACraftCrackDecalActor::Show(const TArray<uint8>& VisibleDirectionIndexes)
{
	SetActorHiddenInGame(false);

	if(VisibleDirectionIndexes.Num() > 0)
	{
		RootComponent->SetVisibility(true, false);

		for (const auto& Decal : DecalComponents)
		{
			Decal->SetVisibility(false);
		}

		for (const auto& Direction : VisibleDirectionIndexes)
		{
			DecalComponents[Direction]->SetVisibility(true);
		}
	} else
	{
		RootComponent->SetVisibility(true, true);
	}
}

void ACraftCrackDecalActor::ShowBP(const TArray<uint8> VisibleDirectionIndexes)
{
	Show(VisibleDirectionIndexes);
}

void ACraftCrackDecalActor::Hide()
{
	SetActorHiddenInGame(true);
	RootComponent->SetVisibility(false, true);
}

// Called when the game starts or when spawned
void ACraftCrackDecalActor::BeginPlay()
{
	Super::BeginPlay();
}
