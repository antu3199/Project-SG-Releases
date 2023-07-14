#include "SGNarrativeWidget.h"

#include "SGHUDWidget.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/Subsystems/SGUISubsystem.h"

USGNarrativeWidget::USGNarrativeWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USGNarrativeWidget::PlayNarrativeText(const FSGNarrativeTableData& Data)
{
	if (Data.Text.Num() == 0)
	{
		return;
	}
	
	ResetRuntimeData();
	RuntimeData.Text = Data.Text;
	const float DisplayForTime = GetNarrationTime(0);
	
	OnDisplayNarrativeText_BP(RuntimeData.Text[0], DisplayForTime);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USGNarrativeWidget::AdvanceNarrativeText, DisplayForTime, false, DisplayForTime);
}

void USGNarrativeWidget::ResetRuntimeData()
{
	RuntimeData.Text.Reset();
	RuntimeData.TextIndex = 0;
	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
}

float USGNarrativeWidget::GetNarrationTime(int32 TextIndex)
{
	// TODO: Maybe calculate time based on audio
	return 3.0f;
}

void USGNarrativeWidget::EndNarration()
{
	ResetRuntimeData();
	if (const USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		if (USGHUDWidget* HUD = Subsystem->GetHUDInstance())
		{
			HUD->RemoveNarrative();
		}
	}
}

void USGNarrativeWidget::AdvanceNarrativeText()
{
	if (TimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}

	RuntimeData.TextIndex++;

	if (RuntimeData.TextIndex < RuntimeData.Text.Num())
	{
		const float DisplayForTime = GetNarrationTime(RuntimeData.TextIndex);
		OnDisplayNarrativeText_BP(RuntimeData.Text[RuntimeData.TextIndex], DisplayForTime);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USGNarrativeWidget::AdvanceNarrativeText, DisplayForTime, false, DisplayForTime);
	}
	else
	{
		EndNarration();
	}



}
