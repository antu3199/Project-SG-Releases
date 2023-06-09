// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SGHumanoidPlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "SGPlayerController.generated.h"




/**
 * 
 */
UCLASS()
class SOULSGAME_API ASGPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ASGPlayerController();

	enum ECharacterActionPriority
	{
		Default = 0,
		Move = 1,
		Attack = 2,
		Roll = 3
	};

	virtual void OnPossess(APawn* InPawn) override;

	virtual void SetPlayer(UPlayer* InPlayer) override;

	ASGCharacterBase* GetPawnCharacter() const;
	ASGHumanoidCharacter* GetPawnPlayableCharacter() const;

	ASGHumanoidPlayerCharacter * GetPawnCharacterLegacy() const;

	void ShowUpgradeMenu();
	void UpdateHUD();
	void QueueNormalAttack();

	// TODO: Maybe change this later
	void Debug_QueueEmpoweredAttack();

	void QueueAction(TFunction<void()> Action, int32 ActionPriority = 0, bool ForceCloseMontage = false, ECharacterActionHint ActionHint = ECharacterActionHint::Hint_Default);
	TFunction<void()> GetQueuedAction() const { return QueuedAction; }

	void PlayQueuedAction();
	void ClearQueuedAction();

	void SetCanQueueNextMove(bool CanQueue)
	{
		bCanQueueNextMove = CanQueue;
	}

	bool GetCanQueueNextMove() const { return bCanQueueNextMove; }

	void PostBeginPlay();

	void TryUseQueuedMove();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Classes)
	TArray<TSubclassOf<APawn>> GeneratedPawnClasses;

	APawn* GetCurrentPawnBasedOnIndex() const;

	UFUNCTION(BlueprintCallable)
	FVector GetCameraForwardVector(float FocusPoint = 1000.0f) const;

	TWeakObjectPtr<ASGCharacterBase> GetBossActorPtr() const
	{
		return BossActorPtr;
	}

	UFUNCTION(BlueprintCallable)
	ASGCharacterBase* GetBossActor() const
	{
		if (BossActorPtr.IsValid())
		{
			return BossActorPtr.Get();
		}

		return nullptr;
	}

	void SetBossActor(ASGCharacterBase* Actor);

	void UnsetBossActor();
	
protected:

	UFUNCTION()
	void OnCharacterAppliedEffects(class ASGCharacterBase* AppliedEffectCharacter);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void UpdateRotation(float DeltaTime) override;

	// Called to bind functionality to input

	virtual void SetupInputComponent() override;

	void HandleCameraRotation();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> UpgradeMenuClass;

	void RotateCamera(float InputAxis);
	void MoveForward(float InputAxis);
	void MoveRight(float InputAxis);

	void OnKeyPressed(EKeys Key, EInputEvent EventType);


	// DEPRECATED: Add input binding using lambda. Causes runtime errrors
	void AddInputBinding(const FKey& Key, TFunction<void()> Callback);
	
	void OnFKeyPressed();

	void OnQKeyPressed();
	void OnEKeyPressed();
	void OnRKeyPressed();
	void OnTKeyPressed();

	
	void OnHKeyPressed();
	void OnGKeyPressed();
	void OnLeftControlPressed();
	void OnSpaceBarPressed();
	void OnLeftAltPressed();
	void OnZKeyPressed();
	void OnZKeyReleased();
	
	void OnXKeyPressed();
	void OnCKeyPressed();

	void OnZeroKeyPressed();
	
	void OnNumPadOneKeyPressed();
	void OnNumPadTwoKeyPressed();
	void OnNumPadThreeKeyPressed();
	void OnNumPadFourKeyPressed();
	void OnNumPadFiveKeyPressed();
	void OnNumPadSixKeyPressed();
	void OnNumPadSevenKeyPressed();
	
	void OnNumPadNineKeyPressed();
	void OnBackspaceKeyPressed();
	
	
	void OnRightMouseButtonPressed();
	void OnRightMouseButtonReleased();

	void OnOnePressed();
	void OnTwoPressed();
	void OnThreePressed();
	void OnFourPressed();

	/** Adds an inputcomponent to the top of the input stack. */
	virtual void PushInputComponent(UInputComponent* Input) override;

	/** Removes given inputcomponent from the input stack (regardless of if it's the top, actually). */
	virtual bool PopInputComponent(UInputComponent* Input) override;

	void SetupGlobalInputComponents(UInputComponent* Input);

	void OnLevelUp(class ASGCharacterBase* CharacterBase, int32 NewLevel);

	void SwitchPawnIndex(int Index);

	void QueueBeginAbility(const FString AbilityName, ECharacterActionHint ActionHint = ECharacterActionHint::Hint_Default, bool bRotateTowardsReticle = false);

	void QueueBeginSGAbility(const FGameplayTag& Tag, ECharacterActionHint ActionHint = ECharacterActionHint::Hint_Default, bool bRotateTowardsReticle = false);
	void QueueUseAbilityOnSlot(const int32 Index, ECharacterActionHint ActionHint = ECharacterActionHint::Hint_Default, bool bRotateTowardsReticle = false);
	

	FDelegateHandle LevelUpHandle;

	// Queue action stuff
	TFunction<void()> QueuedAction = nullptr;
	int QueuedActionPriority = 0;
	bool bCanQueueNextMove = false;
	bool bQueuedActionForceStopMontage = false;

	UPROPERTY(Transient)
	TArray<APawn*> GeneratedPawns;
	int CurrentPawnIndex = 0;

	bool bHasCameraLag = false;
	TSet<FString> UsedFallingEffects;


	TWeakObjectPtr<ASGCharacterBase> BossActorPtr;
	
};



