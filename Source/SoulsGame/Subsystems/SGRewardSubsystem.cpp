#include "SGRewardSubsystem.h"

#include "NativeGameplayTags.h"
#include "SoulsGame/Character/SGCharacterBase.h"

FSGRewardBlueprint::~FSGRewardBlueprint()
{
}

FSGReward FSGRewardBlueprint::CreateRewardInstance() const
{
	FSGReward RewardInstance;
	RewardInstance.RewardType = RewardType;
	int32 Value = 1;

	int32 Min = QuantityMin;
	if (QuantityMinCurve != nullptr)
	{
		Min = QuantityMinCurve->GetFloatValue(Level);
	}

	int32 Max = QuantityMax;
	if (QuantityMaxCurve != nullptr)
	{
		Max = QuantityMaxCurve->GetFloatValue(Level);
	}

	RewardInstance.Value = FMath::RandRange(Min, Max);
	
	return RewardInstance;
}

void USGRewardSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USGRewardSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USGRewardSubsystem::ApplyRewardBlueprint(FSGRewardBlueprint RewardBlueprint, ASGCharacterBase* Character)
{
	ApplyRewardPtr(&RewardBlueprint, Character);
}

void USGRewardSubsystem::ApplyReward(FSGReward Reward, ASGCharacterBase* Character)
{
	ApplyRewardPtr(&Reward, Character);
}

void USGRewardSubsystem::ApplyRewardData(TObjectPtr<class USGRewardData> RewardData, ASGCharacterBase* Character)
{
	if (!RewardData)
	{
		return;
	}
	
	for (const FSGRewardBlueprint& RewardBlueprint : RewardData->Rewards)
	{
		ApplyRewardPtr(&RewardBlueprint, Character);
	}
	
	OnRewardDataApplied.Broadcast(RewardData->OptionalRewardIdentifierTag);
}

void USGRewardSubsystem::ApplyRewardPtr(const FSGRewardBlueprint* RewardBlueprint, ASGCharacterBase* Character)
{
	if (RewardBlueprint == nullptr || Character == nullptr)
	{
		return;
	}

	FSGReward Reward = RewardBlueprint->CreateRewardInstance();
	ApplyRewardPtr(&Reward, Character);
}

void USGRewardSubsystem::ApplyRewardPtr(const FSGReward* Reward, ASGCharacterBase* Character)
{
	if (!Reward || !Character)
	{
		return;
	}

	bool bIsPlayerCharacter = Character->IsPlayerControlled();
	UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_Reward_XP, "Reward.XP")

	if (Reward->RewardType == Tag_Reward_XP.GetTag())
	{
		if (USGStatComponent* StatComponent = Character->FindComponentByClass<USGStatComponent>())
		{
			const FGameplayTag XPTag = USGStatComponent::GetXPTag();
			FSGStatModifier StatModifier(XPTag, Reward->Value);
			StatModifier.ModiferType = ESGStatModifierType::Additive;
			StatModifier.EffectDurationType = ESGEffectDurationType::Instantaneous;
			
			StatComponent->AddStatModifier(XPTag, StatModifier);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid reward type: %s"), *Reward->RewardType.ToString());
	}

	OnRewardApplied.Broadcast(*Reward);
}
