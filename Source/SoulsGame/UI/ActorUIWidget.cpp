#include "ActorUIWidget.h"

#include "SoulsGame/SBlueprintLibrary.h"
#include "UMG/Public/Components/WidgetComponent.h"

UActorUIWidget::UActorUIWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UActorUIWidget::CustomInitialize(UWidgetComponent * InComponent, float InCurValue, float InMaxValue)
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

void UActorUIWidget::SetValue(float InCurValue)
{
	if (InCurValue != CurValue)
	{
		CurValue = InCurValue;
		SetValue_BP(InCurValue / MaxValue);
		ShowUIWidget();
	}
}

void UActorUIWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsDisplayed)
	{
		SetDrawSize();
	}
}

void UActorUIWidget::SetDrawSize() const
{
	if (Component.IsValid())
	{
		const float Distance = SBlueprintLibrary::GetDistanceFromCamera(GetWorld(), 0, Component->GetOwner()->GetActorLocation());
		if (Distance != 0)
		{
			const float Scale = UIDistanceScale / Distance;
			const FVector2D NewDrawSize = OriginalActorUIDrawSize * Scale;
			Component->SetDrawSize(NewDrawSize);
		}
	}
}

void UActorUIWidget::ShowUIWidget()
{
	if (bIsDisabled)
	{
		return;
	}
	
	bIsDisplayed = true;
	Component->SetHiddenInGame(false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UActorUIWidget::HideUIWidget, TimeDisplayed, false);
}

void UActorUIWidget::HideUIWidget()
{
	if (bIsDisabled)
	{
		return;
	}

	bIsDisabled = false;
	Component->SetHiddenInGame(true);
}

