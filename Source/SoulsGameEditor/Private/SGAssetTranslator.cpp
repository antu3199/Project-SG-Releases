#include "SoulsGameEditor/Public/SGAssetTranslator.h"

#include "AssetSelection.h"
#include "GameplayTagsManager.h"
#include "NiagaraSystem.h"
#include "Animation/AnimMontage.h"
#include "SoulsGame/Animation/SGAnimNotifyState_CreateHitboxBox.h"
#include "SoulsGame/Animation/SGAnimNotifyState_CreateHitboxCapsule.h"
#include "SoulsGame/Animation/SGAnimNotifyState_CreateHitboxSphere.h"
#include "SoulsGame/Animation/SGAnimNotifyState_TimedNiagaraEffect.h"
#include "SoulsGame/Animation/SGAnimNotifyState_SendGameplayTagEvent.h"
#include "SoulsGame/Tools/SGAssetComponent.h"
#include "SoulsGameEditor/Public/SGAnimationFileReaderStruct.h"
#include "SoulsGameEditor/Public/SGEditorUtils.h"
#include "SoulsGame/SGCollisionProfile.h"

bool FSGAssetTranslator::TranslateMontageData(USGAssetComponent* InSAC)
{
	if (InSAC == nullptr || InSAC->JsonObjectPath.IsEmpty() || InSAC->InputObject == nullptr)
	{
		return false;
	}

	UAnimMontage* AnimMontage = Cast<UAnimMontage>(InSAC->InputObject);
	if (AnimMontage == nullptr)
	{
		return false;
	}

	
	FString ContentDir = FPaths::ProjectContentDir();
	FString JsonFilePath = FPaths::Combine(ContentDir, InSAC->JsonObjectPath);
	FSAnimationFileReaderStruct AnimationFileStruct;
	FSGEditorUtils::ReadFromJson(JsonFilePath, AnimationFileStruct);

	FSAnimationFileReaderStructActions Actions;
	TranslateAnimationFileReaderStructToActions(AnimationFileStruct, Actions);

	/*
	UClass* NotifyClass = FSEditorUtils::FindClass("MyAnimNotifyState");
	//UClass* NotifyClass = UMyAnimNotifyState::StaticClass();
	if (NotifyClass == nullptr)
	{
		return false;
	}
	
	FAnimNotifyEvent& NotifyEvent = CreateNewNotify(AnimMontage, "Test notify name", NotifyClass, 0, 0, 1);
	*/

	ApplyActionsToMontageData(AnimMontage, Actions);
	
	AnimMontage->RefreshCacheData();
	return AnimMontage->MarkPackageDirty();
}

FAnimNotifyEvent& FSGAssetTranslator::CreateNewNotify(UAnimSequenceBase* Sequence, FString NewNotifyName, UClass* NotifyClass, int32 TrackIndex,  float StartTime, float NotifyStateDuration)
{
	// Insert a new notify record and spawn the new notify object
	int32 NewNotifyIndex = Sequence->Notifies.Add(FAnimNotifyEvent());
	FAnimNotifyEvent& NewEvent = Sequence->Notifies[NewNotifyIndex];
	NewEvent.NotifyName = FName(*NewNotifyName);
	NewEvent.Guid = FGuid::NewGuid();

	NewEvent.Link(Sequence, StartTime);
	NewEvent.TriggerTimeOffset = GetTriggerTimeOffsetForType(Sequence->CalculateOffsetForNotify(StartTime));
	NewEvent.TrackIndex = TrackIndex;

	if( NotifyClass )
	{
		class UObject* AnimNotifyClass = NewObject<UObject>(Sequence, NotifyClass, NAME_None, RF_Transactional);
		NewEvent.NotifyStateClass = Cast<UAnimNotifyState>(AnimNotifyClass);
		NewEvent.Notify = Cast<UAnimNotify>(AnimNotifyClass);

		if( NewEvent.NotifyStateClass )
		{
			// Set default duration to 1 frame for AnimNotifyState.
			NewEvent.SetDuration(NotifyStateDuration);
			NewEvent.EndLink.Link(Sequence, NewEvent.EndLink.GetTime());
			NewEvent.TriggerWeightThreshold = NewEvent.NotifyStateClass->GetDefaultTriggerWeightThreshold();
		}
		else if ( NewEvent.Notify )
		{
			NewEvent.TriggerWeightThreshold = NewEvent.Notify->GetDefaultTriggerWeightThreshold();
		}
	}
	else
	{
		NewEvent.Notify = NULL;
		NewEvent.NotifyStateClass = NULL;
	}

	if(NewEvent.Notify)
	{
		TArray<FAssetData> SelectedAssets;
		AssetSelectionUtils::GetSelectedAssets(SelectedAssets);

		for( TFieldIterator<FObjectProperty> PropIt(NewEvent.Notify->GetClass()); PropIt; ++PropIt )
		{
			if(PropIt->GetBoolMetaData(TEXT("ExposeOnSpawn")))
			{
				FObjectProperty* Property = *PropIt;
				const FAssetData* Asset = SelectedAssets.FindByPredicate([Property](const FAssetData& Other)
				{
					return Other.GetAsset()->IsA(Property->PropertyClass);
				});

				if( Asset )
				{
					uint8* Offset = (*PropIt)->ContainerPtrToValuePtr<uint8>(NewEvent.Notify);
					(*PropIt)->ImportText( *Asset->GetAsset()->GetPathName(), Offset, 0, NewEvent.Notify );
					break;
				}
			}
		}

		NewEvent.Notify->OnAnimNotifyCreatedInEditor(NewEvent);
	}
	else if (NewEvent.NotifyStateClass)
	{
		NewEvent.NotifyStateClass->OnAnimNotifyCreatedInEditor(NewEvent);
	}

	Sequence->PostEditChange();
	Sequence->MarkPackageDirty();

	return NewEvent;
}

bool FSGAssetTranslator::TranslateAnimationFileReaderStructToActions(const FSAnimationFileReaderStruct& FrameInfo,
	FSAnimationFileReaderStructActions& OutActions)
{
	// Only care about data frames
	TArray<FSGAnimationFileReaderEvent> Events = FrameInfo.Events.FilterByPredicate([=](const FSGAnimationFileReaderEvent& A)
	{
		return A.bHasData;
	});

	int32 FrameEventsLen = Events.Num();
	if (FrameEventsLen == 0)
	{
		return true;
	}

	const int32 NumNotifyTracks = Events[0].AnimNotifies.Num();

	for (int32 NotifyTrackIndex = 0; NotifyTrackIndex < NumNotifyTracks; NotifyTrackIndex++)
	{
		FSGAnimationFileReaderEvent* LastFrameEvent = nullptr;
		for (int32 CurrentFrame = 0; CurrentFrame < FrameEventsLen; CurrentFrame++)
		{
			FSGAnimationFileReaderEvent* CurrentFrameEvent = &Events[CurrentFrame];
			check(CurrentFrameEvent != nullptr);

			if (NotifyTrackIndex >= CurrentFrameEvent->AnimNotifies.Num())
			{
				continue;
			}
			
			FSGAnimationFileReaderAnimNotify& CurrentEventNotify = CurrentFrameEvent->AnimNotifies[NotifyTrackIndex];

			if ((LastFrameEvent != nullptr && CurrentFrame + 1 >= FrameEventsLen) || // Last frame, but outstanding event
			(LastFrameEvent != nullptr && !LastFrameEvent->AnimNotifies[NotifyTrackIndex].IsEqualTo(CurrentEventNotify))) // Switch event
			{
				// Add action at current frame
				OutActions.Actions.Emplace(FSGAnimationFileReaderStructNotifyAction());
				FSGAnimationFileReaderStructNotifyAction & Action = OutActions.Actions.Last();
				Action.StartTime = FSGEditorUtils::FramesToTime(LastFrameEvent->Frame);
				Action.Duration = FSGEditorUtils::FramesToTime(CurrentFrameEvent->Frame - LastFrameEvent->Frame);
				Action.TrackIndex = NotifyTrackIndex;
				Action.RawEventName = LastFrameEvent->AnimNotifies[NotifyTrackIndex].EventName;
				Action.RawEventArgs = LastFrameEvent->AnimNotifies[NotifyTrackIndex].EventArgs;

				if (!CurrentEventNotify.IsEventEmpty())
				{
					LastFrameEvent = CurrentFrameEvent;
				}
				else
				{
					LastFrameEvent = nullptr;
				}
			}
			else if (LastFrameEvent == nullptr && !CurrentEventNotify.IsEventEmpty())
			{
				// Start of event
				LastFrameEvent = CurrentFrameEvent;
			}
			
		}
	}
	
	return true;
}

bool FSGAssetTranslator::ApplyActionsToMontageData(UAnimMontage* Montage,
                                                  const FSAnimationFileReaderStructActions& Actions)
{
	// TODO:
	// - Add ability to choose composite sections using Montage->GetSectionStartAndEndTime
	const bool bDeleteAutoGenerated = true;

	if (bDeleteAutoGenerated)
	{
		Montage->Notifies = Montage->Notifies.FilterByPredicate([=](const FAnimNotifyEvent& A)
		{
			if (USGAnimNotifyState* MyNotifyState = Cast<USGAnimNotifyState>(A.NotifyStateClass))
			{
				if (MyNotifyState->bIsAutoGenerated)
				{
					return false;
				}
			}

			if (USGAnimNotifyState_TimedNiagaraEffect* NiagaraNS = Cast<USGAnimNotifyState_TimedNiagaraEffect>(A.NotifyStateClass))
			{
				if (NiagaraNS->bIsAutoGenerated)
				{
					return false;
				}
			}
		
			return true;
		});
	}

	for (const FSGAnimationFileReaderStructNotifyAction& Action : Actions.Actions)
	{
		UClass* NotifyClass = FSGEditorUtils::FindClass(Action.RawEventName);
		if (NotifyClass == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("[FSAssetTranslator::ApplyActionsToMontageData] Unable to find action name: %s"), *Action.RawEventName);
			return false;
		}

		FAnimNotifyEvent& NotifyEvent = CreateNewNotify(Montage, Action.RawEventName, NotifyClass, Action.TrackIndex, Action.StartTime, Action.Duration);

		TObjectPtr<class UAnimNotify> Notify = NotifyEvent.Notify;
		TObjectPtr<class UAnimNotifyState>& NotifyState = NotifyEvent.NotifyStateClass;


		if (Notify != nullptr)
		{
			
		}
		else if (NotifyState != nullptr)
		{
			if (USGAnimNotifyState* MyNotifyState = Cast<USGAnimNotifyState>(NotifyState))
			{
				MyNotifyState->bIsAutoGenerated = true;
			}
			else if (USGAnimNotifyState_TimedNiagaraEffect* NiagaraNS = Cast<USGAnimNotifyState_TimedNiagaraEffect>(NotifyState))
			{
				NiagaraNS->bIsAutoGenerated = true;
			}

			const int32 NumArgs = Action.RawEventArgs.Num();
			int32 CurArgIndex = 0;

			if (USGAnimNotifyState_SendGameplayTagEvent* GameplayTagNS = Cast<USGAnimNotifyState_SendGameplayTagEvent>(NotifyState))
			{
				FString Arg_Start = Action.RawEventArgs[CurArgIndex++];

				FGameplayTag StartingTag = UGameplayTagsManager::Get().RequestGameplayTag(FName(Arg_Start));
				GameplayTagNS->GameplayTag = StartingTag;

				if (NumArgs >= CurArgIndex + 1)
				{
					FString Arg_End = Action.RawEventArgs[CurArgIndex++];
					FGameplayTag EndingTag = UGameplayTagsManager::Get().RequestGameplayTag(FName(Arg_End));
					GameplayTagNS->OnEndGameplayTag = EndingTag;
				}
			}
			else if (USGAnimNotifyState_TimedNiagaraEffect* NiagaraNS = Cast<USGAnimNotifyState_TimedNiagaraEffect>(NotifyState))
			{
				//FString TestPath = "NiagaraSystem'/Game/ParticleFX/UsedParticles/P_SG_Fire_DashTrail.P_SG_Fire_DashTrail'";
				FString Arg_PSPath = Action.RawEventArgs[CurArgIndex++];
				UNiagaraSystem* NiagaraSystem = FSGEditorUtils::LoadObjFromPath<UNiagaraSystem>(FName(Arg_PSPath));
				NiagaraNS->Template = NiagaraSystem;

				if (NumArgs >= CurArgIndex + 1)
				{
					FString Arg_PS_Socket = Action.RawEventArgs[CurArgIndex++];
					NiagaraNS->SocketName = FName(Arg_PS_Socket);
				}

				if (NumArgs >= CurArgIndex + 1)
				{
					FVector Vector = GetVectorFromStr(Action.RawEventArgs[CurArgIndex++]);
					NiagaraNS->SpawnScale = Vector;
				}
				else
				{
					// Default to (0.1,0.1,0.1)
					NiagaraNS->SpawnScale = FVector::OneVector * 0.1f;
				}


			}
			else if (USGAnimNotifyState_CreateHitboxBox* HitboxBox = Cast<USGAnimNotifyState_CreateHitboxBox>(NotifyState))
			{

				if (NumArgs >= CurArgIndex + 1)
				{
					FString Arg_PS_Socket = Action.RawEventArgs[CurArgIndex++];
					HitboxBox->SocketName = FName(Arg_PS_Socket);
				}
				
				if (NumArgs >= CurArgIndex + 1)
				{
					ESGCollisionProfileType ProfileType = FSGCollisionProfileHelper::GetCollisionProfileNameFromString(Action.RawEventArgs[CurArgIndex++]);
					FSGCollisionParams Params;
					Params.ProfileType = ProfileType;
					HitboxBox->CollisionParams = Params;
				}
				
				if (NumArgs >= CurArgIndex + 1)
				{
					FVector Vector = GetVectorFromStr(Action.RawEventArgs[CurArgIndex++]);
					HitboxBox->BoxExtents = Vector;
				}
			}
			else if (USGAnimNotifyState_CreateHitboxSphere* HitboxSphere = Cast<USGAnimNotifyState_CreateHitboxSphere>(NotifyState))
			{

				if (NumArgs >= CurArgIndex + 1)
				{
					FString Arg_PS_Socket = Action.RawEventArgs[CurArgIndex++];
					HitboxSphere->SocketName = FName(Arg_PS_Socket);
				}
				
				if (NumArgs >= CurArgIndex + 1)
				{
					ESGCollisionProfileType ProfileType = FSGCollisionProfileHelper::GetCollisionProfileNameFromString(Action.RawEventArgs[CurArgIndex++]);
					FSGCollisionParams Params;
					Params.ProfileType = ProfileType;
					HitboxSphere->CollisionParams = Params;
				}
				
				if (NumArgs >= CurArgIndex + 1)
				{
					float Radius = FCString::Atof(*Action.RawEventArgs[CurArgIndex++]);
					HitboxSphere->Radius = Radius;
				}
			}
			else if (USGAnimNotifyState_CreateHitboxCapsule* HitboxCapsule = Cast<USGAnimNotifyState_CreateHitboxCapsule>(NotifyState))
			{
				if (NumArgs >= CurArgIndex + 1)
				{
					FString Arg_PS_Socket = Action.RawEventArgs[CurArgIndex++];
					HitboxCapsule->SocketName = FName(Arg_PS_Socket);
				}
				
				if (NumArgs >= CurArgIndex + 1)
				{
					ESGCollisionProfileType ProfileType = FSGCollisionProfileHelper::GetCollisionProfileNameFromString(Action.RawEventArgs[CurArgIndex++]);
					FSGCollisionParams Params;
					Params.ProfileType = ProfileType;
					HitboxCapsule->CollisionParams = Params;
				}
				
				if (NumArgs >= CurArgIndex + 1)
				{
					float CapsuleHalfHeight = FCString::Atof(*Action.RawEventArgs[CurArgIndex++]);
					HitboxCapsule->CapsuleHalfHeight = CapsuleHalfHeight;
				}
				
				if (NumArgs >= CurArgIndex + 1)
				{
					float CapsuleRadius = FCString::Atof(*Action.RawEventArgs[CurArgIndex++]);
					HitboxCapsule->CapsuleRadius = CapsuleRadius;
				}
				
			}

			
		}
	}

	return true;
}

FVector FSGAssetTranslator::GetVectorFromStr(const FString& Str)
{
	TArray<FString> Split;
	Str.ParseIntoArray(Split, TEXT(","));
	if (Split.Num() < 3)
	{
		return FVector::ZeroVector;
	}
	
	const float X = FCString::Atof(*Split[0]);
	const float Y = FCString::Atof(*Split[1]);
	const float Z = FCString::Atof(*Split[2]);

	return FVector(X,Y,Z);
	
}
