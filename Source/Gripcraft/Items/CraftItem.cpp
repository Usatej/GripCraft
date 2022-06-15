#include "CraftItem.h"

UCraftItem::UCraftItem()
{
}

bool UCraftItem::HasTag(const FGameplayTag Tag) const
{
	return Tags.HasTag(Tag);
}

const FGameplayTagContainer& UCraftItem::GetTags() const
{
	return Tags;
}
