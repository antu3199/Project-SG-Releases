// Fill out your copyright notice in the Description page of Project Settings.


#include "SGBTService_SetTargetObjectWithinRange.h"


#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/Character/SGCharacterBase.h"
#include "SoulsGame/Character/SGTeamComponent.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "SoulsGame/AI/SGEnemyAIController.h"

USGBTService_SetTargetObjectWithinRange::USGBTService_SetTargetObjectWithinRange()
{
	NodeName = "UBTService_SetTargetObjectWithinRange";
}

void USGBTService_SetTargetObjectWithinRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ASGCharacterBase * Character = Cast<ASGCharacterBase>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Character)
	{
		return;
	}

	ASGEnemyAIController* AIController = Cast<ASGEnemyAIController>(Character->GetController());
	if (!AIController)
	{
		return;
	}

	USGTeamComponent* TeamComponent = Character->GetTeamComponent();

	if (TeamComponent == nullptr)
	{
		return;
	}
	ESGTeam OpposingTeam = TeamComponent->GetOpposingTeam();
	// TODO: Cap range later
	AActor* ClosestTarget = FSGUtils::GetClosestTeamCharacterFromLocation(GetWorld(), Character->GetActorLocation(), OpposingTeam);


	/*
	
	if (Debug_JustSelectPlayer)
	{
		APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn == nullptr)
		{
			return;
		}

		Character->SetFocusTarget(PlayerPawn->GetRootComponent());
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn);
		return;
	}
	

	FVector Center = Character->GetActorLocation();

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	if (bSelectPlayersOnly)
	{
		TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1)); //Player
	}
	else
	{
		TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2)); //Enemy
	}
	
	
	UClass * SeekClass = ASCharacterBase::StaticClass();

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(Character);

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Center, TargetRadius, TraceObjectTypes, SeekClass, IgnoreActors, OutActors);
	
	// Visualization:

	float ClosestActorDistance = INT_MAX;
	AActor * ClosestActor = nullptr;

	// Select closest actor
	for (AActor* OverlappedActor : OutActors)
	{
		if (OverlappedActor == nullptr) continue;
		
		//UE_LOG(LogTemp, Log, TEXT("OverlappedActor: %s"), *overlappedActor->GetName());
		ASCharacterBase * OverlappedCharacter = Cast<ASCharacterBase>(OverlappedActor);

		if (bSelectPlayersOnly && !OverlappedCharacter->IsPlayerControlled())
		{
			continue;
		}

		if (!bSelectPlayersOnly && OverlappedCharacter->IsPlayerControlled())
		{
			continue;
		}

		if (OverlappedCharacter == nullptr || OverlappedCharacter->GetIsDead()) continue;
		
		FVector OtherActorLocation = OverlappedActor->GetActorLocation();
		float Distance = FVector::Distance(Center, OtherActorLocation);
		if (Distance < ClosestActorDistance)
		{
			ClosestActor = OverlappedActor;
			ClosestActorDistance = Distance;
		}
	}

	if (ClosestActor == nullptr)
	{
		return;
	}

	if (ClosestActor != nullptr)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Closest actor %s"), *ClosestActor->GetActorNameOrLabel());
	}
	*/
	

	// Check if in nav mesh, but doesn't check if inbetween....
	// FNavLocation OutLocation;
	// UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	// bool Test = NavSys->ProjectPointToNavigation(ClosestTarget->GetActorLocation(), OutLocation);

	if (Character->GetStartAggroed())
	{
		int32 ASD = 0;
	}

	bool bInBoundingVolume = false;
	if (ClosestTarget != nullptr)
	{
		if (ASGEnemyAIController* CastedController = Cast<ASGEnemyAIController>(Character->GetController()))
		{
			bInBoundingVolume = CastedController->IsLocationInBoundingVolume(ClosestTarget->GetActorLocation());

			if (!bInBoundingVolume)
			{
				//ClosestTarget = nullptr;
				AIController->SetAggro(false);
			}
			else
			{
				// If start aggroed, always set aggro to true
				if (Character->GetStartAggroed())
				{
					AIController->SetAggro(true);
				}
			}
		}
	
	}

	if (ClosestTarget != nullptr)
	{
		if (!AIController->IsAggroed())
		{
			ClosestTarget = nullptr;
		}
	}

	if (ClosestTarget != nullptr)
	{
		Character->SetFocusTarget(ClosestTarget->GetRootComponent());
	}
	else
	{
		Character->SetFocusTarget(nullptr);
	}


	OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), ClosestTarget);


	// Targetting callbacks

	const bool bHasTarget = ClosestTarget != nullptr;
	const bool bPreviouslyHadTarget = AIController->HasTarget();
	if (bPreviouslyHadTarget != bHasTarget)
	{
		AIController->SetHasTarget(bHasTarget);
		if (ASGCharacterBase* ClosestCharacter = Cast<ASGCharacterBase>(ClosestTarget))
		{
			ClosestCharacter->OnAggroSet(bHasTarget, Character);
		}
	}
	
	/*
	if (OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	}
	*/
}
