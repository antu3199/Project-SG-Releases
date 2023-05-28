#include "HUDWidget.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/MyGameModeBase.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"
#include "SoulsGame/Character/MyPlayerController.h"
#include "SoulsGame/Character/SGCharacterInventoryComponent.h"

UHUDWidget::UHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UHUDWidget::InitializeHUD(UMyAttributeSet* AttributeSet)
{
	PlayerAttributeSet = AttributeSet;
}

void UHUDWidget::SetShouldShowStats(bool Set)
{
	bShouldShowStats = Set;
}

float UHUDWidget::GetCurrentDifficulty()
{
	const UWorld * World = GetWorld();
	if (!World)
	{
		return 1.0f;
	}

	if (const AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		return GameMode->GetCurrentDifficulty();
	}

	return 1.0f;
	
}

void UHUDWidget::SetBossActor(ASCharacterBase* Actor)
{
	SetBossActor_BP(Actor);
	OnBossCharacterSet.Broadcast(Actor);
}

void UHUDWidget::UnsetBossActor()
{
	UnsetBossActor_BP();
}

void UHUDWidget::SetBossHealthPercent(float Percentage)
{
	SetBossHealthPercent_BP(Percentage);
}

void UHUDWidget::UpdateBossHealth()
{
	if (AMyPlayerController* Controller = Cast<AMyPlayerController>(GetOwningPlayer()))
	{
		if (Controller->GetBossActorPtr().IsValid())
		{
			const float HealthPercent = Controller->GetBossActorPtr()->GetHealthPercent();
			SetBossHealthPercent(HealthPercent);
		}
	}
}

void UHUDWidget::SetHealthPots(int32 Amount)
{
	SetHealthPots_BP(Amount);
}

void UHUDWidget::AddInteract(UInteractEntryData* Data)
{
	OnAddInteract(Data);
}

void UHUDWidget::RemoveInteract(AActor* Owner)
{
	OnRemoveInteract(Owner);
}

void UHUDWidget::RequestUpdateAbilityAmounts()
{
	if (AMyPlayerController* Controller = Cast<AMyPlayerController>(GetOwningPlayer()))
	{
		if (ASCharacterBase* Character = Cast<ASCharacterBase>(Controller->GetPawnCharacter()))
		{
			UpdateAbilityAmounts(Character);
		}
	}
}

void UHUDWidget::UpdateAbilityAmounts(ASCharacterBase* OwnerCharacter)
{
	if (OwnerCharacter)
	{
		if (USGCharacterInventoryComponent* InventoryComponent = OwnerCharacter->FindComponentByClass<USGCharacterInventoryComponent>())
		{
			int32 AbilityOne_CurrentAmount = InventoryComponent->TEMP_AbilityOneUses;
			int32 AbilityTwo_CurrentAmount = InventoryComponent->TEMP_AbilityTwoUses;

			UpdateAbilityAmounts_BP(OwnerCharacter, AbilityOne_CurrentAmount, AbilityTwo_CurrentAmount);
		}
	}
}

