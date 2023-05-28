// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpSectionNS.h"

#include "SoulsGame/Character/MyPlayerController.h"
#include "SoulsGame/SUtils.h"


void UJumpSectionNS::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	UE_LOG(LogTemp, Display, TEXT("UJumpSectionNS Begin"));

	ASHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}
	
	Character->SetComboJumpSection(this);

	// Need to turn on buffered input
	if (AMyPlayerController* Controller = Cast<AMyPlayerController>(Character->GetController()))
	{
		Controller->SetCanQueueNextMove(true);
	}
	

	// 
	if (FSUtils::bIsTimestopped)
	{
		if (AMyPlayerController* Controller = Cast<AMyPlayerController>(Character->GetController()))
		{
			Controller->Debug_QueueEmpoweredAttack();
		}
	}

	if (!Character->IsPlayerControlled())
	{
		Character->TriggerJumpSectionCombo();
	}
	
	/*

	Character->JumpSectionCancellable = false;
	Character->BufferedInput.Reset();
	Character->BufferedInput.StartBufferingInput = true;
	UE_LOG(LogTemp, Display, TEXT("TriggerJumpSectionStart!"));

	*/

}

void UJumpSectionNS::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	UE_LOG(LogTemp, Display, TEXT("UJumpSectionNS End"));
	
	ASHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	if (bAllowInputOnNotifyEnd)
	{
		Character->SetAllowInputDuringMontage(true);
		Character->SetMontageCancellable(CancelMontageOnAllowInput);

		if (AMyPlayerController* Controller = Cast<AMyPlayerController>(Character->GetController()))
		{
			Controller->TryUseQueuedMove();
		}
		
		MeshComp->GetWorld()->GetTimerManager().SetTimerForNextTick([=]()
		{
			Character->SetAllowInputDuringMontage(false);
			Character->SetMontageCancellable(false);
			Character->SetComboJumpSection(nullptr);
		});
	}
	else
	{
		Character->SetComboJumpSection(nullptr);
	}


	



	/*

	Character->JumpSectionCancellable = true;
	Character->CheckBufferedInput();
	Character->BufferedInput.StartBufferingInput = false;
	Character->SetComboJumpSection(nullptr);
	*/
}
