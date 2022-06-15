#include "CraftInventoryComponent.h"

#include "Gripcraft/CraftLogChannels.h"


UCraftInventoryComponent::UCraftInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

const TArray<UCraftItem*>& UCraftInventoryComponent::GetItems() const
{
	return Items;
}

void UCraftInventoryComponent::AddItem(UCraftItem* Item)
{
	Items.Add(Item);
	OnItemAdded.Broadcast(Item);
}

void UCraftInventoryComponent::SelectItem(UCraftItem* Item)
{
	if(SelectedItem != Item)
	{
		if(Items.Contains(Item))
		{
			SelectedItem = Item;
			OnSelectedItemChanged.Broadcast(SelectedItem);
		} else
		{
			UE_LOG(LogCraft, Warning, TEXT("It is not possible to select an item that is not in the inventory."));
		}
	}
}

void UCraftInventoryComponent::SelectItemByIndex(const int32 ItemIndex)
{
	if(ItemIndex >= 0 && ItemIndex < Items.Num())
	{
		if(SelectedItem != Items[ItemIndex])
		{
			SelectedItem = Items[ItemIndex];
			OnSelectedItemChanged.Broadcast(SelectedItem);
		}
	} else
	{
		UE_LOG(LogCraft, Warning, TEXT("It is not possible to select an item. Provided index is out of bounds."));
	}
}

UCraftItem* UCraftInventoryComponent::GetSelectedItem() const
{
	return SelectedItem;
}

int32 UCraftInventoryComponent::GetSelectedItemIndex() const
{
	return Items.Find(SelectedItem);
}

void UCraftInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for(const auto& ItemClass: StartingItems)
	{
		TObjectPtr<UCraftItem> Item = NewObject<UCraftItem>(this, ItemClass);
		AddItem(Item);
	}
}

