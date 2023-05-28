// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ConfectioniteCharacter.h"
#include "GameFramework/PlayerController.h"
#include "ConfectioniteController.generated.h"

/**
 * 
 */
UCLASS()
class SOULSGAME_API AConfectioniteController : public APlayerController
{
	GENERATED_BODY()

	
protected:

	virtual void SetupInputComponent() override;

	AConfectioniteCharacter * GetConfectioniteCharacter();

	
};
