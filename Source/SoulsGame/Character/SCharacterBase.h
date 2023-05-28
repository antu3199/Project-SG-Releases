// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "AbilitySystemInterface.h"
#include "SGPhaseComponent.h"
#include "SoulsGame/Abilities/CharacterAbilitySystemComponent.h"
#include "SoulsGame/Abilities/Attributes/MyAttributeSet.h"
#include "SoulsGame/Character/ConfectioniteCharacter.h"
#include "SoulsGame/DataAssets/WeaponAsset.h"
#include "GameFramework/Character.h"
#include "SoulsGame/SaveLoad/SGSaveRecord.h"
#include "SoulsGame/Weapon/WeaponActor.h"

#include "SCharacterBase.generated.h"


class UMyCharacterMovementComponent;
class UJumpSectionNS;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCharacterLevelUp, ASCharacterBase* Character, int32 NewLevel);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnCharacterNotifyHit, ASCharacterBase* Character, AActor* Other, const FHitResult& Hit);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterAppliedInitialEffects, ASCharacterBase*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeath, ASCharacterBase*, Character);


enum EBufferedInputType
{
	None = 0,
	Attack = 1,
	Ability = 2,
	Roll = 3,
};

USTRUCT()
struct FBufferedInput
{
	GENERATED_BODY()

	EBufferedInputType InputType = EBufferedInputType::None;
	bool StartBufferingInput = false;

	void SetBufferedAttackInput();
	void SetBufferedRollInput();
	void SetBufferedAbilityInput();

	void Reset();
	
};



// Note: IAbilitySystemInterface needed otherwise it won't listen to gameplay events
UCLASS(Blueprintable)
class SOULSGAME_API ASCharacterBase : public ACharacter, public IAbilitySystemInterface, public ISGSaveLoadInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacterBase(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual bool CanJumpInternal_Implementation() const override;

	virtual void SaveGame(struct FSGSaveRecord& SaveRecord) override;
	virtual void LoadGame(const struct FSGSaveRecord& LoadRecord) override;

	UFUNCTION(BlueprintCallable)
	bool GetIsDead() const;

   // Animation blueprint property
	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe)) // Needs to be BlueprintThreadSafe otherwise warning in ABP
	float GetAnimMovementSpeed() const;

	UFUNCTION(BlueprintCallable)
    void UpdateAnimMovementSpeed();

	UFUNCTION(BlueprintCallable)
	class UActorUIWidget* GetUIActorWidget() const;
	
	// Calls OnDamaged in the C++ side (Done in MyAttributeSet)
	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	void Debug_TestHit();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	TArray<TSubclassOf<UMyGameplayAbility>> DefaultAbilities;
	
	bool CanGetDamaged() const;

	virtual void UseAbility(const FString & AbilityTag);

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

	UCharacterAbilitySystemComponent* GetCharacterAbilitySystemComponent() const { return AbilitySystemComponent; }

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
	UMyCharacterMovementComponent * GetCharacterMovementComponent() const;
	void SetStopRootMotionMovement(bool Enabled);

	void SetStopRootMotionOnHit(bool Enabled);

	void SetRootMotionEnabled(bool Set);

	bool GetMontageCancellable() { return MontageCancellable; }

	// Helper to create a movetowards task
	class USBTTask_MoveTo* CreateAIMoveTowardsTask(const FVector& GoalLocation, float AcceptanceRadius = -1.f);

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

	UMyAttributeSet* GetAttributeSet() const
	{
		return AttributeSet;
	};

	void AddXp(int32 AmountAdded);

	void LevelUp();

	FOnCharacterLevelUp OnLevelUpDelegate;

	UPROPERTY(EditAnywhere, Category ="Loot")
	TSubclassOf<class ASGLootActor> SpawnedLootOnDeathTemplate = nullptr;

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

	virtual void OnAggroSet(bool bIsAggro, ASCharacterBase* OtherPawn);

	bool GetStartAggroed() const { return bStartAggroed; }

	UFUNCTION(BlueprintCallable)
	class USGPhaseComponent* GetOptionalPhaseComponent() const;
	
	
	FOnCharacterNotifyHit OnCharacterNotifyHit;
	
protected:

	int32 Temp_StaggerCount = 0;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;


	void InitializeAbilitySystem();
	void AddStartupGameplayAbilities();

	UFUNCTION()
	virtual void OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool AllowInputDuringMontage = false;
	bool MontageCancellable = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCharacterAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* DetectionRadius;
	
	UPROPERTY()
	int32 bAbilitiesInitialized;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* UIActorParent;
	
	bool IsDead;
	float AnimMovementSpeed;
	TArray<FGameplayAbilitySpecHandle> AbilitiesSpecHandles;

	// Note: Transient flag needed otherwise it's garbage collected. Not sure why...
	UPROPERTY(Transient)
	UMyAttributeSet* AttributeSet = nullptr;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UMyGameplayEffect>> PassiveGameplayEffects;


	UPROPERTY()
	UCharacterMovementComponent * CharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USGTeamComponent* TeamComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USGMovementOverrideComponent* MovementOverrideComponent = nullptr;

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


	int32 DebugHitboxId = -1;

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
