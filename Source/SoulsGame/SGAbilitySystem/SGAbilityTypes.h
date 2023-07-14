#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"


#include "SGAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct SOULSGAME_API FSGAbilityActorInfo
{
	GENERATED_USTRUCT_BODY()

	virtual ~FSGAbilityActorInfo() {}

	/** The actor that owns the abilities, shouldn't be null */
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<AActor>	OwnerActor;

	/** The physical representation of the owner, used for targeting and animation. */
	/** Get "body" of task's owner / default, having location in world (e.g. Owner = AIController, Avatar = Pawn) */
	// In most cases, equal to owner
	UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
	TWeakObjectPtr<AActor>	AvatarActor;

	class USGAbilityComponent* GetAbilityComponent() const;
	class UAnimInstance* GetAnimInstance() const;

	/** Initializes the info from an owning actor. Will set both owner and avatar */
	virtual void InitFromActor(AActor *InOwnerActor, AActor *InAvatarActor);

	/** Clears out any actor info, both owner and avatar */
	virtual void ClearActorInfo();
};

/** Metadata for a tag-based Gameplay Event, that can activate other abilities or run ability-specific logic */
USTRUCT(BlueprintType)
struct SOULSGAME_API FSGAbilityEventData
{
	GENERATED_USTRUCT_BODY()

	FSGAbilityEventData()
		: Instigator(nullptr)
		, Target(nullptr)
		, bBroadcastOnce(true)
		, OptionalObject(nullptr)
		, OptionalObject2(nullptr)
	{
	}
	
	/** Tag of the event that triggered this */
	UPROPERTY(BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	FGameplayTag TriggerTag;
	
	// Tag for which if set, only triggers OnHandleEvent on abilities with that tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	FGameplayTag AssociatedAbilityTag;
	
	/** The instigator of the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	const AActor* Instigator;

	/** The target of the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	const AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	bool bBroadcastOnce;
	
	/** An optional ability-specific object to be passed though the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	const UObject* OptionalObject;

	/** A second optional ability-specific object to be passed though the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	const UObject* OptionalObject2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	int32 OptionalInt1 = -1;

	struct FAnimMontageInstance* MontageInstance = nullptr;
};

UENUM(BlueprintType)
enum class ESGEffectDurationType : uint8
{
	Instantaneous,
	StatusEffect,
};


USTRUCT(BlueprintType)
struct SOULSGAME_API FSGDamageParams
{
	GENERATED_USTRUCT_BODY()
protected:

	/** The instigator of the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "RequestedDamageCurve == nullptr"))
	float RequestedFlatDamage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RequestedFlatDamageCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "RequestedAttackScalingCurve == nullptr"))
	float RequestedAttackScaling = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* RequestedAttackScalingCurve = nullptr;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> DamageOwnerActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> DamageAvatarActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> DamageTarget = nullptr;

	/** An optional ability-specific object to be passed though the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USGAbility* OptionalAbility = nullptr;

	/** An optional ability-specific object to be passed though the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USGEffect* OptionalEffect = nullptr;
	
	/** An optional ability-specific object to be passed though the event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const UObject* OptionalObject = nullptr;

	// Modifiers used to effect damage logic
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer Modifiers;

	// Calculated final damage
	UPROPERTY(BlueprintReadWrite)
	float FinalFlatDamage = 0;

	UPROPERTY(BlueprintReadWrite)
	float FinalAttackScaling = 0;
	
	UPROPERTY(BlueprintReadWrite)
	bool bBaseDamageCalculated = false;
	
	void CalculateBaseDamage();
};