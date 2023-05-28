#include "SGCharacterInventoryComponent.h"

#include "MyPlayerController.h"
#include "SCharacterBase.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Subsystems/UISubsystem.h"
#include "SoulsGame/UI/HUDWidget.h"

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

	if (UHUDWidget* Widget = GetHUD())
	{
		Widget->SetHealthPots(Pots);
	}
}

void USGCharacterInventoryComponent::ResetAbilityUses()
{
	TEMP_AbilityOneUses = InitialAbilityOneUses;
	TEMP_AbilityTwoUses =InitialAbilityTwoUses;

	if (UHUDWidget* HUD = GetHUD())
	{
		HUD->RequestUpdateAbilityAmounts();
	}
}

UHUDWidget* USGCharacterInventoryComponent::GetHUD() const
{
	if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		if (UHUDWidget* HUD = Subsystem->GetHUDInstance())
		{
			return HUD;
		}
	}

	return nullptr;
}
