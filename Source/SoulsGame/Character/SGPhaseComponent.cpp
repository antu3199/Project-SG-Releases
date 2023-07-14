#include "SGPhaseComponent.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "SGCharacterBase.h"

void USGPhaseComponent::BeginPlay()
{
	Super::BeginPlay();

	Phases.Sort( [=](const FSGPhase& PhaseA, const FSGPhase& PhaseB)
	{
		return PhaseA.HealthPercent > PhaseB.HealthPercent;
	});

	Phase = 0;

	// Note: Don't call update phase here because stats haven't been applied yet
}

int32 USGPhaseComponent::GetPhase() const
{
	return Phase;
}

void USGPhaseComponent::UpdatePhase()
{
	if (const ASGCharacterBase* Character = Cast<ASGCharacterBase>(GetOwner()))
	{
		const float HealthPercent = Character->GetHealthPercent();
		int32 CurPhase = 0;
		for (CurPhase = 0; CurPhase < Phases.Num() - 1; CurPhase++)
		{
			if (HealthPercent < Phases[CurPhase].HealthPercent && HealthPercent >= Phases[CurPhase + 1].HealthPercent)
			{
				break;
			}
		}

		// Cannot go back phases
		if (CurPhase > Phase)
		{
			Phase = CurPhase;
			OnTransitionPhase(CurPhase);
		}
	}
}

void USGPhaseComponent::OnTransitionPhase(int32 NewPhase)
{
	if (UAnimInstance* AnimInstance = Helper_GetAnimMontage())
	{
		if (StaggerMontage != nullptr)
		{
			AnimInstance->StopAllMontages(0.0f);

			if (const ASGCharacterBase* Character = Cast<ASGCharacterBase>(GetOwner()))
			{
				if (AAIController* AIController = Cast<AAIController>(Character->GetController()))
				{
					//AIController->BrainComponent->RestartLogic(); //STop logic?
					AIController->BrainComponent->StopLogic(TEXT("Phase Transition"));
				}
			}

			AnimInstance->Montage_Play(StaggerMontage);

			this->MontageEndedDelegate.BindUObject(this, &USGPhaseComponent::OnMontageEnded);
			AnimInstance->Montage_SetEndDelegate(this->MontageEndedDelegate, StaggerMontage);
		}
	}
	
	OnPhaseChanged.Broadcast(NewPhase);
}

UAnimInstance* USGPhaseComponent::Helper_GetAnimMontage() const
{
	if (const ASGCharacterBase* Character = Cast<ASGCharacterBase>(GetOwner()))
	{
		if (UAnimInstance * AnimInstance = Character->GetMesh()->GetAnimInstance())
		{
			return AnimInstance;
		}
	}

	return nullptr;
}

void USGPhaseComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	this->MontageEndedDelegate.Unbind();
	if (const ASGCharacterBase* Character = Cast<ASGCharacterBase>(GetOwner()))
	{
		if (AAIController* AIController = Cast<AAIController>(Character->GetController()))
		{
			//AIController->BrainComponent->RestartLogic(); //STop logic?
			AIController->BrainComponent->RestartLogic();
		}
	}
}
