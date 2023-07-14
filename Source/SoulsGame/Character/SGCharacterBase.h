// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "AbilitySystemInterface.h"
#include "SGPhaseComponent.h"
#include "SoulsGame/DataAssets/SGWeaponAsset.h"
#include "GameFramework/Character.h"
#include "SoulsGame/SaveLoad/SGSaveRecord.h"
#include "SoulsGame/SGAbilitySystem/SGAbility.h"
#include "SoulsGame/Weapon/SGWeaponActor.h"
#include "SoulsGame/StaticSGData.h"
#include "SGCharacterBase.generated.h"


class USGCharacterMovementComponent;
class USGAnimNotifyState_JumpSection;
class ASGCharacterBase;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterLevelUp, ASGCharacterBase* Character, int32 NewLevel);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnCharacterNotifyHit, ASGCharacterBase* Character, AActor* Other, const FHitResult& Hit);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterAppliedInitialEffects, ASGCharacterBase*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeath, ASGCharacterBase*, Character);


enum ESGBufferedInputType
{
	None = 0,
	Attack = 1,
	Ability = 2,
	Roll = 3,
};

USTRUCT()
struct FSGBufferedInput
{
	GENERATED_BODY()

	ESGBufferedInputType InputType = ESGBufferedInputType::None;
	bool StartBufferingInput = false;

	void SetBufferedAttackInput();
	void SetBufferedRollInput();
	void SetBufferedAbilityInput();

	void Reset();
	
};



// Note: IAbilitySystemInterface needed otherwise it won't listen to gameplay events
UCLASS(Blueprintable)
class SOULSGAME_API ASGCharacterBase : public ACharacter, public ISGSaveLoadInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASGCharacterBase(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;


	virtual bool CanJumpInternal_Implementation() const override;

	virtual void OnSaveGame(struct FSGSaveRecord& SaveRecord) override;
	virtual void LoadGame(const struct FSGSaveRecord& LoadRecord) override;

	UFUNCTION(BlueprintCallable)
	bool GetIsDead() const;

	UFUNCTION(BlueprintNativeEvent)
	void TestFunction();
	
   // Animation blueprint property
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe)) // Needs to be BlueprintThreadSafe otherwise warning in ABP
	float GetAnimMovementSpeed() const;

	UFUNCTION(BlueprintCallable)
    void UpdateAnimMovementSpeed();

	UFUNCTION(BlueprintCallable)
	class USGActorUIWidget* GetUIActorWidget() const;
	
	// Calls OnDamaged in the C++ side (Done in MyAttributeSet)
	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	virtual void HandleSGDamage(const FSGDamageParams& DamageParams, float Damage, float NewHealth);

	UFUNCTION()
	virtual void HandleStatChanged(FGameplayTag StatTag, FSGStatValues OldStat, FSGStatValues NewStat);

	virtual void HandleHealthChanged(const FSGStatValues& OldStat, const FSGStatValues& NewStat);
	
	void Debug_TestHit();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, DisplayName="Default SGAbilities")
	TArray<TSubclassOf<USGAbility>> m_DefaultSGAbilities;
	
	bool CanGetDamaged() const;

	UPROPERTY(EditAnywhere, Category="Animation")
	UAnimMontage * OnHitMontage;

	UPROPERTY(EditAnywhere, Category="Animation")
	UAnimMontage * OnDeathMontage;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterAppliedInitialEffects OnCharacterAppliedInitialEffects;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterDeath OnCharacterDeath;

	void SetCanMove(bool Set);
	virtual bool GetCanMove() const;

	void SetDisableInputForNonRootMotion(bool Set)
	{
		bDisableInputForNonRootMotion = Set;
	}

	bool GetDisableInputForNonRootMotion() const
	{
		return bDisableInputForNonRootMotion;
	}

	void StopPlayingMontage();

	bool IsPlayingMontage() const;
	bool HasAppliedInitialEffects() const { return bHasAppliedInitialEffects; }

	// Direction
	void SetForwardBackDirectionVector(const FVector Forward, const float InputAxis);
	void SetRightLeftDirectionVector(const FVector Right, const float InputAxis);
	
	FVector GetDirectionVector() const;
	void RotateTowardsDirection();

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
	USGCharacterMovementComponent * GetCharacterMovementComponent() const;
	void SetStopRootMotionMovement(bool Enabled);

	void SetStopRootMotionOnHit(bool Enabled);

	void SetRootMotionEnabled(bool Set);

	bool GetMontageCancellable() { return MontageCancellable; }

	// Helper to create a movetowards task
	class USGBTTask_MoveTo* CreateAIMoveTowardsTask(const FVector& GoalLocation, float AcceptanceRadius = -1.f);

	void DoJump();

	// On landed
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintPure)
    float GetHealthPercent() const;

	UFUNCTION(BlueprintPure)
	float GetXpPercent() const;
	
	UFUNCTION(BlueprintPure)
	float GetManaPercent() const;
	
	UPROPERTY()
	USceneComponent* FocusTarget = nullptr;

	USceneComponent* GetFocusTarget() const { return FocusTarget; }
	void SetFocusTarget(USceneComponent* Component) { FocusTarget = Component; }

	UPROPERTY(EditAnywhere)
	int32 Temp_StaggerLimit = 20;
	
	void ToggleGodMode();
	void SetGodMode(bool Set);


	void LevelUp();

	FOnCharacterLevelUp OnLevelUpDelegate;

	UPROPERTY(EditAnywhere, Category ="Loot")
	TObjectPtr<USGRewardData> LootOnDeath;

	UPROPERTY(EditAnywhere)
	float TimeBeforeDespawn = 3.0f;

	virtual void Falling() override;
	
	void SetAllowInputDuringMontage(const bool & Set) { bAllowInputDuringMontage = Set; }
	bool GetAllowInputDuringMontage() { return bAllowInputDuringMontage; }

	void SetMontageCancellable(bool bEnabled) { bMontageCancellable = bEnabled; }
	void SetOverrideRotation(bool bEnabled) { bOverrideRotation = bEnabled; }

	class USGTeamComponent* GetTeamComponent() const;
	class USGMovementOverrideComponent* GetMovementOverrideComponent() const;

	float GetFinalMontageSpeed() const;

	void SetCurrentMontageSpeed(float Value);
	float GetCurrentMontageSpeed() const;

	void SetMontageSpeedMultiplier(float Value);
	float GetMontageSpeedMultiplier() const;

	bool GetAllowSetMontageSpeed() const;
	void SetAllowSetMontageSpeed(bool Value);

	virtual void OnAggroSet(bool bIsAggro, ASGCharacterBase* OtherPawn);

	bool GetStartAggroed() const { return bStartAggroed; }

	UFUNCTION(BlueprintCallable)
	class USGPhaseComponent* GetOptionalPhaseComponent() const;
	
	
	FOnCharacterNotifyHit OnCharacterNotifyHit;
	
protected:

	int32 Temp_StaggerCount = 0;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;


	void AddStartupGameplayAbilities();

	void OnDeath();

	UFUNCTION()
	virtual void OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool AllowInputDuringMontage = false;
	bool MontageCancellable = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* DetectionRadius;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USGAbilityComponent * SGAbilityComponent;

	
	UPROPERTY()
	int32 bAbilitiesInitialized;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* UIActorParent;
	
	bool IsDead;
	float AnimMovementSpeed;

	UPROPERTY()
	UCharacterMovementComponent * CharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USGTeamComponent* TeamComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USGMovementOverrideComponent* MovementOverrideComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USGStatComponent* StatComponent = nullptr;

	UPROPERTY(EditAnywhere)
	float MaxKnockbackImpactVelocity = 1000.0f;

	UPROPERTY(EditAnywhere, Category="AI")
	bool bStartAggroed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetComponent * UIActorWidgetComponent;

	UPROPERTY(EditAnywhere)
	int32 Debug_MagicNumber = 1234;

	UPROPERTY(Transient)
	class USGPhaseComponent* OptionalPhaseComponent = nullptr;
	
	bool CanMove = true;

	bool GetRootMotionEnabled() const;

	FVector ForwardBackDirectionVector;
	FVector RightLeftDirectionVector;

	bool StopRootMotionOnHit = false;

	bool bAllowInputDuringMontage = false;
	bool bMontageCancellable = false;

	bool bOverrideRotation = false;

	UPROPERTY(EditAnywhere)
	float CharacterOverrideRotationRate = 0.05f;

	float CurrentMontageSpeed = 1.0f;
	float MontageSpeedMultiplier = 1.0f;
	bool bAllowSetMontageSpeed = true;

	bool bDisableInputForNonRootMotion = false;
	bool bHasAppliedInitialEffects = false;


};
