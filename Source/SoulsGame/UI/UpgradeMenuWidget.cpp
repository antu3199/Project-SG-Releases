#include "UpgradeMenuWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SoulsGame/Character/MyPlayerController.h"
#include "SoulsGame/Character/SHumanoidPlayerCharacter.h"

UUpgradeMenuWidget::UUpgradeMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UUpgradeMenuWidget::SelectOptionToCpp(const FUpgradeOption& Option)
{
	// Do something with option!
	if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetOwningPlayer()))
	{
		if (ASHumanoidPlayerCharacter* Character = Cast<ASHumanoidPlayerCharacter>(PlayerController->GetPawnCharacter()))
		{
			if (USGUpgradeController* Controller = USGUpgradeController::GetSingleton(this))
			{
				Controller->ApplyUpgradeToCharacter(Option, Character);
			}
			return;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("[UUpgradeMenuWidget::SelectOptionToCpp] Unable to get character!"));
}

void UUpgradeMenuWidget::WidgetCloseToCpp()
{
	UGameplayStatics::SetGamePaused(this, false);
	RemoveFromParent();
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer());
}
