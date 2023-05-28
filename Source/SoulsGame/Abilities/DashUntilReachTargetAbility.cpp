#include "DashUntilReachTargetAbility.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Character/SGTeamComponent.h"
#include "SoulsGame/UE4Overrides/MyCharacterMovementComponent.h"

void UDashUntilReachTargetAbility::SetStoppingDistance(float Distance)
{
	StoppingDistance = Distance;
}

void UDashUntilReachTargetAbility::PreActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::PreActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASCharacterBase* Character = GetCharacter();
	if (Character == nullptr)
	{
		return;
	}

	// If player controlled, then just do as normal
	// if (Character->IsPlayerControlled())
	// {
	// 	return;
	// }

	UMyCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovementComponent();
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
			AActor* ClosestTarget = FSUtils::GetClosestTeamCharacterFromLocation(GetWorld(), Character->GetActorLocation(), OpposingTeam);
			TargetActor = ClosestTarget;
		
		}
	}


	Character->OnCharacterNotifyHit.AddUObject(this, &UDashUntilReachTargetAbility::OnCharacterHitObject);
	CharacterMovementComponent->AddFreezeFrameCondition(std::bind(&UDashUntilReachTargetAbility::UnFreezeCondition, this));

	bStopDueToHitActor = false;
	CachedLastDistance = -1.0f;
}

void UDashUntilReachTargetAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	
	if (ASCharacterBase* Character = GetCharacter())
	{
		// Freeze frame conditions automatically removed
		if (UMyCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovementComponent())
		{
		}

		Character->OnCharacterNotifyHit.RemoveAll(this);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UDashUntilReachTargetAbility::UnFreezeCondition()
{
	ASCharacterBase* Character = GetCharacter();
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
		
		//if (CachedLastDistance != -1.0f)
		//{
		//	const float DiffDistance = CachedLastDistance - Distance;
		//	if (DiffDistance < 0.0f)
		//	{
		//		return  true;
		//	}
		//}
//
		//CachedLastDistance = Distance;
	}

	if (bStopDueToHitActor)
	{
		return true;
	}
	
	return false;
}

void UDashUntilReachTargetAbility::OnCharacterHitObject(ASCharacterBase* Character, AActor* Other,
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
