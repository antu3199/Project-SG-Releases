#include "SGActorUIWidget.h"

#include "SoulsGame/SGBlueprintLibrary.h"
#include "UMG/Public/Components/WidgetComponent.h"

USGActorUIWidget::USGActorUIWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USGActorUIWidget::CustomInitialize(UWidgetComponent * InComponent, float InCurValue, float InMaxValue)
{
	Component = MakeWeakObjectPtr(InComponent);

	HideUIWidget();

	CurValue = InCurValue;
	MaxValue = InMaxValue;
	CustomInitialize_BP(InCurValue / InMaxValue);

	bIsDisplayed = false;
	OriginalActorUIDrawSize = Component->GetDrawSize();

	SetDrawSize();
}

void USGActorUIWidget::SetValue(float InCurValue)
{
	if (InCurValue != CurValue)
	{
		CurValue = InCurValue;
		SetValue_BP(InCurValue / MaxValue);
		ShowUIWidget();
	}
}

void USGActorUIWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsDisplayed)
	{
		SetDrawSize();
	}
}

void USGActorUIWidget::SetDrawSize() const
{
	if (Component.IsValid())
	{
		const float Distance = SGBlueprintLibrary::GetDistanceFromCamera(GetWorld(), 0, Component->GetOwner()->GetActorLocation());
		if (Distance != 0)
		{
			const float Scale = UIDistanceScale / Distance;
			const FVector2D NewDrawSize = OriginalActorUIDrawSize * Scale;
			Component->SetDrawSize(NewDrawSize);
		}
	}
}

void USGActorUIWidget::ShowUIWidget()
{
	if (bIsDisabled)
	{
		return;
	}
	
	bIsDisplayed = true;
	Component->SetHiddenInGame(false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &USGActorUIWidget::HideUIWidget, TimeDisplayed, false);
}

void USGActorUIWidget::HideUIWidget()
{
	if (bIsDisabled)
	{
		return;
	}

	bIsDisabled = false;
	Component->SetHiddenInGame(true);
}

