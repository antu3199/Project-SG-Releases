#pragma once
#include "CoreMinimal.h"
#include "SGCollisionProfile.h"
#include "SGConstants.h"
#include "SGTeam.h"
#include "Abilities/MyGameplayEffect.h"

#include "SHitboxManager.generated.h"

#define INVALID_HITBOX_ID  -1


typedef std::function<void(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)> FOnOverlapBeginCallback;
typedef std::function<void(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)> FOnOverlapEndCallback;
typedef std::function<void(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor)> FOnOverlapTickCallback;

// For now, lets just stick to box component
// Then, expand support for other hitbox types

UCLASS()
class UHitboxObject : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginDestroy() override;
	
	void SetOnOverlapBegin(const FOnOverlapBeginCallback& BeginCallback);
	void SetOnOverlapEnd(const FOnOverlapEndCallback& EndCallback);
	void SetOnOverlapTick(const FOnOverlapTickCallback& TickCallback, float TickInterval);
	
	void Tick(float DeltaTime);

	void RemoveFromActor();

	bool IsHitboxValid() const;

	UPROPERTY(Instanced)
	class UPrimitiveComponent* Component = nullptr;
	
	TWeakObjectPtr<UWorld> World = nullptr;

	int32 HitboxId = INVALID_HITBOX_ID;

	TWeakObjectPtr<AActor> Actor = nullptr;

	FVector PreviousLocation = FVector::ZeroVector;
	int32 DebugLevel = 1;
	FDateTime LastUpdateTime = FDateTime::Now();

	bool bExistingComponent = false;

	FSGCollisionParams CollisionParams;

	float OverlapTickInterval = -1.0f;
	FTimerHandle OverlapRefreshHandle;

	bool bIsAbleToDoOverlapTick = true;

	void RefreshOverlapTick();

private:
	
	FOnOverlapBeginCallback OnOverlapBeginCallback = nullptr;
	FOnOverlapEndCallback OnOverlapEndCallback = nullptr;
	FOnOverlapTickCallback OnOverlapTickCallback = nullptr;
	
};

UCLASS()
class USHitboxManager : public UObject, public FTickableGameObject 
{
public:
	GENERATED_BODY()
	static USHitboxManager* GetSingleton();

	// Note: Instead of hitbox ID, key by component?
	
	
	int32 CreateBox(USceneComponent* ParentComponent, const FString& Socket, const FVector& BoxExtents, EAttachmentRule Attachment = EAttachmentRule::KeepRelative, const FVector& TranslationOffset = FVector::ZeroVector, const FQuat& RotationOffset = FQuat::Identity);
	int32 CreateSphere(USceneComponent* ParentComponent, const FString& Socket, const float& Radius, EAttachmentRule Attachment = EAttachmentRule::KeepRelative, const FVector& TranslationOffset = FVector::ZeroVector, const FQuat& RotationOffset = FQuat::Identity);
	int32 CreateCapsule(USceneComponent* Parent, const FString& Socket, const float& HalfHeight, const float& Radius, EAttachmentRule Attachment = EAttachmentRule::KeepRelative, const FVector& TranslationOffset = FVector::ZeroVector, const FQuat& RotationOffset = FQuat::Identity);
	
	int32 GetHitboxIdFromComponent(UPrimitiveComponent* Component) const;
	
	int32 RegisterExistingComponent(UPrimitiveComponent* Component);


	virtual void Tick( float DeltaTime ) override;

	virtual bool IsTickableInEditor() const override
	{
		return true;
	}

	void RemoveHitbox(int32 HitboxId);

	virtual TStatId GetStatId() const override
	{
		return TStatId();
	}

	void SetCollisionProfile(int32 HitboxId, const FSGCollisionParams& CollisionParams);
	void AddOnOverlapBeginCallback(int32 HitboxId, FOnOverlapBeginCallback OnOverlapBeginCallback);
	void AddOnOverlapEndCallback(int32 HitboxId, FOnOverlapEndCallback OnOverlapEndCallback);
	void AddOnOverlapTickCallback(int32 HitboxId, FOnOverlapTickCallback OnOverlapTickCallback, float TickInterval = -1.0f);

	
	void SetDebugLevel(int32 HitboxId, ESGDebugCollisionLevel DebugLevel);
	
private:
	int32 CreateNewHitboxObject(UPrimitiveComponent* Component, UWorld* World, bool bExistingComponent);
	
	int32 GetHitboxIdFromPrimitiveComponent(UPrimitiveComponent* Component);

	
	
	int32 RunningHitboxId = 0;

	UPROPERTY(Transient)
	TMap<int32, TObjectPtr<UHitboxObject>> HitboxObjects;

	TSet<int32> ReusableHitboxIds;
};

