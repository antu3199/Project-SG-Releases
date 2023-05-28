#include  "NS_TimestopEffect.h"

#include "Kismet/GameplayStatics.h"
#include "SoulsGame/MyGameModeBase.h"

void UNS_TimestopEffect::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	// TODO: Do we want to have actual timestop as well?
	if (AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(MeshComp->GetWorld())))
	{
		GameMode->AddTimestopMaterial();
	}
}

void UNS_TimestopEffect::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	if (AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(MeshComp->GetWorld())))
	{
		GameMode->RemoveTimestopMaterial();
	}
}
