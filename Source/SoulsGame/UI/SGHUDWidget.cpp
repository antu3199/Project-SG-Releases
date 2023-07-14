#include "SGHUDWidget.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/SGGameModeBase.h"
#include "SoulsGame/Character/SGPlayerController.h"
#include "SoulsGame/Character/SGCharacterInventoryComponent.h"

USGHUDWidget::USGHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USGHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void USGHUDWidget::InitializeHUD()
{

}

void USGHUDWidget::SetShouldShowStats(bool Set)
{
	bShouldShowStats = Set;
}

float USGHUDWidget::GetCurrentDifficulty()
{
	const UWorld * World = GetWorld();
	if (!World)
	{
		return 1.0f;
	}

	if (const ASGGameModeBase* GameMode = Cast<ASGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		return GameMode->GetCurrentDifficulty();
	}

	return 1.0f;
	
}

void USGHUDWidget::SetBossActor(ASGCharacterBase* Actor)
{
	SetBossActor_BP(Actor);
	OnBossCharacterSet.Broadcast(Actor);
}

void USGHUDWidget::UnsetBossActor()
{
	UnsetBossActor_BP();
}

void USGHUDWidget::SetBossHealthPercent(float Percentage)
{
	SetBossHealthPercent_BP(Percentage);
}

void USGHUDWidget::UpdateBossHealth()
{
	if (ASGPlayerController* Controller = Cast<ASGPlayerController>(GetOwningPlayer()))
	{
		if (Controller->GetBossActorPtr().IsValid())
		{
			const float HealthPercent = Controller->GetBossActorPtr()->GetHealthPercent();
			SetBossHealthPercent(HealthPercent);
		}
	}
}

void USGHUDWidget::SetHealthPots(int32 Amount)
{
	SetHealthPots_BP(Amount);
}

void USGHUDWidget::AddInteract(USGInteractEntryData* Data)
{
	OnAddInteract(Data);
}

void USGHUDWidget::RemoveInteract(AActor* Owner)
{
	OnRemoveInteract(Owner);
}

void USGHUDWidget::RequestUpdateAbilityAmounts()
{
	if (ASGPlayerController* Controller = Cast<ASGPlayerController>(GetOwningPlayer()))
	{
		if (ASGCharacterBase* Character = Cast<ASGCharacterBase>(Controller->GetPawnCharacter()))
		{
			UpdateAbilityAmounts(Character);
		}
	}
}

void USGHUDWidget::UpdateAbilityAmounts(ASGCharacterBase* OwnerCharacter)
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

