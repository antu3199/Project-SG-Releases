// Fill out your copyright notice in the Description page of Project Settings.


#include "SGCharacterBase.h"


#include "AIController.h"
#include "BrainComponent.h"
#include "GameplayTagsManager.h"
#include "SoulsGame/SGAssetManager.h"
#include "SGPlayerController.h"
#include "NativeGameplayTags.h"
#include "SoulsGame/AI/SGEnemyAIController.h"
#include "SGMovementOverrideComponent.h"
#include "SGTeamComponent.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/AI/BehaviorTree/SGBTTask_MoveTo.h"
#include "SoulsGame/DataAssets/SGWeaponAsset.h"
#include "SoulsGame/Animation/SGAnimNotifyState_JumpSection.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ChildActorComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "SoulsGame/SGGameModeBase.h"
#include "SoulsGame/SGBlueprintLibrary.h"
#include "SoulsGame/SGGameInstance.h"
#include "SoulsGame/Level/SGLootActor.h"
#include "SoulsGame/SaveLoad/SGSaveRecord.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityComponent.h"
#include "SoulsGame/Subsystems/SGRewardSubsystem.h"
#include "SoulsGame/Subsystems/SGUISubsystem.h"
#include "Tasks/AITask_MoveTo.h"
#include "SoulsGame/UE4Overrides/SGCharacterMovementComponent.h"
#include "SoulsGame/UI/SGActorUIWidget.h"
#include "SoulsGame/UI/SGFloatingCombatTextComponent.h"
#include "SoulsGame/UI/SGHUDWidget.h"


// Sets default values
ASGCharacterBase::ASGCharacterBase(const FObjectInitializer& ObjectInitializer)
	// Override character movement component
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USGCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CharacterMovementComponent = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass()));

	TeamComponent = CreateDefaultSubobject<USGTeamComponent>(TEXT("TeamComponent"));
	MovementOverrideComponent = CreateDefaultSubobject<USGMovementOverrideComponent>(TEXT("MovementOverrideComponent"));

	DetectionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionRadius"));
	DetectionRadius->SetupAttachment(GetCapsuleComponent());

	UIActorParent = CreateDefaultSubobject<USceneComponent>(TEXT("UIActorParent"));
	UIActorParent->SetupAttachment(GetCapsuleComponent());
	
	UIActorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UIActorWidgetComponent"));
	UIActorWidgetComponent->SetupAttachment(UIActorParent);

	SGAbilityComponent = CreateDefaultSubobject<USGAbilityComponent>(TEXT("SGAbilityComponent"));
	StatComponent = CreateDefaultSubobject<USGStatComponent>(TEXT("SGStatComponent"));
}

bool ASGCharacterBase::CanGetDamaged() const
{
	if (IsDead)
	{
		return false;
	}

	/*
	const FGameplayTag InvincibleTag = USGAbilitySystemGlobals::GetSG().Tag_State_Invincible;
	if (AbilitySystemComponent->HasMatchingGameplayTag(InvincibleTag))
	{
		return false;
	}
	*/

	// TODO: Readd invincibility

	return true;
}


void ASGCharacterBase::SetCanMove(bool Set)
{
	CanMove = Set;
}

bool ASGCharacterBase::GetCanMove() const
{
	return CanMove && !IsDead;
}



void ASGCharacterBase::StopPlayingMontage()
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

bool ASGCharacterBase::IsPlayingMontage() const
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


bool ASGCharacterBase::GetRootMotionEnabled() const
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

void ASGCharacterBase::SetRootMotionEnabled(bool Set)
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

FVector ASGCharacterBase::GetDirectionVector() const
{
	FVector Result = (ForwardBackDirectionVector + RightLeftDirectionVector);
	Result.Z = 0;
	Result.Normalize();
        
	return Result;
}

void ASGCharacterBase::RotateTowardsDirection()
{
	const FVector Dir = GetDirectionVector();

	FRotator ThisRotation = GetActorRotation();

	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, Dir);
	Rotation.Pitch = ThisRotation.Pitch;

	SetActorRotation(Rotation);
}

void ASGCharacterBase::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
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

USGCharacterMovementComponent* ASGCharacterBase::GetCharacterMovementComponent() const
{
	return Cast<USGCharacterMovementComponent>(CharacterMovementComponent);
}

void ASGCharacterBase::SetStopRootMotionMovement(bool Enabled)
{
	USGCharacterMovementComponent * MovementComponent = GetCharacterMovementComponent();
	MovementComponent->StopRootMotionMovement = Enabled;
}

void ASGCharacterBase::SetStopRootMotionOnHit(bool Enabled)
{
	StopRootMotionOnHit = Enabled;
	if (!Enabled)
	{
		SetStopRootMotionMovement(false);
	}
}


USGBTTask_MoveTo* ASGCharacterBase::CreateAIMoveTowardsTask(const FVector& GoalLocation, float AcceptanceRadius)
{

	AAIController * AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return nullptr;
	}

	USGBTTask_MoveTo* MoveToTask = USGBTTask_MoveTo::CreateAIMoveTo(AIController, GoalLocation, nullptr, AcceptanceRadius);
	if (!MoveToTask)
	{
		return nullptr;
	}

	//MoveToTask->ActivateAndBind();
	return MoveToTask;
}

void ASGCharacterBase::DoJump()
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

void ASGCharacterBase::Landed(const FHitResult& Hit)
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

float ASGCharacterBase::GetHealthPercent() const
{
	if (FSGStat* HealthStat = StatComponent->GetHealthStat())
	{
		float CurrentValue = HealthStat->GetCurrentValue();
		float MaxValue = HealthStat->GetCurrentMaxValue();
		return HealthStat->GetCurrentValue() / HealthStat->GetCurrentMaxValue();
	}

	return 1;
}

float ASGCharacterBase::GetXpPercent() const
{
	/*
	const float Xp = AttributeSet->GetXp();
	const float MaxXp = AttributeSet->GetMaxXp();
	return Xp / MaxXp;
	*/
	return 1;
}


float ASGCharacterBase::GetManaPercent() const
{
	/*
	const float Mana = AttributeSet->GetMana();
	const float MaxMana = AttributeSet->GetMaxMana();
	return Mana / MaxMana;
	*/
	return 1;
}

void ASGCharacterBase::ToggleGodMode()
{
	/*
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
	*/
}

void ASGCharacterBase::SetGodMode(bool Set)
{
	/*
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
	*/
}



void ASGCharacterBase::LevelUp()
{

	int32 TestLevel  = 1;
	
	OnLevelUpDelegate.Broadcast(this, TestLevel);
}

void ASGCharacterBase::Falling()
{
	Super::Falling();

	/*
	auto AiController = static_cast<AAIController*>(GetController());
	if (AiController != nullptr) {
		AiController->GetPathFollowingComponent()->Deactivate();
	}
	*/
}

USGTeamComponent* ASGCharacterBase::GetTeamComponent() const
{
	return TeamComponent;
}

USGMovementOverrideComponent* ASGCharacterBase::GetMovementOverrideComponent() const
{
	return MovementOverrideComponent;
}

float ASGCharacterBase::GetFinalMontageSpeed() const
{
	return CurrentMontageSpeed * MontageSpeedMultiplier;
}

void ASGCharacterBase::SetCurrentMontageSpeed(float Value)
{
	CurrentMontageSpeed = Value;
}

float ASGCharacterBase::GetCurrentMontageSpeed() const
{
	return CurrentMontageSpeed;
}

void ASGCharacterBase::SetMontageSpeedMultiplier(float Value)
{
	MontageSpeedMultiplier = Value;
}

float ASGCharacterBase::GetMontageSpeedMultiplier() const
{
	return MontageSpeedMultiplier;
}

bool ASGCharacterBase::GetAllowSetMontageSpeed() const
{
	return bAllowSetMontageSpeed;
}

void ASGCharacterBase::SetAllowSetMontageSpeed(bool Value)
{
	bAllowSetMontageSpeed = Value;
}

void ASGCharacterBase::OnAggroSet(bool bIsAggro, ASGCharacterBase* OtherPawn)
{
	if (!IsPlayerControlled())
	{
		return;
	}
	UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_CharacterType_AI_Boss, "CharacterType.AI.Boss")
	
	if (OtherPawn->ActorHasTag(FName(Tag_CharacterType_AI_Boss.GetTag().ToString())))
	{
		if (ASGPlayerController* PlayerController = Cast<ASGPlayerController>(GetController()))
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

USGPhaseComponent* ASGCharacterBase::GetOptionalPhaseComponent() const
{
	return OptionalPhaseComponent;
}


// Called when the game starts or when spawned
void ASGCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	OptionalPhaseComponent = Cast<USGPhaseComponent>(GetComponentByClass(USGPhaseComponent::StaticClass()));
	
	DetectionRadius->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	
	if (bStartAggroed)
	{
		if (ASGEnemyAIController* AIController = Cast<ASGEnemyAIController>(GetController()))
		{
			AIController->SetAggro(true);
		}
	}

	DetectionRadius->OnComponentBeginOverlap.AddDynamic(this, &ASGCharacterBase::OnDetectionOverlapBegin);
	DetectionRadius->OnComponentEndOverlap.AddDynamic(this, &ASGCharacterBase::OnDetectionOverlapEnd);

	if (USGActorUIWidget* UIWidget = GetUIActorWidget())
	{
		//const float Health = AttributeSet->GetHealth();
		//const float MaxHealth = AttributeSet->GetMaxHealth();
		//UIWidget->CustomInitialize(UIActorWidgetComponent, Health, MaxHealth);
		if (FSGStat* HealthStat = StatComponent->GetHealthStat())
		{
			UIWidget->CustomInitialize(UIActorWidgetComponent, HealthStat->GetCurrentValue(), HealthStat->GetCurrentMaxValue());
		}
	}

	if (StatComponent)
	{
		StatComponent->OnStatChanged.AddDynamic(this, &ASGCharacterBase::HandleStatChanged);
	}
}

void ASGCharacterBase::BeginDestroy()
{
	if (DetectionRadius != nullptr)
	{
		DetectionRadius->OnComponentBeginOverlap.RemoveAll(this);
		DetectionRadius->OnComponentEndOverlap.RemoveAll(this);
	}

	if (StatComponent)
	{
		StatComponent->OnStatChanged.RemoveAll(this);
	}
	
	Super::BeginDestroy();
}


void ASGCharacterBase::AddStartupGameplayAbilities()
{

	USGAssetManager & MyAssetManager = USGAssetManager::Get();

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


	OnCharacterAppliedInitialEffects.Broadcast(this);
	bHasAppliedInitialEffects = true;
	
}

void ASGCharacterBase::OnDeath()
{
	if (UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance())
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
	}
		
	IsDead = true;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Spawn loot
	if (LootOnDeath != nullptr)
	{
		if (USGRewardSubsystem * RewardSubsystem = FSGUtils::GetGameInstanceSubsystem<USGRewardSubsystem>(GetWorld()))
		{
			if (ASGCharacterBase* PlayerPawn = Cast<ASGCharacterBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
			{
				RewardSubsystem->ApplyRewardData(LootOnDeath, PlayerPawn);
			}
		}
	}

	auto DespawnDelegate = [this]()
	{
		OnCharacterDeath.Broadcast(this);
		if (!IsPlayerControlled())
		{
			UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_CharacterType_AI_Boss, "CharacterType.AI.Boss")
				
			if (this->ActorHasTag(Tag_CharacterType_AI_Boss.GetTag().GetTagName()))
			{
				if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
				{
					if (USGHUDWidget* HUD = Subsystem->GetHUDInstance())
					{
						HUD->UnsetBossActor();
					}
				}
			}
				
			this->Destroy();
		}
		else
		{
			if (ASGGameModeBase* GameModeBase = Cast<ASGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				GameModeBase->RestartGame();
			}
		}
	};

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, DespawnDelegate, TimeBeforeDespawn, false);
}

void ASGCharacterBase::OnDetectionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// TODO:
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (Pawn->IsPlayerControlled())
		{
			if (ASGEnemyAIController* AIController = Cast<ASGEnemyAIController>(GetController()))
			{
				AIController->SetAggro(true);
			}
		}

	}

}

void ASGCharacterBase::OnDetectionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}


void ASGCharacterBase::SetForwardBackDirectionVector(const FVector Forward, const float InputAxis)
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

void ASGCharacterBase::SetRightLeftDirectionVector(const FVector Right, const float InputAxis)
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
void ASGCharacterBase::Tick(const float DeltaTime)
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

void ASGCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

bool ASGCharacterBase::CanJumpInternal_Implementation() const
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

void ASGCharacterBase::OnSaveGame(FSGSaveRecord& SaveRecord)
{
	SaveRecord.MagicNumber = Debug_MagicNumber;
}

void ASGCharacterBase::LoadGame(const FSGSaveRecord& LoadRecord)
{
	Debug_MagicNumber = LoadRecord.MagicNumber;
}

bool ASGCharacterBase::GetIsDead() const
{
	return this->IsDead;
}

void ASGCharacterBase::TestFunction_Implementation()
{
}

float ASGCharacterBase::GetAnimMovementSpeed() const
{
	return this->AnimMovementSpeed;
}

void ASGCharacterBase::UpdateAnimMovementSpeed()
{
	const FVector Velocity = this->GetVelocity();
	const float NormalizedVelocity = Velocity.Size();

	this->AnimMovementSpeed = NormalizedVelocity;
}

USGActorUIWidget* ASGCharacterBase::GetUIActorWidget() const
{
	return Cast<USGActorUIWidget>(UIActorWidgetComponent->GetWidget());
}

void ASGCharacterBase::HandleDamage(float DamageAmount, const FHitResult& HitInfo,
                                   const FGameplayTagContainer& DamageTags, AActor* DamageCauser)
{
	// UE_LOG(LogTemp, Warning, TEXT("I Got hit: %s"), *this->GetName());
	// Damage causer is player
	/*
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
	*/
}

void ASGCharacterBase::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	/*
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
	*/
}

void ASGCharacterBase::HandleSGDamage(const FSGDamageParams& DamageParams, float Damage, float NewHealth)
{
	// UE_LOG(LogTemp, Warning, TEXT("I Got hit: %s"), *this->GetName());
	// Damage causer is player
	APawn* DamageCauserPawn = Cast<APawn>(DamageParams.DamageOwnerActor.Get());
	//if (DamageCauserPawn == nullptr || !DamageCauserPawn->IsPlayerControlled())
	if (DamageCauserPawn == nullptr || (!DamageCauserPawn->IsPlayerControlled() && DamageParams.DamageAvatarActor.IsValid()))
	{
		DamageCauserPawn = Cast<APawn>(DamageParams.DamageAvatarActor.Get());
	}

	if (DamageCauserPawn != nullptr)
	{
		if (DamageCauserPawn->IsPlayerControlled() && DamageCauserPawn != this)
		{
			if (USGFloatingCombatTextComponent* FloatingTextComponent = Cast<USGFloatingCombatTextComponent>(DamageCauserPawn->GetComponentByClass(USGFloatingCombatTextComponent::StaticClass())))
			{

				const FText DamageText = UKismetTextLibrary::Conv_FloatToText(Damage, ERoundingMode::HalfToEven, false, true, 1, 324, 0, 0);
				const FVector DamageTextLocation = GetActorLocation();
				FloatingTextComponent->AddFloatingText_Client(DamageText, DamageTextLocation, FColor::White);
			}
		}
	}

	// If Damage causer is pawn
	if (this->IsPlayerControlled())
	{
		if (USGFloatingCombatTextComponent* FloatingTextComponent = Cast<USGFloatingCombatTextComponent>(this->GetComponentByClass(USGFloatingCombatTextComponent::StaticClass())))
		{
			const FText DamageText = UKismetTextLibrary::Conv_FloatToText(Damage, ERoundingMode::HalfToEven, false, true, 1, 324, 0, 0);
			const FVector DamageTextLocation = GetActorLocation();
			FloatingTextComponent->AddFloatingText_Client(DamageText, DamageTextLocation, FColor::Red);
		}
	}

	float Health = NewHealth;
	if (Health > 0)
	{
		if (!IsPlayerControlled())
		{
			// Instead of getting impact point, just get actor locations for simplicity
			if (ASGEnemyAIController* AIController = Cast<ASGEnemyAIController>(GetController()))
			{
				AIController->SetAggro(true);
			}
		}

		if (OptionalPhaseComponent != nullptr)
		{
			OptionalPhaseComponent->UpdatePhase();
		}
	}

	bool FaceAttacker = OnHitMontage != nullptr;
	if (FaceAttacker)
	{
		FVector MyLocation = GetActorLocation();

		FVector InstigatorLocation = DamageCauserPawn->GetActorLocation();
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(MyLocation, InstigatorLocation);
		FRotator ThisRotation = this->GetActorRotation();

		Rotation.Pitch = ThisRotation.Pitch;

		SetActorRotation(Rotation);

		if (UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance())
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
	
}

void ASGCharacterBase::HandleStatChanged(FGameplayTag StatTag, FSGStatValues OldStat, FSGStatValues NewStat)
{
	if (StatTag == USGStatComponent::GetHealthTag())
	{
		HandleHealthChanged(OldStat, NewStat);
	}
}

void ASGCharacterBase::HandleHealthChanged(const FSGStatValues& OldStat, const FSGStatValues& NewStat)
{

	UE_LOG(LogTemp, Display, TEXT("Health stat: %f/%f -> %f/%f"), OldStat.GetValue(), OldStat.GetMaxValue(), NewStat.GetValue(), NewStat.GetMaxValue());
	
	if (NewStat.GetValue() <= 0)
	{

		OnDeath();

	}

	if (!IsPlayerControlled())
	{
		UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_CharacterType_AI_Boss, "CharacterType.AI.Boss")

		if (!this->ActorHasTag(Tag_CharacterType_AI_Boss.GetTag().GetTagName()))
		{
			if (USGActorUIWidget* UIWidget = GetUIActorWidget())
			{
				UIWidget->SetValue(NewStat.Value.CurrentValue);
			}
		}
		else
		{
			if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
			{
				if (USGHUDWidget* HUD = Subsystem->GetHUDInstance())
				{
					if (NewStat.Value.CurrentValue > 0)
					{
						HUD->UpdateBossHealth();
					}
					else
					{
						if (ASGPlayerController* PlayerController = Cast<ASGPlayerController>(HUD->GetOwningPlayer()))
						{
							PlayerController->UnsetBossActor();
						}
					}
				}
			}
		}

	}
}

void ASGCharacterBase::Debug_TestHit()
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


// ================= Buffered input:

void FSGBufferedInput::SetBufferedAttackInput()
{
	if ((int) ESGBufferedInputType::Attack < (int)InputType)
	{
		return;
	}

	InputType = ESGBufferedInputType::Attack;

}

void FSGBufferedInput::SetBufferedRollInput()
{
	if ((int) ESGBufferedInputType::Roll < (int)InputType)
	{
		return;
	}

	InputType = ESGBufferedInputType::Roll;
}

void FSGBufferedInput::SetBufferedAbilityInput()
{
	if ((int) ESGBufferedInputType::Ability < (int)InputType)
	{
		return;
	}

	InputType = ESGBufferedInputType::Ability;
}

void FSGBufferedInput::Reset()
{
	InputType = ESGBufferedInputType::None;
	StartBufferingInput = false;
	
}