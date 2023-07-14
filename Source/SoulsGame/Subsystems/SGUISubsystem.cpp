#include "SGUISubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SoulsGame/UI/SGHUDWidget.h"

#include "SoulsGame/SGGameInstance.h"
#include "SoulsGame/StaticSGData.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/UI/SGFadeInOutWidget.h"
#include "SoulsGame/UI/SGNarrativeWidget.h"


void USGUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

void USGUISubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USGUISubsystem::InitializeUIForPlayer(APlayerController* InController, bool bInitializeGameUI)
{
	if (!InController)
	{
		return;
	}
	
	Controller = MakeWeakObjectPtr(InController);

	const USGUISubsystemSettings* Settings = GetDefault<USGUISubsystemSettings>();

	if (bInitializeGameUI)
	{
		if (Settings->HUDClass)
		{
			HUDInstance = Cast<USGHUDWidget>(CreateWidget(InController, Settings->HUDClass));
		}
	}
}

void USGUISubsystem::OnFadeIn()
{
	bFadeInRequested = false;
	OnUIFadeInCompleted.Broadcast();
}

void USGUISubsystem::OnFadeOut()
{
	bFadeOutRequested = false;
	if (ActiveFadeInOutWidget)
	{
		ActiveFadeInOutWidget->RemoveFromParent();
	}
	
	OnUIFadeOutCompleted.Broadcast();
}

UUserWidget* USGUISubsystem::AddArbitraryUIToViewport(TSubclassOf<UUserWidget> WidgetClass, bool bFocusWidget, bool bAddToList)
{
	if (Controller.IsValid())
	{
		// Check if exists first
		UUserWidget* Widget = GetActiveWidgetByClass(WidgetClass);

		if (Widget == nullptr)
		{
			Widget = Cast<UUserWidget>(CreateWidget(Controller.Get(), WidgetClass));
		}
		
		Widget->AddToViewport();

		if (bFocusWidget)
		{
			FocusWidget(Widget);
		}

		if (bAddToList)
		{
			ActiveWidgets.Add(Widget);
		}
		
		return Widget;
	}

	return nullptr;
}

void USGUISubsystem::RemoveArbitraryUIFromViewport(UUserWidget* Widget, bool bUnFocusWidget, bool bRemoveFromList)
{
	if (!Widget)
	{
		return;
	}

	Widget->RemoveFromParent();

	if (bUnFocusWidget)
	{
		UnFocusWidget();
	}

	if (bRemoveFromList)
	{
		ActiveWidgets.Remove(Widget);
		Widget->Destruct();
	}
}

void USGUISubsystem::AddHUDToViewport()
{
	if (HUDInstance && Controller.IsValid())
	{
		if (const ASGCharacterBase* Character = Cast<ASGCharacterBase>(Controller->GetCharacter()))
		{
			HUDInstance->InitializeHUD();
		}

		HUDInstance->AddToViewport();
		//HUDInstance->SetShouldShowStats(false);
		//HUDInstance->UpdateHUDStatsFromCpp();
	}
}

void USGUISubsystem::RemoveHUDFromViewport()
{
	if (HUDInstance)
	{
		HUDInstance->RemoveFromParent();
	}
}

void USGUISubsystem::FocusWidget(UUserWidget* Widget)
{
	if (Controller.IsValid())
	{
		Controller->SetIgnoreMoveInput(true);
		Controller->SetIgnoreLookInput(true);
		Controller->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(Controller.Get(), Widget, EMouseLockMode::DoNotLock);
	}
}

void USGUISubsystem::UnFocusWidget()
{
	Controller->SetIgnoreMoveInput(false);
	Controller->SetIgnoreLookInput(false);
	Controller->SetShowMouseCursor(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(Controller.Get());
}

UUserWidget* USGUISubsystem::GetActiveWidgetByClass(TSubclassOf<UUserWidget> WidgetClass)
{
	for (UUserWidget* Widget : ActiveWidgets)
	{
		if (Widget && Widget->GetClass() == WidgetClass)
		{
			return Widget;
		}
	}

	return nullptr;
}

USGFadeInOutWidget* USGUISubsystem::InitializeFadeInOut(float StartOpacity)
{
	// Don't use Controller yet because it may not be initialized
	if (!ActiveFadeInOutWidget)
	{
		if (Controller.IsValid())
		{
			const USGUISubsystemSettings* Settings = GetDefault<USGUISubsystemSettings>();
			if (Settings->FadeInOutClass)
			{
				ActiveFadeInOutWidget = Cast<USGFadeInOutWidget>(CreateWidget(Controller.Get(), Settings->FadeInOutClass));
			}
		}
	}

	if (ActiveFadeInOutWidget)
	{
		if (ActiveFadeInOutWidget->IsVisible())
		{
			ActiveFadeInOutWidget->RequestCancel();
			if (bFadeInRequested)
			{
				OnFadeIn();
			}

			if (bFadeOutRequested)
			{
				OnFadeOut();
			}
		}

		if (!ActiveFadeInOutWidget->IsVisible())
		{
			ActiveFadeInOutWidget->AddToViewport(9999);
		}

		ActiveFadeInOutWidget->SetFadeOpacity(StartOpacity);
	}

	return ActiveFadeInOutWidget;
}

USGFadeInOutWidget* USGUISubsystem::GetFadeInOutWidgetInstance() const
{
	return ActiveFadeInOutWidget;
}

void USGUISubsystem::DoFadeIn()
{
	if (ActiveFadeInOutWidget)
	{
		ActiveFadeInOutWidget->DoFadeIn();
		bFadeInRequested = true;
	}
}

void USGUISubsystem::DoFadeOut()
{
	if (ActiveFadeInOutWidget)
	{
		ActiveFadeInOutWidget->DoFadeOut();
		bFadeOutRequested = true;
	}
}

FString USGUISubsystem::GetVersionString() const
{
	return FString::Printf(TEXT("Version: %s"), *UStaticSGData::GetVersion());
}




