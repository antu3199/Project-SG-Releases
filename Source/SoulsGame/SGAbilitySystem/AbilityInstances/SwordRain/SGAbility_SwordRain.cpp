#include "SGAbility_SwordRain.h"

#include "GameplayTagsManager.h"
#include "SGSwordRainActor.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/Character/SGCharacterBase.h"
#include "SoulsGame/Character/SGTeamComponent.h"
#include "SoulsGame/Controllers/SGMarkerController.h"


void USGAbility_SwordRain::PostInitProperties()
{
	Super::PostInitProperties();
}

void USGAbility_SwordRain::OnHandleEvent_Implementation(const FSGAbilityEventData& Payload)
{
	Super::OnHandleEvent_Implementation(Payload);

	static const FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Triggers.Ability.Shared.Activate"), false);

	if (Payload.EventTag == Tag)
	{
		SpawnMarkers();
	}
}

/*
 // TODO: Remember to do this
void USGAbility_SwordRain::PreActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::PreActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	bActivated = false;

	if (ASCharacterBase* Character = Cast<ASCharacterBase>(ActorInfo->OwnerActor))
	{
		if (USGPhaseComponent* PhaseComponent = Character->GetOptionalPhaseComponent())
		{
			SetLevel(PhaseComponent->GetPhase());
		}
	}
}
*/




void USGAbility_SwordRain::OnCastTimeFinished(const FVector& CastLocation, const int32 MarkerId)
{
	//GetWorld()->GetTimerManager().ClearTimer();

	if (const UWorld* World = GetWorld())
	{
		USGMarkerController::GetSingleton(World)->RemoveMarker(MarkerId);

		CastHandles.Remove(MarkerId);
	
		ASGSwordRainActor* SwordRainActor = CreateSwordRainActor(CastLocation);
		SwordRainActors.Add(SwordRainActor);
	}
	
}

void USGAbility_SwordRain::OnEndAbility_Implementation()
{
	for (auto& HandlePair : CastHandles)
	{
		GetWorld()->GetTimerManager().ClearTimer(HandlePair.Value);
		USGMarkerController::GetSingleton(GetWorld())->RemoveMarker(HandlePair.Key);
	}
	CastHandles.Reset();
	
	Super::OnEndAbility_Implementation();
}

void USGAbility_SwordRain::SpawnMarkers()
{
	if (ASGCharacterBase* Instigator = Cast<ASGCharacterBase>(ActorInfo.OwnerActor.Get()))
	{
		if (USGTeamComponent* TeamComponent = Instigator->GetTeamComponent())
		{
			ESGTeam OpposingTeam = TeamComponent->GetOpposingTeam();
			// TODO: Cap range later
			AActor* EnemyLocation = FSGUtils::GetClosestTeamCharacterFromLocation(GetWorld(), Instigator->GetActorLocation(), OpposingTeam);

			if (EnemyLocation != nullptr)
			{
				ExpectedNumberOfSwordRainActors = 0;
				FinishedSwordRainActors = 0;
				
				FVector SpawnLocation;
				bool bIsValidSpawnLocation = FSGUtils::GetGroundLocation(GetWorld(), EnemyLocation->GetActorLocation(), SpawnLocation);

				if (bIsValidSpawnLocation)
				{
					ExpectedNumberOfSwordRainActors++;

					ActivateSwordRainAtLocation(SpawnLocation);
				}
				
				if (Level >= 1)
				{
					FVector Forward = (SpawnLocation - ActorInfo.OwnerActor->GetActorLocation());
					Forward.Normalize();

					const FVector Right = Forward.Cross(FVector::UpVector);
					TArray<FVector> LevelValidSpawnLocations;

					FVector UsedSpawnLocation;
					bIsValidSpawnLocation = FSGUtils::GetGroundLocation(GetWorld(), SpawnLocation + Right * SpawningOffset, UsedSpawnLocation);
					if (bIsValidSpawnLocation)
					{
						LevelValidSpawnLocations.Add(UsedSpawnLocation);
					}

					bIsValidSpawnLocation = FSGUtils::GetGroundLocation(GetWorld(), SpawnLocation + -Right * SpawningOffset, UsedSpawnLocation);
					if (bIsValidSpawnLocation)
					{
						LevelValidSpawnLocations.Add(UsedSpawnLocation);
					}

					FVector ForwardSpawnOffset = SpawnLocation - Forward * SpawningOffset;
					bIsValidSpawnLocation = FSGUtils::GetGroundLocation(GetWorld(), ForwardSpawnOffset, UsedSpawnLocation);
					if (bIsValidSpawnLocation)
					{
						LevelValidSpawnLocations.Add(UsedSpawnLocation);
					}
						
					bIsValidSpawnLocation = FSGUtils::GetGroundLocation(GetWorld(), ForwardSpawnOffset + Right * SpawningOffset, UsedSpawnLocation);
					if (bIsValidSpawnLocation)
					{
						LevelValidSpawnLocations.Add(UsedSpawnLocation);
					}

					bIsValidSpawnLocation = FSGUtils::GetGroundLocation(GetWorld(), ForwardSpawnOffset + -Right * SpawningOffset, UsedSpawnLocation);
					if (bIsValidSpawnLocation)
					{
						LevelValidSpawnLocations.Add(UsedSpawnLocation);
					}
					
					FVector BackSpawnOffset = SpawnLocation + Forward * SpawningOffset;
					bIsValidSpawnLocation = FSGUtils::GetGroundLocation(GetWorld(), BackSpawnOffset, UsedSpawnLocation);
					if (bIsValidSpawnLocation)
					{
						LevelValidSpawnLocations.Add(UsedSpawnLocation);
					}
						
					bIsValidSpawnLocation = FSGUtils::GetGroundLocation(GetWorld(), BackSpawnOffset + Right * SpawningOffset, UsedSpawnLocation);
					if (bIsValidSpawnLocation)
					{
						LevelValidSpawnLocations.Add(UsedSpawnLocation);
					}

					bIsValidSpawnLocation = FSGUtils::GetGroundLocation(GetWorld(), BackSpawnOffset + -Right * SpawningOffset, UsedSpawnLocation);
					if (bIsValidSpawnLocation)
					{
						LevelValidSpawnLocations.Add(UsedSpawnLocation);
					}

					
					// Level 2

					FSGUtils::ShuffleArray(LevelValidSpawnLocations);
					if (Level == 1)
					{
						ExpectedNumberOfSwordRainActors += 3;
					}
					else if (Level == 2)
					{
						ExpectedNumberOfSwordRainActors += 7;
					}

					ExpectedNumberOfSwordRainActors = FMath::Min(ExpectedNumberOfSwordRainActors, LevelValidSpawnLocations.Num());

					
					float UsedTimeOffset = 0;
					for (int32 i = 0; i < ExpectedNumberOfSwordRainActors; i++)
					{
						UsedTimeOffset += TimeOffset;
						const FVector Location = LevelValidSpawnLocations[i];
						FTimerHandle Handle;
						auto ActivateSpawnLambda = [this, Location]()
						{
							ActivateSwordRainAtLocation(Location);
						};
					
						GetWorld()->GetTimerManager().SetTimer(Handle, ActivateSpawnLambda, UsedTimeOffset, false);
					}
				}
			}
		}
		
	}
}

ASGSwordRainActor* USGAbility_SwordRain::CreateSwordRainActor(const FVector& ActorSpawnLocation)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //Default
	SpawnParameters.bNoFail = true;
	SpawnParameters.Instigator = nullptr;
	ASGSwordRainActor* SpawnedObject = GetWorld()->SpawnActor<ASGSwordRainActor>(SwordRainActorTemplate, ActorSpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	if (!SpawnedObject)
	{
		return nullptr;
	}

	SpawnedObject->Initialize(this);
	FSGEffectInstigatorContext InstigatorContext = CreateEffectInstigatorContextForAbility(FGameplayTag());
	SpawnedObject->SetEffectContext(InstigatorContext);
	SpawnedObject->PlayEffect();

	return SpawnedObject;
	
}

void USGAbility_SwordRain::OnSwordRainActorDestroy(ASGSwordRainActor* Actor)
{
	int32 Index = -1;
	for (int32 i = 0; i < SwordRainActors.Num(); i++)
	{
		if (Actor == SwordRainActors[i])
		{
			Index = i;
			break;
		}
	}

	if (Index != -1)
	{
		SwordRainActors.RemoveAt(Index);
	}

	FinishedSwordRainActors++;

	if (FinishedSwordRainActors >= ExpectedNumberOfSwordRainActors)
	{
		RequestEndAbility();
	}
}

void USGAbility_SwordRain::ActivateSwordRainAtLocation(const FVector& Location)
{
	const int32 Id = USGMarkerController::GetSingleton(GetWorld())->CreateNewMarker(Location, MarkerSize, false);


	auto OnCastTimeFinishedLambda = [=]()
	{
		OnCastTimeFinished(Location, Id);
	};

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, OnCastTimeFinishedLambda, CastTime, false);
	CastHandles.Add(Id, Handle);
}
