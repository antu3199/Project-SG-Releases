#include "SGInteractableActor.h"

#include "SoulsGame/SGUtils.h"
#include "SoulsGame/Character/SGPlayerController.h"
#include "SoulsGame/Character/SGCharacterBase.h"
#include "SoulsGame/Subsystems/SGUISubsystem.h"
#include "SoulsGame/UI/SGHUDWidget.h"

void ASGInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::F, IE_Released, this, &ASGInteractableActor::OnFKeyPressed);
	}    
}



void ASGInteractableActor::QueryBeginInteract(AActor* OtherActor)
{
	if (OtherActor == nullptr)
	{
		return;
	}

	ASGCharacterBase* Character = Cast<ASGCharacterBase>(OtherActor);
	if (Character == nullptr)
	{
		return;
	}

	InteractedActor = MakeWeakObjectPtr(Character);
	if (ASGPlayerController* Controller = Cast<ASGPlayerController>(InteractedActor->GetController()))
	{
		EnableInput(Controller);
	}

	USGInteractEntryData* InteractEntryData = NewObject<USGInteractEntryData>(this);
	InteractEntryData->Owner = this;
	InteractEntryData->DisplayText = InteractDisplayText;

	if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
	{
		if (USGHUDWidget* HUD = Subsystem->GetHUDInstance())
		{
			HUD->AddInteract(InteractEntryData);
		}
	}
}

void ASGInteractableActor::QueryEndInteract(AActor* OtherActor)
{
	if (OtherActor == InteractedActor.Get())
	{
		if (ASGPlayerController* Controller = Cast<ASGPlayerController>(InteractedActor->GetController()))
		{
			Unlock();
			DisableInput(Controller);
			if (const USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
			{
				if (USGHUDWidget* HUD = Subsystem->GetHUDInstance())
				{
					HUD->RemoveInteract(this);
				}
			}
		}
	}
}

void ASGInteractableActor::UnInteract_Implementation(ASGPlayerController* Controller)
{
	Unlock();
}

void ASGInteractableActor::Unlock()
{
	bInteracted = false;
}

void ASGInteractableActor::UnInteract_BP()
{
	if (InteractedActor.IsValid())
	{
		if (ASGPlayerController* Controller = Cast<ASGPlayerController>(InteractedActor->GetController()))
		{
			UnInteract(Controller);
		}
	}
}

void ASGInteractableActor::OnInteract(ASGPlayerController* Controller)
{
}

void ASGInteractableActor::OnFKeyPressed()
{
	if (bLockWhenInteract && bInteracted)
	{
		return;
	}
	
	if (InteractedActor.IsValid())
	{
		if (ASGPlayerController* Controller = Cast<ASGPlayerController>(InteractedActor->GetController()))
		{
			Interact(Controller);
		}
	}
}


void ASGInteractableActor::Interact_Implementation(ASGPlayerController* Controller)
{
	OnInteract(Controller);
	bInteracted = true;
}