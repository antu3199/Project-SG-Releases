#pragma once

#include "CoreMinimal.h"
#include "SoulsGame/SGHandleGenerator.h"
#include "SoulsGame/SGTeam.h"
#include "SoulsGame/SGAbilitySystem/SGEffect.h"

#include "SGHitboxSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FSGHitboxParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSGCollisionParams CollisionParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TickInterval = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSGEffectInstigatorContext EffectContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStartEnabled = true;
};

USTRUCT(BlueprintType)
struct FSGHitboxOverlapContext
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UPrimitiveComponent> OverlappedComponent;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> OtherActor;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UPrimitiveComponent> OtherComp;

	UPROPERTY(BlueprintReadOnly)
	bool bAppliedEffect = false;
	
};

// Should be a struct, but needs to be a UObject to bind UFUNCTIONs
UCLASS()
class USGHitboxObject : public UObject
{
	GENERATED_BODY()

	friend class USGHitboxSubsystem;
	
public:
	virtual void Init(int32 InHitboxId, const FSGHitboxParams& InParams, UPrimitiveComponent* PrimitiveComponent, bool bShouldOwnComponent);

	UFUNCTION(BlueprintCallable)
	int32 GetHitboxId() const { return HitboxId; }

	UFUNCTION(BlueprintCallable)
	FSGHitboxParams GetHitboxParams() const { return Params; }

	UFUNCTION(BlueprintCallable)
	void SetEnabled(bool Set);

	UFUNCTION(BlueprintCallable)
	void SetEffectContext(const FSGEffectInstigatorContext& Context);

	void RequestDestroy();
	
	// Just keep them public because I'm lazy
	int32 HitboxId;
	FSGHitboxParams Params;
	
private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CheckOverlap();
	void OnTick(float DeltaTime);

	UPrimitiveComponent* GetPrimitiveComponent() const;

	UPROPERTY(Transient)
	UPrimitiveComponent* OwnedPrimitiveComponent;

	TWeakObjectPtr<UPrimitiveComponent> ExternalPrimitiveComponent;
	
	TArray<FSGHitboxOverlapContext> OverlapContexts;

	float LastTriggeredTime = -1;
	bool bEnabled = true;
	
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHitboxTriggered, USGHitboxObject*, HitboxObject, const FSGHitboxOverlapContext&, OverlapContext);

UCLASS()
class USGHitboxSubsystem : public UGameInstanceSubsystem, public FTickableGameObject 
{
	GENERATED_BODY()

	friend class USGHitboxObject;

public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickableInEditor() const override
	{
		return true;
	}

	virtual TStatId GetStatId() const override
	{
		return TStatId();
	}

	UFUNCTION(BlueprintCallable)
	int32 CreateBox(FSGHitboxParams HitboxParams, USceneComponent* ParentComponent, const FString& Socket, const FVector& BoxExtents, EAttachmentRule Attachment = EAttachmentRule::KeepRelative, const FVector& TranslationOffset = FVector::ZeroVector, const FRotator& RotationOffset = FRotator::ZeroRotator);

	UFUNCTION(BlueprintCallable)
	int32 RegisterExistingComponent(FSGHitboxParams HitboxParams, UPrimitiveComponent* Component);

	UFUNCTION(BlueprintCallable)
	void RemoveHitbox(int32 HitboxId);

	UFUNCTION(BlueprintCallable)
	USGHitboxObject* GetHitbox(int32 HitboxId);

	UPROPERTY(BlueprintAssignable)
	FOnHitboxTriggered OnHitboxTriggered;
	
private:
	int32 CreateNewHitboxObject(const FSGHitboxParams& Params, UPrimitiveComponent* Component, UWorld* World, bool bOwnsComponent);
	
	void TriggerHitbox(USGHitboxObject* HitboxObject, const FSGHitboxOverlapContext& OverlapContext);

	UPROPERTY(Transient)
	TMap<int32, TObjectPtr<USGHitboxObject>> HitboxObjects;


	FSGHandleGenerator HandleGenerator;
};
