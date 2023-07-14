#include "SGEnemyAIController.h"

#include "SoulsGame/Character/SGCharacterBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "SoulsGame/SGGameModeBase.h"

void ASGEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	InitializeBoundingVolume();
}

void ASGEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	InitializeBoundingVolume();

	// Note: When loading a streaming level, character is not set yet, so move intiailization to OnPossessed()
	if (ASGCharacterBase* CastedCharacter = Cast<ASGCharacterBase>(GetCharacter()))
	{
		CastedCharacter->bUseControllerRotationYaw = true;
	}
}

bool ASGEnemyAIController::IsLocationInBoundingVolume(const FVector& Location)
{
	InitializeBoundingVolume();
	
	if (BoundingVolume == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor is not inside a bounding volume!"));
		return false;
	}

	const FBoxSphereBounds Bounds = BoundingVolume->GetBounds();
	if (Bounds.GetBox().IsInside(Location))
	{
		return true;
	}

	return false;
}

AActor* ASGEnemyAIController::GetAITarget() const
{
	if (const UBlackboardComponent* BlackboardComponent = GetBrainComponent()->GetBlackboardComponent())
	{
		AActor* Actor = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("Target")));
		return Actor;
	}

	return nullptr;
}

void ASGEnemyAIController::InitializeBoundingVolume()
{
	if (BoundingVolume != nullptr)
	{
		return;
	}

	if (ASGCharacterBase* CastedCharacter = Cast<ASGCharacterBase>(GetCharacter()))
	{
	
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass(), FoundActors);

		float MinDistanceFromVolume = TNumericLimits<float>::Max();
		int32 MinIndex = -1;

		for (int32 i = 0; i < FoundActors.Num(); i++)
		{
			ANavMeshBoundsVolume* Volume = Cast<ANavMeshBoundsVolume>(FoundActors[i]);
			const FBoxSphereBounds Bounds = Volume->GetBounds();
			if (Bounds.GetBox().IsInside(CastedCharacter->GetActorLocation()))
			{
				float Distance = FVector::Distance(Bounds.Origin, CastedCharacter->GetActorLocation());
				if (Distance < MinDistanceFromVolume)
				{
					MinIndex = i;
					MinDistanceFromVolume = Distance;
				}
			}
		}

		if (MinIndex != -1)
		{
			BoundingVolume = Cast<ANavMeshBoundsVolume>(FoundActors[MinIndex]);
		}

		if (CastedCharacter->GetStartAggroed())
		{
			int32 Asd = 0;
		}

		if (BoundingVolume == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot find bounding volume!"));
		}
	}
}





