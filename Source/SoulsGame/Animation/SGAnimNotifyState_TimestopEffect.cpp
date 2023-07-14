#include  "SGAnimNotifyState_TimestopEffect.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/SGGameModeBase.h"

void USGAnimNotifyState_TimestopEffect::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                      float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	// TODO: Do we want to have actual timestop as well?
	if (ASGGameModeBase* GameMode = Cast<ASGGameModeBase>(UGameplayStatics::GetGameMode(MeshComp->GetWorld())))
	{
		GameMode->AddTimestopMaterial();
	}
}

void USGAnimNotifyState_TimestopEffect::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	if (ASGGameModeBase* GameMode = Cast<ASGGameModeBase>(UGameplayStatics::GetGameMode(MeshComp->GetWorld())))
	{
		GameMode->RemoveTimestopMaterial();
	}
}
