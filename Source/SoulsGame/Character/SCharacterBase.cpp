// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacterBase.h"


#include "AIController.h"
#include "BrainComponent.h"
#include "GameplayTagsManager.h"
#include "SoulsGame/MyAssetManager.h"
#include "MyPlayerController.h"
#include "PlayerAIController.h"
#include "SGMovementOverrideComponent.h"
#include "SGTeamComponent.h"
#include "SoulsGame/SHitboxManager.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Abilities/AbilityConfectioniteBase.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"
#include "SoulsGame/AI/SBTTask_MoveTo.h"
#include "SoulsGame/DataAssets/AbilityAsset.h"
#include "SoulsGame/DataAssets/WeaponAsset.h"
#include "SoulsGame/Animation/JumpSectionNS.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ChildActorComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "SoulsGame/MyGameModeBase.h"
#include "SoulsGame/SBlueprintLibrary.h"
#include "SoulsGame/SGameInstance.h"
#include "SoulsGame/Level/SGLootActor.h"
#include "SoulsGame/SaveLoad/SGSaveRecord.h"
#include "SoulsGame/Subsystems/UISubsystem.h"
#include "Tasks/AITask_MoveTo.h"
#include "SoulsGame/UE4Overrides/MyCharacterMovementComponent.h"
#include "SoulsGame/UI/ActorUIWidget.h"
#include "SoulsGame/UI/FloatingCombatTextComponent.h"
#include "SoulsGame/UI/HUDWidget.h"


// Sets default values
ASCharacterBase::ASCharacterBase(const FObjectInitializer& ObjectInitializer)
	// Override character movement component
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMyCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("AttributeSet"));
	AbilitySystemComponent->AddAttributeSetSubobject(AttributeSet);

	CharacterMovementComponent = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass()));

	TeamComponent = CreateDefaultSubobject<USGTeamComponent>(TEXT("TeamComponent"));
	MovementOverrideComponent = CreateDefaultSubobject<USGMovementOverrideComponent>(TEXT("MovementOverrideComponent"));

	DetectionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionRadius"));
	DetectionRadius->SetupAttachment(GetCapsuleComponent());

	UIActorParent = CreateDefaultSubobject<USceneComponent>(TEXT("UIActorParent"));
	UIActorParent->SetupAttachment(GetCapsuleComponent());
	
	UIActorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UIActorWidgetComponent"));
	UIActorWidgetComponent->SetupAttachment(UIActorParent);
}

bool ASCharacterBase::CanGetDamaged() const
{
	if (IsDead)
	{
		return false;
	}

	const FGameplayTag InvincibleTag = USGAbilitySystemGlobals::GetSG().Tag_State_Invincible;
	//const FName InvincibleTag = "State.Invincible";
	if (AbilitySystemComponent->HasMatchingGameplayTag(InvincibleTag))
	{
		return false;
	}

	return true;
}


void ASCharacterBase::SetCanMove(bool Set)
{
	CanMove = Set;
}

bool ASCharacterBase::GetCanMove() const
{
	return CanMove && !IsDead;
}



void ASCharacterBase::StopPlayingMontage()
{
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
	
	AnimInstance->Montage_Stop(0.0f, CurrentActiveMontage);
	MontageCancellable = false;
	AllowInputDuringMontage = false;
	// his->JumpSectionCancellable = false;
}

bool ASCharacterBase::IsPlayingMontage() const
{
	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return false;
	}

	UAnimMontage * CurrentActiveMontage = AnimInstance->GetCurrentActiveMontage();
	if (!CurrentActiveMontage)
	{
		return false;
	}

	return true;
}


bool ASCharacterBase::GetRootMotionEnabled() const
{
	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return false;
	}

	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveMontageInstance();
	if (!MontageInstance)
	{
		return false;
	}

	return !MontageInstance->IsRootMotionDisabled();
}

void ASCharacterBase::SetRootMotionEnabled(bool Set)
{
	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	if (Set)
	{
		AnimInstance->SetRootMotionMode(ERootMotionMode::Type::RootMotionFromMontagesOnly);
	}
	else
	{
		AnimInstance->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	}
	
}

FVector ASCharacterBase::GetDirectionVector() const
{
	FVector Result = (ForwardBackDirectionVector + RightLeftDirectionVector);
	Result.Z = 0;
	Result.Normalize();
        
	return Result;
}

void ASCharacterBase::RotateTowardsDirection()
{
	const FVector Dir = GetDirectionVector();

	FRotator ThisRotation = GetActorRotation();

	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, Dir);
	Rotation.Pitch = ThisRotation.Pitch;

	SetActorRotation(Rotation);
}

void ASCharacterBase::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Other == this)
	{
		return;
	}

	if (StopRootMotionOnHit)
	{
		SetStopRootMotionMovement(true);
	}

	OnCharacterNotifyHit.Broadcast(this, Other, Hit);
}

UMyCharacterMovementComponent* ASCharacterBase::GetCharacterMovementComponent() const
{
	return Cast<UMyCharacterMovementComponent>(CharacterMovementComponent);
}

void ASCharacterBase::SetStopRootMotionMovement(bool Enabled)
{
	UMyCharacterMovementComponent * MovementComponent = GetCharacterMovementComponent();
	MovementComponent->StopRootMotionMovement = Enabled;
}

void ASCharacterBase::SetStopRootMotionOnHit(bool Enabled)
{
	StopRootMotionOnHit = Enabled;
	if (!Enabled)
	{
		SetStopRootMotionMovement(false);
	}
}


USBTTask_MoveTo* ASCharacterBase::CreateAIMoveTowardsTask(const FVector& GoalLocation, float AcceptanceRadius)
{

	AAIController * AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return nullptr;
	}

	USBTTask_MoveTo* MoveToTask = USBTTask_MoveTo::CreateAIMoveTo(AIController, GoalLocation, nullptr, AcceptanceRadius);
	if (!MoveToTask)
	{
		return nullptr;
	}

	//MoveToTask->ActivateAndBind();
	return MoveToTask;
}

void ASCharacterBase::DoJump()
{
	if (!CanJump())
	{
		return;
	}
	
	// if (JumpNumber >= 2)
	// {
	// 	return;
	// }

	/*
	bool bMaxSpeedOnJump = false;
	FVector DirectionVector = FVector::ZeroVector;
	if (bMaxSpeedOnJump) DirectionVector = GetDirectionVector();
	float JumpZVelocity = CharacterMovementComponent->JumpZVelocity;
	float CurrentSpeed = CharacterMovementComponent->Velocity.Size();

	FVector JumpVelocity;
	if (!GetRootMotionEnabled())
	{
		JumpVelocity = DirectionVector * CurrentSpeed;
	}
	else
	{
		CharacterMovementComponent->Velocity = FVector::ZeroVector;
	}

	
	//JumpVelocity.Normalize();
	JumpVelocity.Z = JumpZVelocity;
	//JumpVelocity *= JumpZVelocity;

	//CharacterMovementComponent->Velocity = FVector::ZeroVector;
	LaunchCharacter(JumpVelocity, false, true);
*/

	//StopPlayingMontage();

	bool bJumpSuccessful = CharacterMovementComponent->DoJump(false);
	if (bJumpSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("JumpSuccessful"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT JumpSuccessful"));
	}
	JumpCurrentCount++;


}

void ASCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	/*
	auto AiController = static_cast<AAIController*>(GetController());
	if (AiController != nullptr) {
		AiController->GetPathFollowingComponent()->Activate();
	}
	*/
	/*
	auto AiController = static_cast<AAIController*>(GetController());
	if (AiController != nullptr) {
		AiController->BrainComponent->RestartLogic();
	}
	*/
}

float ASCharacterBase::GetHealthPercent() const
{
	const float Health = AttributeSet->GetHealth();
	const float MaxHealth = AttributeSet->GetMaxHealth();
	return Health / MaxHealth;
}

float ASCharacterBase::GetXpPercent() const
{
	const float Xp = AttributeSet->GetXp();
	const float MaxXp = AttributeSet->GetMaxXp();
	return Xp / MaxXp;
}


float ASCharacterBase::GetManaPercent() const
{
	const float Mana = AttributeSet->GetMana();
	const float MaxMana = AttributeSet->GetMaxMana();
	return Mana / MaxMana;
}

void ASCharacterBase::ToggleGodMode()
{
	if (AbilitySystemComponent == nullptr)
	{
		return;
	}

	const FGameplayTag InvincibleTag = USGAbilitySystemGlobals::GetSG().Tag_State_Invincible;
	if (AbilitySystemComponent->HasMatchingGameplayTag(InvincibleTag))
	{
		// Remove it
		UE_LOG(LogTemp, Warning, TEXT("GODMODE REMOVED"));
		AbilitySystemComponent->RemoveLooseGameplayTag(InvincibleTag);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GODMODE ENGAGED"));
		AbilitySystemComponent->AddLooseGameplayTag(InvincibleTag);
	}
}

void ASCharacterBase::SetGodMode(bool Set)
{
	const FGameplayTag InvincibleTag = USGAbilitySystemGlobals::GetSG().Tag_State_Invincible;
	if (Set)
	{
		if (!AbilitySystemComponent->HasMatchingGameplayTag(InvincibleTag))
		{
			AbilitySystemComponent->AddLooseGameplayTag(InvincibleTag);
		}
		else
		{
			// Do nothing
		}
	}
	else
	{
		if (AbilitySystemComponent->HasMatchingGameplayTag(InvincibleTag))
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(InvincibleTag);
		}
		else
		{
			// Do nothing
		}
	}
}

void ASCharacterBase::AddXp(int32 AmountAdded)
{
	AttributeSet->SetXp(AttributeSet->GetXp() + AmountAdded);
	if (AttributeSet->GetXp() >= AttributeSet->GetMaxXp())
	{
		LevelUp();
	}
	

	if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
	{
		PlayerController->UpdateHUD();
	}
}

void ASCharacterBase::LevelUp()
{
	AttributeSet->SetMaxXp(AttributeSet->GetMaxXp() * 1.5f);
	AttributeSet->SetXp(0);

	const int32 OldLevel = AttributeSet->GetLevel();
	const int32 NewLevel = OldLevel + 1;
	AttributeSet->SetLevel(NewLevel);
	
	OnLevelUpDelegate.Broadcast(this, NewLevel);
}

void ASCharacterBase::Falling()
{
	Super::Falling();

	/*
	auto AiController = static_cast<AAIController*>(GetController());
	if (AiController != nullptr) {
		AiController->GetPathFollowingComponent()->Deactivate();
	}
	*/
}

USGTeamComponent* ASCharacterBase::GetTeamComponent() const
{
	return TeamComponent;
}

USGMovementOverrideComponent* ASCharacterBase::GetMovementOverrideComponent() const
{
	return MovementOverrideComponent;
}

float ASCharacterBase::GetFinalMontageSpeed() const
{
	return CurrentMontageSpeed * MontageSpeedMultiplier;
}

void ASCharacterBase::SetCurrentMontageSpeed(float Value)
{
	CurrentMontageSpeed = Value;
}

float ASCharacterBase::GetCurrentMontageSpeed() const
{
	return CurrentMontageSpeed;
}

void ASCharacterBase::SetMontageSpeedMultiplier(float Value)
{
	MontageSpeedMultiplier = Value;
}

float ASCharacterBase::GetMontageSpeedMultiplier() const
{
	return MontageSpeedMultiplier;
}

bool ASCharacterBase::GetAllowSetMontageSpeed() const
{
	return bAllowSetMontageSpeed;
}

void ASCharacterBase::SetAllowSetMontageSpeed(bool Value)
{
	bAllowSetMontageSpeed = Value;
}

void ASCharacterBase::OnAggroSet(bool bIsAggro, ASCharacterBase* OtherPawn)
{
	if (!IsPlayerControlled())
	{
		return;
	}

	if (OtherPawn->ActorHasTag(USGAbilitySystemGlobals::GetSG().Tag_CharacterType_AI_Boss.GetTagName()))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			if (bIsAggro)
			{
				PlayerController->SetBossActor(OtherPawn);
			}
			else
			{
				PlayerController->UnsetBossActor();
			}
		}
	}
}

USGPhaseComponent* ASCharacterBase::GetOptionalPhaseComponent() const
{
	return OptionalPhaseComponent;
}


// Called when the game starts or when spawned
void ASCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	OptionalPhaseComponent = Cast<USGPhaseComponent>(GetComponentByClass(USGPhaseComponent::StaticClass()));
	
	DetectionRadius->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	
    this->InitializeAbilitySystem();

	 if (USHitboxManager* HitboxManager = USHitboxManager::GetSingleton())
	 {
	 	DebugHitboxId = HitboxManager->RegisterExistingComponent(GetCapsuleComponent());
	 	HitboxManager->SetDebugLevel(DebugHitboxId, ESGDebugCollisionLevel::All); // TODO: Maybe remove later
	 }

	if (bStartAggroed)
	{
		if (APlayerAIController* AIController = Cast<APlayerAIController>(GetController()))
		{
			AIController->SetAggro(true);
		}
	}

	DetectionRadius->OnComponentBeginOverlap.AddDynamic(this, &ASCharacterBase::OnDetectionOverlapBegin);
	DetectionRadius->OnComponentEndOverlap.AddDynamic(this, &ASCharacterBase::OnDetectionOverlapEnd);

	if (UActorUIWidget* UIWidget = GetUIActorWidget())
	{
		const float Health = AttributeSet->GetHealth();
		const float MaxHealth = AttributeSet->GetMaxHealth();
		UIWidget->CustomInitialize(UIActorWidgetComponent, Health, MaxHealth);
	}
}

void ASCharacterBase::BeginDestroy()
{
	if (DebugHitboxId != -1)
	{
		if (USHitboxManager* HitboxManager = USHitboxManager::GetSingleton())
		{
			HitboxManager->RemoveHitbox(DebugHitboxId);
		}
	}

	DebugHitboxId = -1;

	if (DetectionRadius != nullptr)
	{
		DetectionRadius->OnComponentBeginOverlap.RemoveAll(this);
		DetectionRadius->OnComponentEndOverlap.RemoveAll(this);
	}

	
	Super::BeginDestroy();
}

void ASCharacterBase::InitializeAbilitySystem()
{

	if (this->AbilitySystemComponent)
	{
		this->AbilitySystemComponent->InitAbilityActorInfo(this, this);
		this->AddStartupGameplayAbilities();
	}
}

void ASCharacterBase::AddStartupGameplayAbilities()
{
	if (!this->AbilitySystemComponent)
	{
		return;
	}
	
	if (this->AbilitiesSpecHandles.Num() != 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warning: Ability spec handles not empty"));
	}

	UMyAssetManager & MyAssetManager = UMyAssetManager::Get();

	/*
	for (const FPrimaryAssetId & AssetId : this->DefaultSlottedAbilities)
	{
		UAbilityAsset * ItemAsset = MyAssetManager.ForceLoad<UAbilityAsset>(AssetId);
		if (ItemAsset)
		{
			//this->AbilitiesSpecHandles.Add
			FGameplayAbilityDataContainer & DataContainer = this->AbilitySystemComponent->AddNewGameplayAbilityDataContainer(); 
			DataContainer.SetGameplayAbility(ItemAsset->GrantedAbility.GetDefaultObject());
			DataContainer.GameplayAbilitySpec = FGameplayAbilitySpec(DataContainer.MyGameplayAbility);
			DataContainer.GameplayAbilitySpecHandle = this->AbilitySystemComponent->GiveAbility(DataContainer.GameplayAbilitySpec);

			UE_LOG(LogTemp, Warning, TEXT("Ability granted %s"), *DataContainer.GetMyGameplayAbilityName());
		}
	}
	*/

	this->AbilitySystemComponent->GrantAbilities(this->DefaultAbilities);
	
	for (TSubclassOf<UMyGameplayEffect> & Effect : PassiveGameplayEffects)
	{
		this->AbilitySystemComponent->ApplyGameplayEffect(Effect, this);
	}

	if (AttributeSet != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Initial Health: %s %f"), *GetActorNameOrLabel(), AttributeSet->GetHealth());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AttributeSet was null!: %s "), *GetActorNameOrLabel());
	}

	OnCharacterAppliedInitialEffects.Broadcast(this);
	bHasAppliedInitialEffects = true;
	
}

void ASCharacterBase::OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// TODO:
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (Pawn->IsPlayerControlled())
		{
			if (APlayerAIController* AIController = Cast<APlayerAIController>(GetController()))
			{
				AIController->SetAggro(true);
			}
		}

	}

}

void ASCharacterBase::OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}


void ASCharacterBase::SetForwardBackDirectionVector(const FVector Forward, const float InputAxis)
{
	if (InputAxis > 0)
	{
		ForwardBackDirectionVector = Forward;
	}
	else if (InputAxis < 0)
	{
		ForwardBackDirectionVector = -Forward;
	}
	else
	{
		ForwardBackDirectionVector = FVector::ZeroVector;
	}
}

void ASCharacterBase::SetRightLeftDirectionVector(const FVector Right, const float InputAxis)
{
	if (InputAxis > 0)
	{
		RightLeftDirectionVector = Right;
	}
	else if (InputAxis < 0)
	{
		RightLeftDirectionVector = -Right;
	}
	else
	{
		RightLeftDirectionVector = FVector::ZeroVector;
	}
}


// Called every frame
void ASCharacterBase::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);


	
	const FVector Dir = this->GetDirectionVector();

	if (this->bOverrideRotation && Dir != FVector::ZeroVector)
	{
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, Dir);
		FRotator ThisRotation = this->GetActorRotation();
    
		this->SetActorRotation(FMath::Lerp(ThisRotation, Rotation, this->CharacterOverrideRotationRate));
	}

	// TODO: TEMP Code
	/*
	if (UActorUIWidget* UIWidget = GetUIActorWidget())
	{
		float Distance = SBlueprintLibrary::GetDistanceFromCamera(GetWorld(), 0, GetActorLocation());
		if (Distance != 0)
		{
			float Scale = UIDistanceScale / Distance;
			FVector2D NewDrawSize = OriginalActorUIDrawSize * Scale;
			UIActorWidgetComponent->SetDrawSize(NewDrawSize);
		}
	}
	*/
	
	
	
	/*
	if (GetCapsuleComponent()->GetCollisionEnabled() == ECollisionEnabled::Type::NoCollision && !IsDead)
	{
		int32 ASD = 23;
	}
	
	if (IsDead)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
		UAnimMontage * CurrentActiveMontage = AnimInstance->GetCurrentActiveMontage();
		if (CurrentActiveMontage != OnDeathMontage)
		{
			int32 Test = 0;
		}
	}

	if (!FSUtils::bIsTimestopped && this->GetActorTimeDilation() != 1.0f)
	{
		int32 asdf = 123;
	}


	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	FAnimMontageInstance* RootMotion = AnimInstance->GetActiveMontageInstance();
	UAnimMontage * CurrentActiveMontage = AnimInstance->GetCurrentActiveMontage();
	
	if (GetCapsuleComponent()->GetCollisionEnabled() == ECollisionEnabled::Type::NoCollision && CurrentActiveMontage != OnDeathMontage)
	{
		int32 ASD = 23;
	}
	*/
}

void ASCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


UAbilitySystemComponent* ASCharacterBase::GetAbilitySystemComponent() const
{
	return this->AbilitySystemComponent;
}

bool ASCharacterBase::CanJumpInternal_Implementation() const
{
	// 	Default implementation
	// Ensure that the CharacterMovement state is valid
	bool bJumpIsAllowed = CharacterMovementComponent->CanAttemptJump();

	if (bJumpIsAllowed)
	{
		// Ensure JumpHoldTime and JumpCount are valid.
		if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
		{
			if (JumpCurrentCount == 0 && CharacterMovementComponent->IsFalling())
			{
				bJumpIsAllowed = JumpCurrentCount + 1 < JumpMaxCount;
			}
			else
			{
				bJumpIsAllowed = JumpCurrentCount < JumpMaxCount;
			}
		}
		else
		{
			// Only consider JumpKeyHoldTime as long as:
			// A) The jump limit hasn't been met OR
			// B) The jump limit has been met AND we were already jumping
			const bool bJumpKeyHeld = (bPressedJump && JumpKeyHoldTime < GetJumpMaxHoldTime());
			bJumpIsAllowed = bJumpKeyHeld &&
				((JumpCurrentCount < JumpMaxCount) || (bWasJumping && JumpCurrentCount == JumpMaxCount));
		}
	}

	return bJumpIsAllowed;
}

void ASCharacterBase::SaveGame(FSGSaveRecord& SaveRecord)
{
	SaveRecord.MagicNumber = Debug_MagicNumber;
}

void ASCharacterBase::LoadGame(const FSGSaveRecord& LoadRecord)
{
	Debug_MagicNumber = LoadRecord.MagicNumber;
}

bool ASCharacterBase::GetIsDead() const
{
	return this->IsDead;
}

float ASCharacterBase::GetAnimMovementSpeed() const
{
	return this->AnimMovementSpeed;
}

void ASCharacterBase::UpdateAnimMovementSpeed()
{
	const FVector Velocity = this->GetVelocity();
	const float NormalizedVelocity = Velocity.Size();

	this->AnimMovementSpeed = NormalizedVelocity;
}

UActorUIWidget* ASCharacterBase::GetUIActorWidget() const
{
	return Cast<UActorUIWidget>(UIActorWidgetComponent->GetWidget());
}

void ASCharacterBase::HandleDamage(float DamageAmount, const FHitResult& HitInfo,
                                   const FGameplayTagContainer& DamageTags, AActor* DamageCauser)
{
	// UE_LOG(LogTemp, Warning, TEXT("I Got hit: %s"), *this->GetName());
	// Damage causer is player
	APawn* DamageCauserPawn = Cast<APawn>(DamageCauser);
	if (DamageCauserPawn == nullptr || !DamageCauserPawn->IsPlayerControlled())
	{
		DamageCauserPawn = DamageCauser->GetInstigator();
	}

	if (DamageCauserPawn != nullptr)
	{
		if (DamageCauserPawn->IsPlayerControlled() && DamageCauserPawn != this)
		{
			if (UFloatingCombatTextComponent* FloatingTextComponent = Cast<UFloatingCombatTextComponent>(DamageCauser->GetComponentByClass(UFloatingCombatTextComponent::StaticClass())))
			{

				const FText DamageText = UKismetTextLibrary::Conv_FloatToText(DamageAmount, ERoundingMode::HalfToEven, false, true, 1, 324, 0, 0);
				const FVector DamageTextLocation = GetActorLocation();
				FloatingTextComponent->AddFloatingText_Client(DamageText, DamageTextLocation, FColor::White);
			}
		}
	}

	// If Damage causer is pawn
	if (this->IsPlayerControlled())
	{
		if (UFloatingCombatTextComponent* FloatingTextComponent = Cast<UFloatingCombatTextComponent>(this->GetComponentByClass(UFloatingCombatTextComponent::StaticClass())))
		{
			const FText DamageText = UKismetTextLibrary::Conv_FloatToText(DamageAmount, ERoundingMode::HalfToEven, false, true, 1, 324, 0, 0);
			const FVector DamageTextLocation = GetActorLocation();
			FloatingTextComponent->AddFloatingText_Client(DamageText, DamageTextLocation, FColor::Red);
		}
	}
	
	float Health = AttributeSet->GetHealth();
	float MaxHealth = AttributeSet->GetMaxHealth();
	
	if (IsDead) return;


	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}
	
	if (Health > 0)
	{
		Temp_StaggerCount++;

		bool bShouldStagger = false;
		bool bDoLaunchCharacter = false;

		if (Temp_StaggerCount >= Temp_StaggerLimit)
		{
			bShouldStagger = true;
		}

		// Have all hits against player stagger for now
		if (IsPlayerControlled())
		{
			bShouldStagger = true;
		}

		if (!IsPlayerControlled())
		{
			// Instead of getting impact point, just get actor locations for simplicity
			FVector HitLocation = this->GetActorLocation();
			FVector CauserLocation = DamageCauser->GetActorLocation();
			FVector MoveDirection = HitLocation - CauserLocation;
			MoveDirection.Z = 0;
			MoveDirection.Normalize();
			float Distance = FVector::Distance(CauserLocation, this->GetActorLocation());


			if (bDoLaunchCharacter)
			{
				float Magnitude = MaxKnockbackImpactVelocity;
				AnimInstance->StopAllMontages(0.0f);
			
				CharacterMovementComponent->StopMovementImmediately();
				LaunchCharacter(MoveDirection * Magnitude, true, true);

				// 
				auto AiController = static_cast<AAIController*>(GetController());
				if (AiController != nullptr && AiController->BrainComponent) {
					//AiController->BrainComponent->StopLogic(TEXT("Hit"));
				}
				auto RestartLogicDelegate = [this]()
				{
					auto AiController = static_cast<AAIController*>(GetController());
					if (AiController != nullptr && AiController->BrainComponent) {
						// AiController->BrainComponent->RestartLogic();
						// Ignore restarting logic for now...
					}
				};

				FTimerHandle RestartLogicHandle;
				GetWorld()->GetTimerManager().SetTimer(RestartLogicHandle, RestartLogicDelegate, 0.1f, false);
			}
			
			if (APlayerAIController* AIController = Cast<APlayerAIController>(GetController()))
			{
				AIController->SetAggro(true);
			}
		}
		
		if (bShouldStagger)
		{
			Temp_StaggerCount = 0;

			if (this->OnHitMontage != nullptr)
			{
				const FName CurrentSectionName = AnimInstance->Montage_GetCurrentSection(OnHitMontage);

				const int RandInt = FMath::RandRange(0, OnHitMontage->CompositeSections.Num() - 1);

				AnimInstance->Montage_Play(OnHitMontage);
				const FName NextSectionName = OnHitMontage->GetSectionName(RandInt);
				if (NextSectionName != "None")
				{
					AnimInstance->Montage_JumpToSection(NextSectionName, OnHitMontage);
				}
			}
		}

		if (OptionalPhaseComponent != nullptr)
		{
			OptionalPhaseComponent->UpdatePhase();
		}

	}
	else
	{
		if (this->OnDeathMontage != nullptr)
		{
			AnimInstance->Montage_Play(OnDeathMontage);
		}
		else
		{
			AAIController* AiController = static_cast<AAIController*>(GetController());
			if (AiController != nullptr && AiController->BrainComponent)
			{
				StopPlayingMontage();
				AiController->BrainComponent->StopLogic(TEXT("Death"));
			}
		}
		
		IsDead = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Spawn loot
		if (SpawnedLootOnDeathTemplate != nullptr)
		{
			const FTransform Transform(FRotator::ZeroRotator,GetActorLocation(), FVector::OneVector);
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; //Default
			//SpawnParameters.Owner = this;
			ASGLootActor* SpawnedObject = Cast<ASGLootActor>(GetWorld()->SpawnActor<AActor>(SpawnedLootOnDeathTemplate, Transform, SpawnParameters));
		}
		auto DespawnDelegate = [this]()
		{
			OnCharacterDeath.Broadcast(this);
			if (!IsPlayerControlled())
			{
				if (this->ActorHasTag(USGAbilitySystemGlobals::GetSG().Tag_CharacterType_AI_Boss.GetTagName()))
				{
					if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
					{
						if (UHUDWidget* HUD = Subsystem->GetHUDInstance())
						{
							HUD->UnsetBossActor();
						}
					}
				}
				
				this->Destroy();
			}
			else
			{
				if (AMyGameModeBase* GameModeBase = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
				{
					GameModeBase->RestartGame();
					//UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
				}
			}
		};

		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, DespawnDelegate, TimeBeforeDespawn, false);

	}

	bool FaceAttacker = OnHitMontage != nullptr;
	if (FaceAttacker)
	{
		FVector MyLocation = GetActorLocation();

		FVector InstigatorLocation = DamageCauser->GetActorLocation();
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(MyLocation, InstigatorLocation);
		FRotator ThisRotation = this->GetActorRotation();

		Rotation.Pitch = ThisRotation.Pitch;

		SetActorRotation(Rotation);
	}
}

void ASCharacterBase::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	const float Health = AttributeSet->GetHealth();
	if (!IsPlayerControlled())
	{
		if (!this->ActorHasTag(USGAbilitySystemGlobals::GetSG().Tag_CharacterType_AI_Boss.GetTagName()))
		{
			if (UActorUIWidget* UIWidget = GetUIActorWidget())
			{
				UIWidget->SetValue(Health);
			}
		}

	}

	
}

void ASCharacterBase::Debug_TestHit()
{
	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (this->OnHitMontage != nullptr)
	{
		const FName CurrentSectionName = AnimInstance->Montage_GetCurrentSection(OnHitMontage);

		const int RandInt = FMath::RandRange(0, OnHitMontage->CompositeSections.Num() - 1);

		AnimInstance->Montage_Play(OnHitMontage);
		const FName NextSectionName = OnHitMontage->GetSectionName(RandInt);
		if (NextSectionName != "None")
		{
			AnimInstance->Montage_JumpToSection(NextSectionName, OnHitMontage);
		}
	}
}

void ASCharacterBase::UseAbility(const FString & AbilityTag)
{
	if (!this->AbilitySystemComponent)
	{
		return;
	}

	if (this->AbilitySystemComponent->ActivateAbilityWithTag(AbilityTag))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Activate ability %s"), *AbilityTag.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter::DoRangedAttack SUCCESSED!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to activate ability %s"), *AbilityTag);
	}
}


// ================= Buffered input:

void FBufferedInput::SetBufferedAttackInput()
{
	if ((int) EBufferedInputType::Attack < (int)InputType)
	{
		return;
	}

	InputType = EBufferedInputType::Attack;

}

void FBufferedInput::SetBufferedRollInput()
{
	if ((int) EBufferedInputType::Roll < (int)InputType)
	{
		return;
	}

	InputType = EBufferedInputType::Roll;
}

void FBufferedInput::SetBufferedAbilityInput()
{
	if ((int) EBufferedInputType::Ability < (int)InputType)
	{
		return;
	}

	InputType = EBufferedInputType::Ability;
}

void FBufferedInput::Reset()
{
	InputType = EBufferedInputType::None;
	StartBufferingInput = false;
	
}