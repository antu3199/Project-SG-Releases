
#pragma once

#include "EditorSubsystem.h"
#include "GameplayTagContainer.h"

#include "SGStaticDataEditorSubsystem.generated.h"

class FObjectPreSaveContext;

/**
 * Editor Subsystem that creates and updates Checkpoints
 */
UCLASS()
class SOULSGAMEEDITOR_API  USGStaticDataEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	static void RegenerateAll();

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override final;
	virtual void Deinitialize() override final;

	void OnObjectPreSave(UObject* InAsset, FObjectPreSaveContext InPreSaveContext);
	void OnObjectsDeleted(AActor* Actor);

	void UpdateActor(const class ASavePointActor& SavePoint);


private:
	void OnPresave(UObject* Asset);

	void UpdateSavepointTransform(const FGameplayTag& Tag, const struct FSavePointTransformData& Data);
	class UStaticSGData* GetStaticDataFromPath();

	
	UPROPERTY(Transient)
	TObjectPtr<class UStaticSGData> m_CheckPointData;

	FString LastLevelName;

	FDelegateHandle m_OnObjectSaveHandle;
	FDelegateHandle m_OnObjectDeletedHandle;
};