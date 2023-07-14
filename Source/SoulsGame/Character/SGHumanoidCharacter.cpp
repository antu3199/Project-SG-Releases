// Fill out your copyright notice in the Description page of Project Settings.


#include "SGHumanoidCharacter.h"

#include "AbilitySystemGlobals.h"
#include "NativeGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SoulsGame/SGAssetManager.h"
#include "SoulsGame/Animation/SGAnimNotifyState_JumpSection.h"

ASGHumanoidCharacter::ASGHumanoidCharacter(const FObjectInitializer& ObjectInitializer)
    // Override character movement component
    : Super(ObjectInitializer)
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASGHumanoidCharacter::BeginPlay()
{
	Super::BeginPlay();
	this->InitializeItems();

	this->MakeWeapon(WeaponOffset);

	// Initialize timeline
	if (DashSpeedCurve != nullptr && OnRollMontage != nullptr)
	{
		DashTimeline = NewObject<UTimelineComponent>(this, FName("DashTimeline"));
		DashTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		DashTimeline->SetNetAddressable();
		DashTimeline->SetPropertySetObject(this);
		DashTimeline->SetDirectionPropertyName(FName("TimelineDirection"));
		DashTimeline->SetLooping(false);

		float MontageLength = OnRollMontage->GetSectionLength(0);

		DashTimeline->SetTimelineLength(MontageLength / 7.5f);
		DashTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);
		//DashTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

		FOnTimelineFloat OnTimelineCallback;
		OnTimelineCallback.BindDynamic(this, &ASGHumanoidCharacter::DashTimelineCallback);
		DashTimeline->AddInterpFloat(DashSpeedCurve, OnTimelineCallback, FName("DashVal"));

		FOnTimelineEventStatic OnTimelineFinishedCallback;
		OnTimelineFinishedCallback.BindUObject(this, &ASGHumanoidCharacter::DashTimelineFinishedCallback);
		
		DashTimeline->SetTimelineFinishedFunc(OnTimelineFinishedCallback);

		DashTimeline->RegisterComponent();
	}
	
}

void ASGHumanoidCharacter::BeginDestroy()
{
	Super::BeginDestroy();
}

void ASGHumanoidCharacter::Destroyed()
{
	if (IsValid(this->WeaponActor))
	{
		this->WeaponActor->Destroy();
		this->WeaponActor = nullptr;
	}

	Super::Destroyed();
}

void ASGHumanoidCharacter::InitializeItems()
{
	// Load items
	// 
	// Load weapon
	USGAssetManager & MyAssetManager = USGAssetManager::Get();
	USGWeaponAsset * TheAsset = MyAssetManager.ForceLoad<USGWeaponAsset>(this->WeaponAssetId);
	if (TheAsset)
	{
		this->WeaponAsset = TheAsset;
		UE_LOG(LogTemp, Warning, TEXT("Weapon granted %s"), *WeaponAsset->ItemName.ToString());
	}
	
}

bool ASGHumanoidCharacter::IsAttacking() const
{
	/*
	const FString MeleeAbilityTag = "Ability.Melee";

	return this->AbilitySystemComponent->IsUsingAbilityWithTag(MeleeAbilityTag);
	*/
	return false;
}

bool ASGHumanoidCharacter::GetCanMove() const
{
	return Super::GetCanMove() && !bIsDoingRoll;
}

bool ASGHumanoidCharacter::DoOnRoll()
{
	// TODO: Remove me
	/*
	if (!this->AbilitySystemComponent)
	{
		return false;
	}
    
	if (this->GetIsDead())
	{
		return false;
	}

	if (this->OnRollMontage == nullptr)
	{
		return false;
	}

	if (bIsDoingRoll)
	{
		return false;
	}

	if (!bAbleToDash)
	{
		return false;
	}

	bAbleToDash = false;


	//FRotator ControlRotation = GetControlRotation(); // Control rotation is the camera, but I don't want this.
	//SetActorRotation(ControlRotation);
    

	//FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, Right);
	//    PawnCharacter->SetActorRotation(Rotation);

	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return false;
	}

	RotateTowardsDirection();

	FVector Forward = GetActorForwardVector();
	//
	//FVector CurrentVelocity = CharacterMovementComponent->Velocity;
	//CurrentVelocity.Z = 0;

	bIsDoingRoll = true;
	RollDirection = Forward;
	// CharacterMovementComponent->RootMotionParams.bHasRootMotion = true;
	// CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);

	//CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
	//CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Custom);
	//CharacterMovementComponent->SetMovementMode(EMovementMode::MOVE_Falling);
	OGGroundFriction = CharacterMovementComponent->GroundFriction;
	CharacterMovementComponent->GroundFriction = 0;

	FVector PreDashVelocity = CharacterMovementComponent->Velocity;
	PreDashVelocity.Z = 0;
	PreDashSpeed = PreDashVelocity.Size();
	
	

	//FVector JumpVelocity = Forward * RollSpeed;

	//LaunchCharacter(JumpVelocity, true, false);
	//CharacterMovementComponent->Velocity = Forward * RollSpeed;

	
	AnimInstance->Montage_Play(OnRollMontage);

	PlayDashTimeline();
*/
	return true;

}

// DEPECATED
bool ASGHumanoidCharacter::DoMeleeAttack()
{
	/*
	if (!this->AbilitySystemComponent)
	{
		return false;
	}
    
	if (this->GetIsDead())
	{
		return false;
	}

	const FString MeleeAbilityTag = "Ability.Melee";

	if (!this->AbilitySystemComponent->IsUsingAbilityWithTag(MeleeAbilityTag))
	{
		// Start melee ability
		this->AbilitySystemComponent->ActivateAbilityWithTag(MeleeAbilityTag);
	}
	else
	{
		this->BufferJumpSectionForCombo();
	}
*/
	return true;
}

// DEPRECATED
bool ASGHumanoidCharacter::DoBAttack()
{
	/*
	if (!this->AbilitySystemComponent)
	{
		return false;
	}
    
	if (this->GetIsDead())
	{
		return false;
	}

	const FString MeleeAbilityTag = "Ability.BAttack";

	if (!this->AbilitySystemComponent->IsUsingAbilityWithTag(MeleeAbilityTag))
	{
		// Start melee ability
		this->AbilitySystemComponent->ActivateAbilityWithTag(MeleeAbilityTag);
	}
	else
	{
		this->BufferJumpSectionForCombo();
	}
*/
	return true;
}

void ASGHumanoidCharacter::StopRolling()
{
}

void ASGHumanoidCharacter::DashTimelineCallback(float Val)
{
	if (bIsDoingRoll)
	{
		float VelocityZ = CharacterMovementComponent->Velocity.Z;
		CharacterMovementComponent->Velocity = Val * RollDirection * RollSpeed;
		CharacterMovementComponent->Velocity.Z = VelocityZ;
	}
}

void ASGHumanoidCharacter::DashTimelineFinishedCallback()
{
	bIsDoingRoll = false;
	RollDirection = FVector::ZeroVector;
	// CharacterMovementComponent->RootMotionParams.bHasRootMotion = false;
	CharacterMovementComponent->SetDefaultMovementMode();
	CharacterMovementComponent->Velocity = FVector::ZeroVector;
	CharacterMovementComponent->GroundFriction = OGGroundFriction;
	FVector Forward = GetActorForwardVector();
	CharacterMovementComponent->Velocity = Forward * PreDashSpeed;
	CharacterMovementComponent->Velocity.Z = 0;
}

void ASGHumanoidCharacter::PlayDashTimeline()
{
	if (DashTimeline != nullptr)
	{
		DashTimeline->PlayFromStart();
	}
}


USGWeaponAsset* ASGHumanoidCharacter::GetWeaponAsset() const
{
    return this->WeaponAsset;
}

ASGWeaponActor* ASGHumanoidCharacter::GetWeaponActor() const
{
	return WeaponActor;
}

bool ASGHumanoidCharacter::TriggerJumpSectionCombo()
{
	if (this->JumpSectionNS == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("TriggerJumpSection failed:no JUmpSectioNS!"));
		if (IsAttacking())
		{
			// TODO: Review if we need this
			FGameplayTagContainer MeleeTagsContainer;
			UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_AbilityType_Melee, "AbilityType.Melee")
			MeleeTagsContainer.AddTag(Tag_AbilityType_Melee);
			// this->AbilitySystemComponent->CancelAbilities(&MeleeTagsContainer);
		}
		return false;
	}

	if (!GetMesh())
	{
		return false;
	}

	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Display, TEXT("TriggerJumpSection failed: no anim instance!"));
		return false;
	}

	UAnimMontage * CurrentActiveMontage = AnimInstance->GetCurrentActiveMontage();
	if (!CurrentActiveMontage)
	{
		UE_LOG(LogTemp, Display, TEXT("TriggerJumpSection failed: no current montage!" ));
		return false;
	}

	/*
	if (BufferedInput.InputType != EBufferedInputType::Attack)
	{
		// Ignore input failures
		UE_LOG(LogTemp, Display, TEXT("TriggerJumpSection failed: Buffered input incorrect!!"));
		return false;
	}
	*/

	OnJumpSectionCombo();

	const FName CurrentSectionName = AnimInstance->Montage_GetCurrentSection(CurrentActiveMontage);

	const int RandInt = FMath::RandRange(0, this->JumpSectionNS->NextMontageNames.Num() - 1);
	const FName NextSectionName = this->JumpSectionNS->NextMontageNames[RandInt];

	// Wait till animation finishes
	//AnimInstance->Montage_SetNextSection(CurrentSectionName, NextSectionName, CurrentActiveMontage);

	// Jumps directly to animation
	AnimInstance->Montage_JumpToSection(NextSectionName, CurrentActiveMontage);


	// Hack to blend two montages. Works out of the box, but need to fix callback logic...

	/*
	FName NewMontageGroupName = CurrentActiveMontage->GetGroupName();

	TPair<FOnMontageEnded, FOnMontageBlendingOutStarted> CallbackPair;
	
	for (int32 InstanceIndex = 0; InstanceIndex < AnimInstance->MontageInstances.Num(); InstanceIndex++)
	{
		FAnimMontageInstance* MontageInstance = AnimInstance->MontageInstances[InstanceIndex];
		if (MontageInstance && MontageInstance->Montage && (MontageInstance->Montage->GetGroupName() == NewMontageGroupName))
		{
			CallbackPair = TPairInitializer<FOnMontageEnded, FOnMontageBlendingOutStarted>(MontageInstance->OnMontageEnded, MontageInstance->OnMontageBlendingOutStarted);
			MontageInstance->OnMontageEnded.Unbind();
			MontageInstance->OnMontageBlendingOutStarted.Unbind();
		}
	}

	CurrentActiveMontage->AddMetaData(nullptr);
	
	AnimInstance->Montage_Play(CurrentActiveMontage);
	
	for (int32 InstanceIndex = 0; InstanceIndex < AnimInstance->MontageInstances.Num(); InstanceIndex++)
	{
		FAnimMontageInstance* MontageInstance = AnimInstance->MontageInstances[InstanceIndex];
		if (MontageInstance && MontageInstance->Montage && (MontageInstance->Montage->GetGroupName() == NewMontageGroupName))
		{
			MontageInstance->OnMontageEnded = CallbackPair.Key;
			MontageInstance->OnMontageBlendingOutStarted = CallbackPair.Value;
		}
	}

	AnimInstance->Montage_JumpToSection(NextSectionName, CurrentActiveMontage);
*/

	
	//UE_LOG(LogTemp, Warning, TEXT("Trigger section %s"), *NextSectionName.ToString());

	return true;
}

void ASGHumanoidCharacter::BufferJumpSectionForCombo()
{
	if (this->JumpSectionNS == nullptr)
	{
		return;
	}

	if (!GetMesh())
	{
		return;
	}

	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	UAnimMontage * CurrentActiveMontage = AnimInstance->GetCurrentActiveMontage();
	if (!CurrentActiveMontage)
	{
		return;
	}

	//this->BufferedInput.SetBufferedAttackInput();

	UE_LOG(LogTemp, Display, TEXT("BufferedJumpSectionCombo Set to true"));

}

void ASGHumanoidCharacter::OnJumpSectionCombo()
{
	if (this->WeaponActor != nullptr)
	{
		//this->WeaponActor->DisableEmpowerWeapon();
	}
}

void ASGHumanoidCharacter::MakeWeapon(const FVector Offset)
{
	if (this->WeaponAsset == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No weapon found!"));
		return;
	}

	
	const FRotator Rotation = FRotator::ZeroRotator;
	const FVector Location(Offset);
	const FVector Scale = FVector::OneVector;
	const FTransform Transform(Rotation,Location, Scale);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined; //Default
	SpawnParameters.Instigator = this; // Needed for WeaponActor.cpp
	SpawnParameters.Owner = this;


	AActor * SpawnedObject = GetWorld()->SpawnActor<AActor>(this->WeaponAsset->WeaponActorTemplate, Location, Rotation, SpawnParameters);
	if (SpawnedObject)
	{
		//this->AttachToComponent(this->GetMesh(), SpawnedObject,
		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true );
		const FName SocketName = this->WeaponSocketName;
		SpawnedObject->AttachToComponent(this->GetMesh(), AttachmentRules, SocketName);
		
		this->WeaponActor = Cast<ASGWeaponActor>(SpawnedObject);
		//this->WeaponActor->AttachToActor(this, AttachmentRules, SocketName);
		if (this->WeaponActor == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Weapon was null!"));
		}
	}
}

void ASGHumanoidCharacter::SetComboJumpSection(USGAnimNotifyState_JumpSection* JumpSection)
{
	this->JumpSectionNS = JumpSection;

	if (JumpSection != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Set jump section %s"), *JumpSection->NextMontageNames[0].ToString() );
	}
	
	//this->BufferedJumpSectionCombo = false; // Not necessary, but just in case
}

bool ASGHumanoidCharacter::CanBufferNextCombo() const
{
	return  this->JumpSectionNS != nullptr;
}


void ASGHumanoidCharacter::TestFunction_Implementation()
{
}