#include "InteractableActor.h"

#include "SoulsGame/SUtils.h"
#include "SoulsGame/Character/MyPlayerController.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Subsystems/UISubsystem.h"
#include "SoulsGame/UI/HUDWidget.h"

void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::F, IE_Released, this, &AInteractableActor::OnFKeyPressed);
	}    
}



void AInteractableActor::QueryBeginInteract(AActor* OtherActor)
{
	if (OtherActor == nullptr)
	{
		return;
	}

	ASCharacterBase* Character = Cast<ASCharacterBase>(OtherActor);
	if (Character == nullptr)
	{
		return;
	}

	InteractedActor = MakeWeakObjectPtr(Character);
	if (AMyPlayerController* Controller = Cast<AMyPlayerController>(InteractedActor->GetController()))
	{
		EnableInput(Controller);
	}

	UInteractEntryData* InteractEntryData = NewObject<UInteractEntryData>(this);
	InteractEntryData->Owner = this;
	InteractEntryData->DisplayText = InteractDisplayText;

	if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		if (UHUDWidget* HUD = Subsystem->GetHUDInstance())
		{
			HUD->AddInteract(InteractEntryData);
		}
	}
}

void AInteractableActor::QueryEndInteract(AActor* OtherActor)
{
	if (OtherActor == InteractedActor.Get())
	{
		if (AMyPlayerController* Controller = Cast<AMyPlayerController>(InteractedActor->GetController()))
		{
			Unlock();
			DisableInput(Controller);
			if (const USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
			{
				if (UHUDWidget* HUD = Subsystem->GetHUDInstance())
				{
					HUD->RemoveInteract(this);
				}
			}
		}
	}
}

void AInteractableActor::UnInteract_Implementation(AMyPlayerController* Controller)
{
	Unlock();
}

void AInteractableActor::Unlock()
{
	bInteracted = false;
}

void AInteractableActor::UnInteract_BP()
{
	if (InteractedActor.IsValid())
	{
		if (AMyPlayerController* Controller = Cast<AMyPlayerController>(InteractedActor->GetController()))
		{
			UnInteract(Controller);
		}
	}
}

void AInteractableActor::OnInteract(AMyPlayerController* Controller)
{
}

void AInteractableActor::OnFKeyPressed()
{
	if (bLockWhenInteract && bInteracted)
	{
		return;
	}
	
	if (InteractedActor.IsValid())
	{
		if (AMyPlayerController* Controller = Cast<AMyPlayerController>(InteractedActor->GetController()))
		{
			Interact(Controller);
		}
	}
}


void AInteractableActor::Interact_Implementation(AMyPlayerController* Controller)
{
	OnInteract(Controller);
	bInteracted = true;
}