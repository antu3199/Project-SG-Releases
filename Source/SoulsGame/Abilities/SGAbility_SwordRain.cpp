#include "SGAbility_SwordRain.h"

#include "SGAbilitySystemGlobals.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Character/SGTeamComponent.h"
#include "SoulsGame/Controllers/SGMarkerController.h"


void USGAbility_SwordRain::PostInitProperties()
{
	Super::PostInitProperties();

	EndOnMontageCompleted = false;
}

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

bool USGAbility_SwordRain::HandleGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (Super::HandleGameplayEvent(EventTag, Payload))
	{
		return true;
	}
	
	if (bActivated)
	{
		return true;
	}
	
	bool bHandled = false;
	FString Tag = EventTag.ToString();
	if (EventTag == USGAbilitySystemGlobals::GetSG().Tag_Triggers_Ability_Activate)
	{
		ASCharacterBase* Instigator = GetCharacter();
		if (Instigator != nullptr)
		{
			if (USGTeamComponent* TeamComponent = Instigator->GetTeamComponent())
			{
				ESGTeam OpposingTeam = TeamComponent->GetOpposingTeam();
				// TODO: Cap range later
				AActor* EnemyLocation = FSUtils::GetClosestTeamCharacterFromLocation(GetWorld(), Instigator->GetActorLocation(), OpposingTeam);

				if (EnemyLocation != nullptr)
				{
					ExpectedNumberOfSwordRainActors = 0;
					FinishedSwordRainActors = 0;
					
					FVector SpawnLocation;
					bool bIsValidSpawnLocation = FSUtils::GetGroundLocation(GetWorld(), EnemyLocation->GetActorLocation(), SpawnLocation);

					if (bIsValidSpawnLocation)
					{
						ExpectedNumberOfSwordRainActors++;

						ActivateSwordRainAtLocation(SpawnLocation);
					}
					
					if (CurrentLevel >= 1)
					{
						FVector Forward = (SpawnLocation - CurrentActorInfo->OwnerActor->GetActorLocation());
						Forward.Normalize();

						const FVector Right = Forward.Cross(FVector::UpVector);
						TArray<FVector> LevelValidSpawnLocations;

						FVector UsedSpawnLocation;
						bIsValidSpawnLocation = FSUtils::GetGroundLocation(GetWorld(), SpawnLocation + Right * SpawningOffset, UsedSpawnLocation);
						if (bIsValidSpawnLocation)
						{
							LevelValidSpawnLocations.Add(UsedSpawnLocation);
						}

						bIsValidSpawnLocation = FSUtils::GetGroundLocation(GetWorld(), SpawnLocation + -Right * SpawningOffset, UsedSpawnLocation);
						if (bIsValidSpawnLocation)
						{
							LevelValidSpawnLocations.Add(UsedSpawnLocation);
						}

						FVector ForwardSpawnOffset = SpawnLocation - Forward * SpawningOffset;
						bIsValidSpawnLocation = FSUtils::GetGroundLocation(GetWorld(), ForwardSpawnOffset, UsedSpawnLocation);
						if (bIsValidSpawnLocation)
						{
							LevelValidSpawnLocations.Add(UsedSpawnLocation);
						}
							
						bIsValidSpawnLocation = FSUtils::GetGroundLocation(GetWorld(), ForwardSpawnOffset + Right * SpawningOffset, UsedSpawnLocation);
						if (bIsValidSpawnLocation)
						{
							LevelValidSpawnLocations.Add(UsedSpawnLocation);
						}

						bIsValidSpawnLocation = FSUtils::GetGroundLocation(GetWorld(), ForwardSpawnOffset + -Right * SpawningOffset, UsedSpawnLocation);
						if (bIsValidSpawnLocation)
						{
							LevelValidSpawnLocations.Add(UsedSpawnLocation);
						}
						
						FVector BackSpawnOffset = SpawnLocation + Forward * SpawningOffset;
						bIsValidSpawnLocation = FSUtils::GetGroundLocation(GetWorld(), BackSpawnOffset, UsedSpawnLocation);
						if (bIsValidSpawnLocation)
						{
							LevelValidSpawnLocations.Add(UsedSpawnLocation);
						}
							
						bIsValidSpawnLocation = FSUtils::GetGroundLocation(GetWorld(), BackSpawnOffset + Right * SpawningOffset, UsedSpawnLocation);
						if (bIsValidSpawnLocation)
						{
							LevelValidSpawnLocations.Add(UsedSpawnLocation);
						}

						bIsValidSpawnLocation = FSUtils::GetGroundLocation(GetWorld(), BackSpawnOffset + -Right * SpawningOffset, UsedSpawnLocation);
						if (bIsValidSpawnLocation)
						{
							LevelValidSpawnLocations.Add(UsedSpawnLocation);
						}

						
						// Level 2

						FSUtils::ShuffleArray(LevelValidSpawnLocations);
						if (CurrentLevel == 1)
						{
							ExpectedNumberOfSwordRainActors += 3;
						}
						else if (CurrentLevel == 2)
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
					
					//MarkerId = USGMarkerController::CreateNewMarker()
					// Do something cool!
					bHandled = true;
					bActivated = true;
				}

			}
			
		}

	}

	return bHandled;
}

void USGAbility_SwordRain::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	for (auto& HandlePair : CastHandles)
	{
		GetWorld()->GetTimerManager().ClearTimer(HandlePair.Value);
	}
	CastHandles.Reset();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGAbility_SwordRain::OnCastTimeFinished(const FVector& CastLocation, const int32 MarkerId)
{
	//GetWorld()->GetTimerManager().ClearTimer();

	USGMarkerController::GetSingleton(GetWorld())->RemoveMarker(MarkerId);

	CastHandles.Remove(MarkerId);
	
	ASGSwordRainActor* SwordRainActor = CreateSwordRainActor(CastLocation);
	SwordRainActors.Add(SwordRainActor);


	
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
	SpawnedObject->SetGameplayEffectDataContainer(GameplayEffectsContainer);
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
		this->EndAbility(this->CurrentSpecHandle, this->CurrentActorInfo, this->CurrentActivationInfo, true, false);
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
