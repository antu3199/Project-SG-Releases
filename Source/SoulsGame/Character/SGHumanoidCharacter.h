// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SGCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "Components/TimelineComponent.h"
#include "SoulsGame/DataAssets/SGWeaponAsset.h"
#include "GameFramework/Character.h"
#include "SoulsGame/Weapon/SGWeaponActor.h"

#include "SGHumanoidCharacter.generated.h"


class USGCharacterMovementComponent;
class USGAnimNotifyState_JumpSection;

// "Playable chracter"
// Note: IAbilitySystemInterface needed otherwise it won't listen to gameplay events
UCLASS(Blueprintable)
class SOULSGAME_API ASGHumanoidCharacter : public ASGCharacterBase
{
	GENERATED_BODY()
	

public:
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;
	// Destroyed via the Destroy() function
	virtual void Destroyed() override;

	virtual void TestFunction_Implementation() override;
	
	ASGHumanoidCharacter(const FObjectInitializer& ObjectInitializer);
	USGWeaponAsset * GetWeaponAsset() const;

	UFUNCTION(BlueprintCallable)
	ASGWeaponActor* GetWeaponActor() const;

	UPROPERTY()
	ASGWeaponActor * WeaponActor = nullptr;
	
	virtual void MakeWeapon(const FVector Offset);

	UPROPERTY(EditAnywhere, Category="Socket")
	FName WeaponSocketName = TEXT("r_handSocket");

	virtual bool TriggerJumpSectionCombo();

	//  Set combo, or nullptr if want to disable combo.
	virtual void SetComboJumpSection(USGAnimNotifyState_JumpSection * JumpSection);
	bool CanBufferNextCombo() const;
	bool IsAttacking() const;


	virtual bool GetCanMove() const override;

	bool DoOnRoll();

	virtual bool DoMeleeAttack();
	virtual bool DoBAttack();
	void StopRolling();
	virtual void BufferJumpSectionForCombo();

protected:
	void InitializeItems();


	void OnJumpSectionCombo();


	UFUNCTION()
	void DashTimelineCallback(float Val);
    
	UFUNCTION()
	void DashTimelineFinishedCallback();

	void PlayDashTimeline();


	UPROPERTY(EditAnywhere, Category="Animation")
	UAnimMontage * OnRollMontage;

	UPROPERTY()
	UTimelineComponent* DashTimeline;
 
	UPROPERTY(EditAnywhere)
	UCurveFloat* DashSpeedCurve;

	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection;   

	UPROPERTY()
	float DashVal;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	FPrimaryAssetId WeaponAssetId;

	UPROPERTY()
	USGWeaponAsset * WeaponAsset = nullptr;

	UPROPERTY(EditAnywhere)
	FVector WeaponOffset;

	UPROPERTY()
	USGAnimNotifyState_JumpSection * JumpSectionNS = nullptr;
	
	FVector RollDirection;
	bool bIsDoingRoll = false;

	float OGGroundFriction = 0;

	bool bAbleToDash = true;
	float PreDashSpeed;

	UPROPERTY(EditAnywhere)
	float RollSpeed = 30;

	
};
