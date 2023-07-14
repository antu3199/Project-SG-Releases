#include "SGCharacterInventoryComponent.h"

#include "SGPlayerController.h"
#include "SGCharacterBase.h"
#include "SoulsGame/SGGameInstance.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/StaticSGData.h"
#include "SoulsGame/Subsystems/SGUISubsystem.h"
#include "SoulsGame/UI/SGHUDWidget.h"


FSGStaticRelicItem* FSGInventoryItem::GetStaticRelicDataFromTag(const UObject* WorldContextObject,
	const FGameplayTag& Tag)
{
	if (const USGGameInstance* GameInstance = Cast<USGGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
	{
		if (const UStaticSGData* StaticSGData = GameInstance->GetStaticData())
		{
			for (int32 i = 0; i < StaticSGData->RelicData->Relics.Num(); i++)
			{
				if (StaticSGData->RelicData->Relics[i].Tag == Tag)
				{
					return &StaticSGData->RelicData->Relics[i];
				}
			}
		}
	}

	return nullptr;
}

FGameplayTag FSGInventoryItem::GetTag() const
{
	if (Tag.IsValid())
	{
		return Tag;
	}

	return GetTagFromStaticData();
}

FGameplayTag FSGInventoryItem::GetTagFromStaticData() const
{
	if (StaticData == nullptr)
	{
		return FGameplayTag();
	}

	return StaticData->Tag;
}

USGCharacterInventoryComponent::USGCharacterInventoryComponent()
{
	
}

void USGCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	SetCurHealthPots(MaxHealthPots);
	ResetAbilityUses();
}

void USGCharacterInventoryComponent::SetCurHealthPots(int32 Pots)
{
	CurHealthPots = Pots;

	if (USGHUDWidget* Widget = GetHUD())
	{
		Widget->SetHealthPots(Pots);
	}
}

void USGCharacterInventoryComponent::ResetAbilityUses()
{
	TEMP_AbilityOneUses = InitialAbilityOneUses;
	TEMP_AbilityTwoUses =InitialAbilityTwoUses;

	if (USGHUDWidget* HUD = GetHUD())
	{
		HUD->RequestUpdateAbilityAmounts();
	}
}

void USGCharacterInventoryComponent::AddItem(FGameplayTag ItemTag, int32 Quantity)
{
	if (Quantity <= 0)
	{
		return;
	}

	if (FSGInventoryItem* ExistingItem = GetItemPtr(ItemTag))
	{
		ExistingItem->Quantity += Quantity;
		return;
	}

	FSGInventoryItem& ItemToAdd = Items.Add_GetRef(FSGInventoryItem());
	ItemToAdd.Tag = ItemTag;
	ItemToAdd.Quantity = Quantity;
	ItemToAdd.StaticData = FSGInventoryItem::GetStaticRelicDataFromTag(GetWorld(), ItemTag);

	check(ItemToAdd.StaticData != nullptr);
}

void USGCharacterInventoryComponent::RemoveItem(FGameplayTag ItemTag, int32 Quantity)
{
	int32 Index = -1;
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].GetTag() == ItemTag)
		{
			Index = i;
			break;
		}
	}

	if (Index == -1)
	{
		return;
	}

	FSGInventoryItem& Item = Items[Index];

	if (Quantity < Item.Quantity)
	{
		Item.Quantity -= Quantity;
	}
	else
	{
		Items.RemoveAt(Index);
	}
}

int32 USGCharacterInventoryComponent::GetNumItems(const FSGInventoryItem& Item)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].GetTag() == Item.GetTag())
		{
			return Items[i].Quantity;
		}
	}

	return 0;
}

FSGStaticInventoryItem USGCharacterInventoryComponent::GetStaticData(const FSGInventoryItem& Item,
	bool& OutResult) const
{
	if (Item.GetStaticData())
	{
		OutResult = true;
		return *Item.GetStaticData();
	}

	OutResult = false;
	return FSGStaticInventoryItem();
}

FSGStaticInventoryItem USGCharacterInventoryComponent::GetStaticDataByTag(const FGameplayTag& ItemTag,
	bool& OutResult)
{
	if (FSGInventoryItem* Item = GetItemPtr(ItemTag))
	{
		if (Item->GetStaticData())
		{
			OutResult = true;
			return *Item->GetStaticData();
		}
	}

	OutResult = false;
	return FSGStaticInventoryItem();
}

FSGStaticRelicItem USGCharacterInventoryComponent::GetStaticRelicDataByTag(FGameplayTag ItemTag, bool& OutResult)
{
	if (FSGInventoryItem* Item = GetItemPtr(ItemTag))
	{
		if (FSGStaticRelicItem* StaticRelicItem = static_cast<FSGStaticRelicItem*>(Item->GetStaticData()))
		{
			OutResult = true;
			return *StaticRelicItem;
		}
	}

	OutResult = false;
	return FSGStaticRelicItem();
}

FSGStaticRelicItem USGCharacterInventoryComponent::GetStaticRelicDataByAbilityTag(FGameplayTag AbilityTag, bool& OutResult)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (FSGStaticInventoryItem* InventoryItem = Items[i].GetStaticData())
		{
			if (InventoryItem->StaticDataType == ESGStaticDataType::Relic)
			{
				FSGStaticRelicItem* RelicItem = static_cast<FSGStaticRelicItem*>(InventoryItem);
				if (RelicItem->GetAbilityTag() == AbilityTag)
				{
					OutResult = true;
					return *RelicItem;
				}
			}
		}
	}

	OutResult = false;
	return FSGStaticRelicItem();
}

FSGInventoryItem* USGCharacterInventoryComponent::GetItemPtr(const FGameplayTag& Tag)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].GetTag() == Tag)
		{
			return &(Items[i]);
		}
	}

	return nullptr;
}

TArray<FSGInventoryItem> USGCharacterInventoryComponent::GetItemsMatchingTag(FGameplayTag Tag)
{
	TArray<FSGInventoryItem> Result;
	for (const FSGInventoryItem& Item : Items)
	{
		if (Item.Tag.MatchesTag(Tag))
		{
			Result.Add(Item);
		}
	}

	return Result;
}

FSGInventoryItem USGCharacterInventoryComponent::GetItemWithTag(FGameplayTag Tag)
{
	for (const FSGInventoryItem& Item : Items)
	{
		if (Item.Tag == Tag)
		{
			return Item;
		}
	}

	return FSGInventoryItem();
}

USGHUDWidget* USGCharacterInventoryComponent::GetHUD() const
{
	if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		if (USGHUDWidget* HUD = Subsystem->GetHUDInstance())
		{
			return HUD;
		}
	}

	return nullptr;
}
