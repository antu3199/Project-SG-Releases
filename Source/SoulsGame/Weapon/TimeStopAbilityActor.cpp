// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeStopAbilityActor.h"


#include "DrawDebugHelpers.h"
#include "GameplayTagsManager.h"
#include "Camera/CameraComponent.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"
#include "SoulsGame/Abilities/Tasks/AsyncTaskTagChanged.h"

ATimeStopAbilityActor::ATimeStopAbilityActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ATimeStopAbilityActor::Initialize(UMyGameplayAbility* DataContainer)
{
	// Allow immediately overlapping actors to trigger
	TArray<AActor *> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor *& Actor : OverlappingActors)
	{
		this->NotifyActorBeginOverlap(Actor);
	}
}

void ATimeStopAbilityActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor == GetInstigator())
	{
		return;
	}
	
	ASCharacterBase *CharacterBase = Cast<ASCharacterBase>(OtherActor);
	if (CharacterBase == nullptr)
	{
		return;
	}

	if (HitActors.Contains(OtherActor))
	{
		return;
	}

	HitActors.Add(OtherActor);

	UE_LOG(LogTemp, Warning, TEXT("Apply timestop to: %s"), *OtherActor->GetName());

	TArray<AActor *> OverlappingActors {OtherActor};

	FAsyncTaskTagChangedData TaskData;
	TaskData.AbilitySystemComponent = CharacterBase->GetAbilitySystemComponent();
	TaskData.EffectGameplayTag = USGAbilitySystemGlobals::GetSG().Tag_State_Timelock;
	TaskData.DestroyOnZero = true;

	UAsyncTaskTagChanged * TagChangedTask = UAsyncTaskTagChanged::CreateTagChangedTask(TaskData);
	TagChangedTask->OnTagAdded.AddDynamic(this, &ATimeStopAbilityActor::OnTagAdded);
	TagChangedTask->OnTagRemoved.AddDynamic(this, &ATimeStopAbilityActor::OnTagRemoved);

	TagChangedTask->Activate();

	ApplyEffectToActor(0, OtherActor);
}

void ATimeStopAbilityActor::NotifyActorEndOverlap(AActor* OtherActor)
{

	Super::NotifyActorEndOverlap(OtherActor);
	
	ASCharacterBase *CharacterBase = Cast<ASCharacterBase>(OtherActor);
	if (CharacterBase == nullptr)
	{
		return;
	}

	if (HitActors.Contains(OtherActor))
	{
		HitActors.Remove(OtherActor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("End overlap called in a non hit actor??"));
	}

	UAbilitySystemComponent * AbilitySystemComponent = CharacterBase->GetAbilitySystemComponent();

	RemoveGameplayEffects(AbilitySystemComponent);
}

void ATimeStopAbilityActor::BeginPlay()
{
	Super::BeginPlay();
	
	//GetWorldTimerManager().SetTimer(CanHitTimer, this, &AWeaponActor::SetCanHitTimer,2.f /*Time you want*/, false /*if you want loop set to true*/);

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([&]
    {
		this->Destroy();
    });

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, 5.0f, false);
	
}

void ATimeStopAbilityActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!GameplayEffectDataContainer.IsValid())
	{
		return;
	}

	const FVector CurrentScale = this->GetActorScale();
	const FVector NewScale = CurrentScale + FVector(0.1f, 0.1f, 0.1f);

	if (NewScale.X < 10)
	{
		this->SetActorScale3D(NewScale);
	}
	bool isInside = false;

	this->Base->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);


	TArray<AActor *> AllOverlappingActors;
	this->GetOverlappingActors(AllOverlappingActors);

	for (AActor * HitActor : AllOverlappingActors)
	{
		UCameraComponent * CameraComponent = HitActor->FindComponentByClass<UCameraComponent>();
		if (CameraComponent)
		{
			FVector Start = CameraComponent->GetComponentLocation(); //CameraLoc;

			FVector End = Start; //CamManager->GetCameraLocation();
			const float Radius = 500;
			//DrawDebugSphere(GetWorld(), Start, Radius, 32, FColor(182, 1, 1));

			const FCollisionQueryParams CollisionParms;
			TArray<FHitResult> Hits;

			const bool DidTrace = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(Radius), CollisionParms);

			if (DidTrace)
			{
				for (FHitResult & Result : Hits)
				{
					if (Result.GetActor() == this)
					{
						isInside = true;
						break;
					}
				}
			}
			FWeightedBlendables & CurWeightedBlendables = CameraComponent->PostProcessSettings.WeightedBlendables;

			if (isInside)
			{
				FWeightedBlendable NewBlendable;
				NewBlendable.Weight = 1;
				NewBlendable.Object = InsideMaterial;
				CameraComponent->PostProcessSettings.WeightedBlendables.Array.Add(NewBlendable);

			}
			else
			{
				/*
				// Remove current applied materials (Note: Probably inefficient. Change later...)
				for (int i = CameraComponent->PostProcessSettings.WeightedBlendables.Array.Num()-1; i >= 0; i--)
				{
					FWeightedBlendable & BlendableWeight = CameraComponent->PostProcessSettings.WeightedBlendables.Array[i];
					if (BlendableWeight.Object->GetName() == this->InsideMaterial->GetName())
					{
						CameraComponent->PostProcessSettings.WeightedBlendables.Array.RemoveAt(i);
					}
				}
				*/
					
			}

		}
	}

	this->Base->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

}


void ATimeStopAbilityActor::OnTagAdded(const FGameplayTag CooldownTag, int32 NewCount, ASCharacterBase * Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("STUNNNNN"));

	UAnimInstance* AnimInstance = Actor->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}
	

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		return;
	}



	float Test = AnimInstance->Montage_GetPosition(RootMotion->Montage);
	float Test2 = RootMotion->GetPosition();
	
	
	RootMotion->Pause();
}

void ATimeStopAbilityActor::OnTagRemoved(const FGameplayTag CooldownTag, int32 NewCount, ASCharacterBase * Actor)
{
	UAnimInstance* AnimInstance = Actor->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}
	

	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	if (!RootMotion)
	{
		return;
	}
	
	RootMotion->Play();
	
}
