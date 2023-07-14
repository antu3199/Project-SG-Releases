#include "SGAbility_DashUntilReachTarget.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/Character/SGCharacterBase.h"
#include "SoulsGame/Character/SGTeamComponent.h"
#include "SoulsGame/UE4Overrides/SGCharacterMovementComponent.h"

void USGAbility_DashUntilReachTarget::SetStoppingDistance(float Distance)
{
	StoppingDistance = Distance;
}

void USGAbility_DashUntilReachTarget::OnActivate_Implementation()
{
	Super::OnActivate_Implementation();

	ASGCharacterBase* Character = Cast<ASGCharacterBase>(ActorInfo.OwnerActor.Get());
	if (Character == nullptr)
	{
		return;
	}

	// If player controlled, then just do as normal
	// if (Character->IsPlayerControlled())
	// {
	// 	return;
	// }

	USGCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovementComponent();
	if (CharacterMovementComponent == nullptr)
	{
		return;
	}
	
	if (AAIController* AIController = UAIBlueprintHelperLibrary::GetAIController(Character))
	{
		BlackboardComponent = AIController->GetBrainComponent()->GetBlackboardComponent();
		if (BlackboardComponent == nullptr)
		{
			return;
		}

		TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorBlackboardKey));
	}
	else
	{
		// This should maybe just be for debugging. Should refactor otherwise
		if (USGTeamComponent* TeamComponent = Character->GetTeamComponent())
		{
			ESGTeam OpposingTeam = TeamComponent->GetOpposingTeam();
			// TODO: Cap range later
			AActor* ClosestTarget = FSGUtils::GetClosestTeamCharacterFromLocation(GetWorld(), Character->GetActorLocation(), OpposingTeam);
			TargetActor = ClosestTarget;
		
		}
	}

	Character->OnCharacterNotifyHit.AddUObject(this, &USGAbility_DashUntilReachTarget::OnCharacterHitObject);
	CharacterMovementComponent->AddFreezeFrameCondition(std::bind(&USGAbility_DashUntilReachTarget::UnFreezeCondition, this));

	bStopDueToHitActor = false;
	CachedLastDistance = -1.0f;
}

void USGAbility_DashUntilReachTarget::OnEndAbility_Implementation()
{
	if (ASGCharacterBase* Character = Cast<ASGCharacterBase>(ActorInfo.OwnerActor.Get()))
	{
		// Freeze frame conditions automatically removed
		Character->OnCharacterNotifyHit.RemoveAll(this);
	}
	
	Super::OnEndAbility_Implementation();
}

bool USGAbility_DashUntilReachTarget::UnFreezeCondition()
{
	ASGCharacterBase* Character = Cast<ASGCharacterBase>(ActorInfo.OwnerActor.Get());
	if (!Character)
	{
		return true;
	}
	
	if (IsValid(TargetActor))
	{
		const float Distance = FVector::Distance(Character->GetActorLocation(), TargetActor->GetActorLocation());

		if (Distance < StoppingDistance)
		{
			return true;
		}

		FVector Diff = Character->GetActorLocation() - Character->GetActorLocation();
		Diff.Normalize();
		FVector Forward = Character->GetActorForwardVector();
		const float Dot = FVector::DotProduct(Diff, Forward);

		if (Dot < 0)
		{
			return true;
		}
	}

	if (bStopDueToHitActor)
	{
		return true;
	}
	
	return false;
}

void USGAbility_DashUntilReachTarget::OnCharacterHitObject(ASGCharacterBase* Character, AActor* Other,
	const FHitResult& Hit)
{
	if (Character == nullptr)
	{
		return;
	}

	FVector Diff = Hit.Location - Character->GetActorLocation();
	Diff.Normalize();

	FVector Forward = Character->GetActorForwardVector();

	const float Dot = FVector::DotProduct(Diff, Forward);
	if (Dot >= 0)
	{
		Character->OnCharacterNotifyHit.RemoveAll(this);

		bStopDueToHitActor = true;
	}
}
