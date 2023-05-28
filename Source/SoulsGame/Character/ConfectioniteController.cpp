// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfectioniteController.h"

void AConfectioniteController::SetupInputComponent()
{
	Super::SetupInputComponent();

    FInputKeyBinding AbilityBinding1(EKeys::Z, IE_Pressed);
     AbilityBinding1.KeyDelegate.GetDelegateForManualSet().BindLambda( [this] ()
    {
    });
    this->InputComponent->KeyBindings.Add(AbilityBinding1);
}

AConfectioniteCharacter* AConfectioniteController::GetConfectioniteCharacter()
{
	return Cast<AConfectioniteCharacter>(GetPawn());
}
