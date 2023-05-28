#include "SGFadeInOutWidget.h"

#include "SoulsGame/SUtils.h"
#include "SoulsGame/Subsystems/UISubsystem.h"

USGFadeInOutWidget::USGFadeInOutWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USGFadeInOutWidget::OnFadeInFinished()
{
	if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		Subsystem->OnFadeIn();
	}
}

void USGFadeInOutWidget::OnFadeOutFinished()
{
	if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		Subsystem->OnFadeOut();
	}
}
