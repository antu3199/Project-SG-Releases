// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAnimNotifyState_JumpSection.h"

#include "SoulsGame/Character/SGPlayerController.h"
#include "SoulsGame/SGUtils.h"


void USGAnimNotifyState_JumpSection::DoNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::DoNotifyBegin(MeshComp, Animation, TotalDuration);

	UE_LOG(LogTemp, Display, TEXT("UJumpSectionNS Begin"));

	ASGHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}
	
	Character->SetComboJumpSection(this);

	// Need to turn on buffered input
	if (ASGPlayerController* Controller = Cast<ASGPlayerController>(Character->GetController()))
	{
		Controller->SetCanQueueNextMove(true);
	}
	

	// 
	if (FSGUtils::bIsTimestopped)
	{
		if (ASGPlayerController* Controller = Cast<ASGPlayerController>(Character->GetController()))
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

void USGAnimNotifyState_JumpSection::DoNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::DoNotifyEnd(MeshComp, Animation);

	UE_LOG(LogTemp, Display, TEXT("UJumpSectionNS End"));
	
	ASGHumanoidCharacter * Character = GetHumanoidCharacter(MeshComp);
	if (!Character)
	{
		return;
	}

	if (bAllowInputOnNotifyEnd)
	{
		Character->SetAllowInputDuringMontage(true);
		Character->SetMontageCancellable(CancelMontageOnAllowInput);

		if (ASGPlayerController* Controller = Cast<ASGPlayerController>(Character->GetController()))
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
