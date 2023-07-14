#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTaskOwnerInterface.h"
#include "SGAbilityTypes.h"
#include "SGEffect.h"
#include "SGStatComponent.h"
#include "SoulsGame/SGHandleGenerator.h"
#include "SoulsGame/SGTeam.h"

#include "SGAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSGOnAbilityEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSGOnAbilityMontageEnd, int32, InstanceHandle);

UCLASS(BlueprintType, Blueprintable)
class SOULSGAME_API USGAbility : public UObject, public IGameplayTaskOwnerInterface
{
	GENERATED_BODY()

	friend class USGAbilityComponent;

public:
	
	static USGAbility* NewSGAbility(UObject* Outer, TSubclassOf<USGAbility> Ability, int32 InstanceHandle, AActor* Owner, AActor* AvatarActor = nullptr, int32 Level = 1);

	// Flow

	// Note: Does this have to be BlueprintNative?
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool RequestActivate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CheckCost(const USGAbilityComponent* RequestedComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ApplyCost();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanActivate(const class USGAbilityComponent* RequestedComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FSGEffectInstigatorContext CreateEffectInstigatorContextForAbility(FGameplayTag ContextTag);

	UFUNCTION(BlueprintCallable)
	void RequestEndAbility();

	UFUNCTION(BlueprintCallable, Category="Cooldown")
	void RequestCooldownStart();

	UFUNCTION(BlueprintCallable)
	void RequestStartDurationTimer();
	
	// State query:
	UFUNCTION(BlueprintCallable)
	bool IsActive() const { return bIsActive; }

	UFUNCTION(BlueprintCallable)
	bool IsUnderCooldown() const;

	UFUNCTION(BlueprintCallable)
	float GetCurrentCooldown() const;

	UFUNCTION(BlueprintCallable)
	float GetAbilityActivationCooldown() const;
	
	UFUNCTION(BlueprintCallable)
	const FGameplayTag& GetIdentifierTag() const { return IdentifierTag; }

	UFUNCTION(BlueprintCallable)
	void SetLevel(int32 InLevel);

	UFUNCTION(BlueprintCallable)
	int32 GetLevel() const;

	UFUNCTION(BlueprintCallable)
	void BroadcastOnAbilityMontageEnd();

	
	UAnimMontage* GetCurrentMontage() const { return Montage; }
	void SetCurrentMontage(UAnimMontage* InMontage);

	const FSGAbilityActorInfo& GetActorInfo() const { return ActorInfo; }
	bool GetIsCDO() const { return bIsCDO; }
	bool GetCreateInstancedOnActivation() const { return CreateInstanceOnActivation; }

	void SetAvatarActor(AActor* Other);
	
	UPROPERTY(BlueprintAssignable)
	FSGOnAbilityEnd OnAbilityEnd;

	UPROPERTY(BlueprintAssignable)
	FSGOnAbilityMontageEnd OnAbilityMontageEnd;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer AbilityTypeTags;

	// --------------------------------------
	//	IGameplayTaskOwnerInterface
	// --------------------------------------	
	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
	virtual AActor* GetGameplayTaskOwner(const UGameplayTask* Task) const override;
	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;
	virtual void OnGameplayTaskInitialized(UGameplayTask& Task) override;
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;

	// AI Helpers
	float GetMaxRangeToActivate() const;
	float GetMinRangeToActivate() const;


	// Need to create own spawn object from class function because not a AActor
	UFUNCTION(BlueprintCallable)
	AActor* SpawnActorFromClass(TSubclassOf<AActor> ActorClass, FVector Location = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator);

	virtual UWorld* GetWorld() const override;
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnActivate();

	UFUNCTION(BlueprintNativeEvent)
	void OnEndAbility();
	
	UFUNCTION(BlueprintNativeEvent)
    void OnHandleEvent(const FSGAbilityEventData& Payload);

	UFUNCTION(BlueprintNativeEvent)
	void OnGiveAbility();

	UFUNCTION(BlueprintNativeEvent)
	void OnDurationTimerExpired();
	
	void DoActivate();
	float GetLastActivationTime() const;
	
	UPROPERTY(EditAnywhere, DisplayName="Identifier Tag", meta = (DisplayPriority = 1, Categories = "Ability"))
	FGameplayTag IdentifierTag;

	UPROPERTY(EditAnywhere, DisplayName="Apply Cost On Activate")
	bool ApplyCostOnActivate = true;

	UPROPERTY(EditAnywhere, Category="Cooldown/Duration", DisplayName="Cooldown")
	float Cooldown = 0.0f;

	UPROPERTY(EditAnywhere, Category= "Cooldown/Duration", meta = (EditCondition = "Cooldown != 0.0f && bRequestCooldownOnAbilityEnd == false"))
	bool bRequestCooldownOnActivate = false;

	UPROPERTY(EditAnywhere, Category= "Cooldown/Duration", meta = (EditCondition = "Cooldown != 0.0f && bRequestCooldownOnActivate == false"))
	bool bRequestCooldownOnAbilityEnd = true;
	
	// If true, creates an instance of this ability when you activate it
	// If false, creates instance of ability when you give the ability, and reuse it.
	UPROPERTY(EditAnywhere, DisplayName="Create Instance on Activation")
	bool CreateInstanceOnActivation = true;

	UPROPERTY(EditAnywhere, Category="Cooldown/Duration", meta=(DisplayName = "Duration (-1 for manual)"))
	float Duration = -1.0f;

	UPROPERTY(EditAnywhere, Category="Cooldown/Duration")
	bool bStartDurationTimerOnActivate = true;

	UPROPERTY(EditAnywhere, Category="Cooldown/Duration")
	bool bEndAbilityOnDurationExpired = true;

	// Minimum distance to activate. For melee abilities, this should be -1. For range, it should be whatever else
	// About 150 is a good melee attack
	UPROPERTY(EditAnywhere, Category= "AI", meta=(DisplayName = "MaxRangeToActivate (Range for Melee attacks)"))
	float MaxRangeToActivate = -1.0f;

	// For ranged abilities
	UPROPERTY(EditAnywhere, Category= "AI", meta=(DisplayName = "MinRangeToActivate (Min range for ranged attacks)"))
	float MinRangeToActivate = -1.0f;

	UPROPERTY(BlueprintReadOnly)
	FSGAbilityActorInfo ActorInfo;
	
	UPROPERTY()
	UAnimMontage* Montage;

	/** List of currently active tasks, do not modify directly */
	UPROPERTY()
	TArray<UGameplayTask*>	ActiveTasks;

	FTimerHandle DurationTimerHandle;
	
	// Instanced ability handle. Should not be invalid!
	int32 InstancedAbilityHandle = FSGHandleGenerator::Handle_Invalid;
	
	int32 Level = 1;
	
	// Whether or not this is an instanced ability (Created using NewObject)
	bool bIsCDO = true;
	
	bool bIsActive = false;
};
