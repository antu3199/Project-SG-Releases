#include "SAnimNotifyState_EmpoweredAura.h"

#include "SoulsGame/SUtils.h"

void USAnimNotifyState_EmpoweredAura::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                    float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	UWorld* World = MeshComp->GetWorld();
	if (FXTemplate != nullptr && FSUtils::bIsTimestopped)
	{
		ASHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
		if (!Character)
		{
			return;
		}

		FTransform Transform = Character->GetTransform();
		FVector Origin = Transform.TransformPosition(OffsetVector);
		//FVector Origin = Character->GetActorLocation();

		FVector SpawnLocation;
		const bool bIsValidLocation = FSUtils::GetGroundLocation(World, Origin, SpawnLocation);
		if (!bIsValidLocation)
		{
			return;
		}
		
		FTransform ComponentTransform;
		ComponentTransform.SetLocation(SpawnLocation);
		ComponentTransform.SetScale3D(FVector(1,1,1));
	
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; //Default
		SpawnParameters.bNoFail = true;

		AActor * SpawnedObject = World->SpawnActor<AActor>(this->FXTemplate, ComponentTransform, SpawnParameters);
	}
}

void USAnimNotifyState_EmpoweredAura::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);
}
