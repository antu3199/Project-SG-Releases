#include "SGFadeInOutWidget.h"

#include "SoulsGame/SGUtils.h"
#include "SoulsGame/Subsystems/SGUISubsystem.h"

USGFadeInOutWidget::USGFadeInOutWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USGFadeInOutWidget::OnFadeInFinished()
{
	if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		Subsystem->OnFadeIn();
	}
}

void USGFadeInOutWidget::OnFadeOutFinished()
{
	if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		Subsystem->OnFadeOut();
	}
}
