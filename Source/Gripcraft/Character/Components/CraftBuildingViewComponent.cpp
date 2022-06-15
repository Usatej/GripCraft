
#include "CraftBuildingViewComponent.h"

#include "CraftInventoryComponent.h"
#include "Gripcraft/Character/CraftCharacter.h"
#include "Gripcraft/Items/CraftBuildingItem.h"
#include "Gripcraft/Utils/CraftCoords.h"
#include "Gripcraft/Utils/CraftTraceUtils.h"
#include "Kismet/GameplayStatics.h"


UCraftBuildingViewComponent::UCraftBuildingViewComponent():
	bBuildingViewActive(false)
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UCraftBuildingViewComponent::PostInitProperties()
{
	Super::PostInitProperties();	
}

void UCraftBuildingViewComponent::BeginPlay()
{
	Super::BeginPlay();

	WireframeActor = GetWorld()->SpawnActor(WireframeActorClass);

	//check if player already has selected item
	UCraftItem* SelectedItem = GetCraftCharacterChecked()->GetInventoryComponent()->GetSelectedItem();
	if(SelectedItem)
	{
		OnSelectedItemChanged(SelectedItem);
	}

	//register for changes
	GetCraftCharacterChecked()->GetInventoryComponent()->OnSelectedItemChanged.AddDynamic(this, &UCraftBuildingViewComponent::OnSelectedItemChanged);
}

void UCraftBuildingViewComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetCraftCharacterChecked()->GetInventoryComponent()->OnSelectedItemChanged.RemoveDynamic(this, &UCraftBuildingViewComponent::OnSelectedItemChanged);
}

void UCraftBuildingViewComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bBuildingViewActive)
	{
		DrawBoxAtPoitingLocation();
	}
}

void UCraftBuildingViewComponent::StartBuildingView()
{
	bBuildingViewActive = true;

	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	check(PlayerCameraManager);
}

void UCraftBuildingViewComponent::StopBuildingView()
{
	bBuildingViewActive = false;
	if(WireframeActor)
	{
		WireframeActor->SetActorHiddenInGame(true);
	}
}

 bool UCraftBuildingViewComponent::GetLocationOfPreviewBlock(FCraftWorldCoords& OutCoords) const
{
	if(!WireframeActor)
	{
		return false;
	}

	OutCoords =  UCraftCoordsUtils::ConvertWorldPositionToWorldCoords(WireframeActor->GetActorLocation());	
	return true;
}

void UCraftBuildingViewComponent::DrawBoxAtPoitingLocation()
{
	FHitResult HitResult;
	if(WireframeActor && UCraftTraceUtils::LineTraceFromCamera(PlayerCameraManager, GetCraftCharacterChecked()->GetBuildingDistanceInWorldUnits(), HitResult))
	{
		FVector BlockWorldLocation = UCraftCoordsUtils::AlignWorldPosition(HitResult.Location + HitResult.Normal);
		WireframeActor->SetActorLocation(BlockWorldLocation);
		WireframeActor->SetActorHiddenInGame(false);
	} else
	{
		WireframeActor->SetActorHiddenInGame(true);
	}
}

void UCraftBuildingViewComponent::OnSelectedItemChanged(const UCraftItem* Item)
{
	if(Item && Item->HasTag(CraftTag_Item_Usable_Building))
	{
		StartBuildingView();
	} else
	{
		StopBuildingView();
	}
}

