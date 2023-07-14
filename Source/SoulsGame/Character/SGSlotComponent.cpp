#include "SGSlotComponent.h"

#include "NativeGameplayTags.h"
#include "SGCharacterInventoryComponent.h"
#include "SoulsGame/StaticSGData.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityComponent.h"

USGSlotComponent::USGSlotComponent()
{
	Slots.SetNum(SG_NUM_SLOTS);
	for (int32 Index = 0; Index < SG_NUM_SLOTS; Index++)
	{
		Slots[Index].SlotIndex = Index;
	}
}

void USGSlotComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->OnWorldBeginPlay.AddUObject(this, &USGSlotComponent::PostBeginPlay);
}

void USGSlotComponent::AssignSlotSpec(int32 SlotIndex, const FGameplayTag& InventoryItemTag)
{
	check(SlotIndex < SG_NUM_SLOTS);

	UnAssignSlot(SlotIndex);

	// If it exists in another slot, unassign it
	if (const int32 ExistingSlot = GetSlotIndexByItemTag(InventoryItemTag); ExistingSlot != INDEX_NONE)
	{
		UnAssignSlot(ExistingSlot);
	}
	
	if (USGCharacterInventoryComponent* InventoryComponent = GetOwner()->FindComponentByClass<USGCharacterInventoryComponent>())
	{
		if (FSGInventoryItem* Item = InventoryComponent->GetItemPtr(InventoryItemTag))
		{
			FSGSlot& Slot = Slots[SlotIndex];
			Slot.InventoryItem = Item;
			Slot.bInUse = true;
			Slot.ItemTag = Item->GetTag();

			if (FSGStaticRelicItem* RelicData = static_cast<FSGStaticRelicItem*>(Item->GetStaticData()))
			{
				const FGameplayTag AbilityTag = RelicData->GetAbilityTag();
				if (USGAbilityComponent* AbilityComponent = GetOwner()->FindComponentByClass<USGAbilityComponent>())
				{
					if (!AbilityComponent->HasGivenAbility(AbilityTag))
					{
						int32 TEMP_Level = 1;
						AbilityComponent->GiveAbility(RelicData->AbilityClass, TEMP_Level);
					}
					
					FSGAbilitySpec* Spec = AbilityComponent->GetGivenAbilitySpecByTag(AbilityTag);
					Slot.AbilitySpec = Spec;
				}
			}

			check(Slot.InventoryItem);
			check(Slot.AbilitySpec);
		}
	}
}

FSGSlot USGSlotComponent::GetSlot(int32 Index) const
{
	check(Index < SG_NUM_SLOTS);
	return Slots[Index];
}

void USGSlotComponent::UnAssignSlot(int32 SlotIndex)
{
	FSGSlot& Slot = Slots[SlotIndex];
	Slot.InventoryItem = nullptr;
	Slot.AbilitySpec = nullptr;
	Slot.bInUse = false;
	Slot.ItemTag = FGameplayTag();
	
}

FSGSlot* USGSlotComponent::GetSlotPtr(int32 Index)
{
	check(Index < SG_NUM_SLOTS);
	return &(Slots[Index]);
}

int32 USGSlotComponent::GetSlotIndexByItemTag(const FGameplayTag& InventoryItemTag) const
{
	for (int i = 0; i < Slots.Num(); i++)
	{
		if (Slots[i].ItemTag == InventoryItemTag)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

void USGSlotComponent::UseAbilityOnSlot(int32 Index) const
{
	check(Index < SG_NUM_SLOTS);

	FSGAbilitySpec* Spec = Slots[Index].AbilitySpec;
	if (Spec == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to activate ability on slot with no spec!"))
		return;
	}

	if (USGAbilityComponent* AbilityComponent = GetOwner()->FindComponentByClass<USGAbilityComponent>())
	{
		AbilityComponent->RequestActivateAbilityBySpec(Spec);
	}
}

float USGSlotComponent::GetCurrentCooldownTimeOnSlot(int32 Index) const
{
	check(Index < SG_NUM_SLOTS);

	const FSGAbilitySpec* Spec = Slots[Index].AbilitySpec;
	if (Spec == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to activate ability on slot with no spec!"))
		return 0.0f;
	}

	if (const USGAbilityComponent* AbilityComponent = GetOwner()->FindComponentByClass<USGAbilityComponent>())
	{
		return AbilityComponent->GetCurrentCooldownTime(Spec->GetTag());
	}

	return 0.0f;
}

float USGSlotComponent::GetCurrentCooldownPercentOnSlot(int32 Index) const
{
	check(Index < SG_NUM_SLOTS);

	const FSGAbilitySpec* Spec = Slots[Index].AbilitySpec;
	if (Spec == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to activate ability on slot with no spec!"))
		return 0.0f;
	}

	if (const USGAbilityComponent* AbilityComponent = GetOwner()->FindComponentByClass<USGAbilityComponent>())
	{
		return AbilityComponent->GetCurrentCooldownPercent(Spec->GetTag());
	}

	return 0.0f;
}

TArray<FSGSlot> USGSlotComponent::GetAllSlots() const
{
	return Slots;
}

void USGSlotComponent::PostBeginPlay()
{
	GetWorld()->OnWorldBeginPlay.RemoveAll(this);

	UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_Relic_DamageRelic, "Items.Relics.DamageRelic")
	const FGameplayTag DamageRelicTag = Tag_Relic_DamageRelic.GetTag();

	UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_Relic_DefenceRelic, "Items.Relics.DefenceRelic")
	const FGameplayTag DefenceRelicTag = Tag_Relic_DefenceRelic.GetTag();

	if (USGCharacterInventoryComponent* InventoryComponent = GetOwner()->FindComponentByClass<USGCharacterInventoryComponent>())
	{
		InventoryComponent->AddItem(DamageRelicTag, 1);
		AssignSlotSpec(0, DamageRelicTag);

		InventoryComponent->AddItem(DefenceRelicTag, 1);
	}
	
	// TODO: Remove me:
	
	/*
	if (USGAbilityComponent* AbilityComponent = GetOwner()->FindComponentByClass<USGAbilityComponent>())
	{
		FSGAbilitySpec* DamageSpec = AbilityComponent->GetGivenAbilitySpecByTag(DamageRelicTag);
		check(DamageSpec);
		AssignSlotSpec(0, DamageSpec);

		FSGAbilitySpec* DefenceSpec = AbilityComponent->GetGivenAbilitySpecByTag(DefenceRelicTag);
		check(DefenceSpec);
		AssignSlotSpec(1, DefenceSpec);
	}
	*/
}
