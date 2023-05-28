// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulsGame/Character/MyPlayerController.h"

#include "GameDelegates.h"
#include "SGTeamComponent.h"
#include "SoulsGame/MyGameModeBase.h"
#include "SoulsGame/SGameInstance.h"
#include "SoulsGame/SUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "SoulsGame/Abilities/SGAbilitySystemGlobals.h"
#include "SoulsGame/Abilities/PowerUps/SGProjectilePowerupAbility.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SoulsGame/Projectiles/SShooterEnums.h"
#include "SoulsGame/Subsystems/UISubsystem.h"
#include "SoulsGame/UE4Overrides/MyCharacterMovementComponent.h"
#include "SoulsGame/UI/UpgradeMenuWidget.h"
#include "SoulsGame/UI/HUDWidget.h"

AMyPlayerController::AMyPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AMyPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (ASCharacterBase* PlayerPawn = GetPawnCharacter())
    {
        if (USGTeamComponent* TeamComponent = PlayerPawn->GetTeamComponent())
        {
            TeamComponent->SetTeam(ESGTeam::Player);
        }


    }
}

void AMyPlayerController::SetPlayer(UPlayer* InPlayer)
{
    Super::SetPlayer(InPlayer);

}

APawn* AMyPlayerController::GetCurrentPawnBasedOnIndex() const
{
    return GeneratedPawns[CurrentPawnIndex];
}

FVector AMyPlayerController::GetCameraForwardVector(float FocusPoint) const
{
    FVector Location;
    FRotator Rotation;
    GetPlayerViewPoint(Location, Rotation);
    return Location + Rotation.Vector() * FocusPoint;
}

void AMyPlayerController::SetBossActor(ASCharacterBase* Actor)
{
    BossActorPtr = MakeWeakObjectPtr(Actor);
    if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
    {
        if (UHUDWidget* HUD = Subsystem->GetHUDInstance())
        {
            HUD->SetBossActor(Actor);
        }
    }
}

void AMyPlayerController::UnsetBossActor()
{
    BossActorPtr.Reset();
    if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
    {
        if (UHUDWidget* HUD = Subsystem->GetHUDInstance())
        {
            HUD->UnsetBossActor();
        }
    }
}

void AMyPlayerController::OnCharacterAppliedEffects(ASCharacterBase* AppliedEffectCharacter)
{
    /*
    if (ASCharacterBase* PlayerPawn = GetPawnCharacter())
    {
        PlayerPawn->OnCharacterAppliedInitialEffects.RemoveAll(this);
        if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
        {
            if (UHUDWidget* HUD = Subsystem->GetHUDInstance())
            {
                HUD->UpdateAbilityAmounts(PlayerPawn);
            }
        }
    }
    */
}

void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();


    if (ASCharacterBase* PlayerPawn = GetPawnCharacter())
    {
        LevelUpHandle = PlayerPawn->OnLevelUpDelegate.AddUObject(this, &AMyPlayerController::OnLevelUp);
    }
     

    UpgradeMenuWidget = Cast<UUpgradeMenuWidget>(CreateWidget(this, UpgradeMenuClass));

    if (USGUpgradeController* UpgradeController = USGUpgradeController::GetSingleton(this))
    {
        UpgradeAppliedHandle = UpgradeController->OnUpgradeAppliedDelegate.AddUObject(this, &AMyPlayerController::OnUpgradeOptionApplied);
    }

    GetWorld()->OnWorldBeginPlay.AddUObject(this, &AMyPlayerController::PostBeginPlay);


    ASCharacterBase* PlayerPawn = GetPawnCharacter();
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined; //Default
    SpawnParameters.Instigator = nullptr; // Needed for WeaponActor.cpp
    SpawnParameters.Owner = nullptr;
    GeneratedPawns.Add(PlayerPawn);
    CurrentPawnIndex = 0;

    USpringArmComponent* SpringArm = FSUtils::GetSpringArmComponent(PlayerPawn);
    bHasCameraLag = SpringArm->bEnableCameraLag || SpringArm->bEnableCameraRotationLag;
    
    for (auto& PawnClass : GeneratedPawnClasses)
    {
        APawn * SpawnedObject = GetWorld()->SpawnActor<APawn>(PawnClass, PlayerPawn->GetActorLocation(), PlayerPawn->GetActorRotation(), SpawnParameters);
        if (bHasCameraLag)
        {
            FSUtils::SetSpringArmLagOnActor(SpawnedObject, false);
        }

        FSUtils::SetActorEnabled(SpawnedObject, false);
        
        GeneratedPawns.Add(SpawnedObject);
    }
    
    if (PlayerPawn->HasAppliedInitialEffects())
    {
        OnCharacterAppliedEffects(PlayerPawn);
    }
    else
    {
        PlayerPawn->OnCharacterAppliedInitialEffects.AddDynamic(this, &AMyPlayerController::OnCharacterAppliedEffects);
    }
    
}

void AMyPlayerController::PostBeginPlay()
{
    // FSUtils::SetActorEnabled(GetPawn(), false, true, true);
}

void AMyPlayerController::TryUseQueuedMove()
{
    ASCharacterBase* PawnCharacter = this->GetPawnCharacter();
    if (PawnCharacter == nullptr)
    {
        return;
    }
    
    if (PawnCharacter->GetAllowInputDuringMontage() && QueuedAction != nullptr)
    {
        PlayQueuedAction();
    }
}

void AMyPlayerController::BeginDestroy()
{
    FGameDelegates::Get().GetViewTargetChangedDelegate().RemoveAll(this);
    
    if (UpgradeAppliedHandle.IsValid())
    {
        if (USGUpgradeController* UpgradeController = USGUpgradeController::GetSingleton(this))
        {
            UpgradeController->OnUpgradeAppliedDelegate.RemoveAll(this);
            UpgradeAppliedHandle.Reset();
        }
    }

    if (UpgradeAppliedHandle.IsValid())
    {
        if (ASCharacterBase* PlayerPawn = GetPawnCharacter())
        {
            PlayerPawn->OnLevelUpDelegate.RemoveAll(this);
        }
    }
    
    Super::BeginDestroy();
}

void AMyPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ASCharacterBase* PawnCharacter = this->GetPawnCharacter();
    if (PawnCharacter == nullptr)
    {
        return;
    }

    if (!PawnCharacter->GetMovementComponent()->IsFalling())
    {
        UsedFallingEffects.Reset();
    }
    
    TryUseQueuedMove();
    
    this->UpdateRotation(DeltaTime);

    // Have all the invisible pawns follow the current one for VFX trails to fix right
    // Can probably avoid this in the future
    for (int i = 0; i < GeneratedPawns.Num(); i++)
    {
        if (CurrentPawnIndex == i)
        {
            continue;
        }

        const FTransform ActorTransform = GeneratedPawns[CurrentPawnIndex]->GetActorTransform();
        GeneratedPawns[i]->TeleportTo(ActorTransform.GetLocation(), FRotator(ActorTransform.GetRotation()));
    }



    // 	World->LineTraceMultiByObjectType(Results, Start, End, FCollisionObjectQueryParams(ECollisionChannel::ECC_Visibility), FCollisionQueryParams(SCENE_QUERY_STAT(LandscapeTrace), true));


    //FVector TestDirection = FVector(-0.41366771875679831f, 0.78701064458783731f, -0.45770434099315982f);
    //PawnCharacter->AddMovementInput(TestDirection, 1.0f);
    
    /*
    FMinimalViewInfo NewCameraInfo = PlayerCameraManager->GetCameraCachePOV();
    UCameraComponent* NewCameraRef = FSUtils::GetCameraComponent(GeneratedPawns[CurrentPawnIndex]);
    FMinimalViewInfo NewCameraInfo2;
    NewCameraRef->GetCameraView(0, NewCameraInfo2);

    int test = 0;

    FMinimalViewInfo OldCameraInfo = PlayerCameraManager->GetCameraCachePOV();
    UCameraComponent* CameraRef = FSUtils::GetCameraComponent(GeneratedPawns[CurrentPawnIndex]);
    FMinimalViewInfo OldCameraInfo2;
    CameraRef->GetCameraView(0, OldCameraInfo2);

    //CameraRef->SetWorldRotation(CameraRef->GetComponentRotation());
    //UE_LOG(LogTemp, Warning, TEXT("Rotation: %s | %s"), *OldCameraInfo2.Rotation.ToString(), *FRotator(CameraRef->GetComponentRotation()).ToString());
        */
    
}

// Tick() cannot be used to set a rotation without SetRotation()....
void AMyPlayerController::UpdateRotation(float DeltaTime)
{
    Super::UpdateRotation(DeltaTime);
    
    this->HandleCameraRotation();
}

void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    /*
     // Using delegates seems to cause errors sometimes
    FInputKeyBinding TimeAbilityBinding(EKeys::G, IE_Pressed);
    TimeAbilityBinding.KeyDelegate.GetDelegateForManualSet().BindLambda( [this] ()
    {
       
    });
    this->InputComponent->KeyBindings.Add(TimeAbilityBinding);
    */
    
    
    this->InputComponent->BindAxis(TEXT("RotateCamera"));
    this->InputComponent->BindAxis(TEXT("ChangePitch"));

    // You can do it this way, but I want to do it less segmentedly.
    this->InputComponent->BindAxis(TEXT("MoveForward"), this, &AMyPlayerController::MoveForward);
    this->InputComponent->BindAxis(TEXT("MoveRight"), this, &AMyPlayerController::MoveRight);

    this->InputComponent->BindAction("NormalAttack", IE_Pressed, this, &AMyPlayerController::QueueNormalAttack);
    //this->InputComponent->BindAction("BAttack", IE_Pressed, this, &AMyPlayerController::BAttack);

    // Note: BindKey requires Slate/SlateCore
    this->InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AMyPlayerController::OnQKeyPressed);
    this->InputComponent->BindKey(EKeys::E, IE_Pressed, this, &AMyPlayerController::OnEKeyPressed);
    this->InputComponent->BindKey(EKeys::R, IE_Pressed, this, &AMyPlayerController::OnRKeyPressed);
    this->InputComponent->BindKey(EKeys::T, IE_Pressed, this, &AMyPlayerController::OnTKeyPressed);


    this->InputComponent->BindKey(EKeys::H, IE_Pressed, this, &AMyPlayerController::OnHKeyPressed);

    this->InputComponent->BindKey(EKeys::LeftControl, IE_Pressed, this, &AMyPlayerController::OnLeftControlPressed);
    this->InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AMyPlayerController::OnSpaceBarPressed);
    this->InputComponent->BindKey(EKeys::LeftAlt, IE_Pressed, this, &AMyPlayerController::OnLeftAltPressed);


    this->InputComponent->BindKey(EKeys::Z, IE_Pressed, this, &AMyPlayerController::OnZKeyPressed);

    this->InputComponent->BindKey(EKeys::X, IE_Pressed, this, &AMyPlayerController::OnXKeyPressed);
    this->InputComponent->BindKey(EKeys::C, IE_Pressed, this, &AMyPlayerController::OnCKeyPressed);
    this->InputComponent->BindKey(EKeys::R, IE_Pressed, this, &AMyPlayerController::OnRKeyPressed);

    SetupGlobalInputComponents(this->InputComponent);
    
    this->InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AMyPlayerController::OnRightMouseButtonPressed);

    FInputKeyBinding Binding = this->InputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &AMyPlayerController::OnRightMouseButtonReleased);

    this->InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AMyPlayerController::OnOneKeyPressed);
    this->InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AMyPlayerController::OnTwoKeyPressed);

    this->InputComponent->BindKey(EKeys::Zero, IE_Pressed, this, &AMyPlayerController::OnZeroKeyPressed);

    this->InputComponent->BindKey(EKeys::BackSpace, IE_Pressed, this, &AMyPlayerController::OnBackspaceKeyPressed);
}


void AMyPlayerController::HandleCameraRotation()
{
    const float RotateCameraValue = GetInputAxisValue(TEXT("RotateCamera"));
    const float ChangePitchValue = GetInputAxisValue(TEXT("ChangePitch"));

    this->AddYawInput(RotateCameraValue);
    this->AddPitchInput(ChangePitchValue);

    //UE_LOG(LogTemp, Warning, TEXT("Bind axes camera %f"), RotateCameraValue);
    //UE_LOG(LogTemp, Warning, TEXT("Bind axes pitch %f"), ChangePitchValue);
}

void AMyPlayerController::RotateCamera(const float InputAxis)
{
    this->AddYawInput(InputAxis);
    //UE_LOG(LogTemp, Warning, TEXT("Bind axes camera %f"), InputAxis);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AMyPlayerController::MoveForward(const float InputAxis)
{
    const FRotator Rotator = PlayerCameraManager->GetCameraRotation();
    const FVector Forward = UKismetMathLibrary::GetForwardVector(Rotator);
    ASCharacterBase* PawnCharacter = this->GetPawnCharacter();

    if (PawnCharacter == nullptr)
    {
        return;
    }
    
    if (PawnCharacter->GetCanMove())
    {
        if (InputAxis != 0)
        {
            auto Move = [=]()
            {
                PawnCharacter->AddMovementInput(Forward, InputAxis);
            };

            QueueAction(Move, ECharacterActionPriority::Move , true);
        }

        PawnCharacter->SetForwardBackDirectionVector(Forward, InputAxis);
    }

}

// ReSharper disable once CppMemberFunctionMayBeConst
void AMyPlayerController::MoveRight(const float InputAxis)
{
    const FRotator Rotator = PlayerCameraManager->GetCameraRotation();
    const FVector Right = UKismetMathLibrary::GetRightVector(Rotator);

    ASCharacterBase* PawnCharacter = this->GetPawnCharacter();

    if (PawnCharacter == nullptr)
    {
        return;
    }
    
    if (PawnCharacter->GetCanMove())
    {
        if (InputAxis != 0)
        {
            auto Move = [=]()
            {
                PawnCharacter->AddMovementInput(Right, InputAxis);
            };

            QueueAction(Move, ECharacterActionPriority::Move, true);
        }

        PawnCharacter->SetRightLeftDirectionVector(Right, InputAxis);
    }
}



ASCharacterBase* AMyPlayerController::GetPawnCharacter() const
{
    return Cast<ASCharacterBase>(GetPawn());
}

ASHumanoidCharacter* AMyPlayerController::GetPawnPlayableCharacter() const
{
    return Cast<ASHumanoidCharacter>(GetPawn());
}

ASHumanoidPlayerCharacter* AMyPlayerController::GetPawnCharacterLegacy() const
{
    return Cast<ASHumanoidPlayerCharacter>(GetPawn());
}

void AMyPlayerController::ShowUpgradeMenu()
{
    if (UpgradeMenuWidget == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Upgrade menu widget nullptr"));
        return;
    }

    if (!UpgradeMenuWidget->IsVisible())
    {
        UGameplayStatics::SetGamePaused(this, true);
        TArray<FUpgradeOption> UpgradeOptions;
        if (USGUpgradeController* UpgradeController = Cast<USGUpgradeController>(USGUpgradeController::GetSingleton(this)))
        {
            if (UpgradeController->GetNFromPool(3, UpgradeOptions, true))
            {
                UpgradeMenuWidget->InitializeFromCpp(UpgradeOptions);
            }
        }
        
        UpgradeMenuWidget->AddToViewport();
        UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, UpgradeMenuWidget, EMouseLockMode::DoNotLock);
    }

    /*
     // Note: This case is not needed because widget takes control - Do rest in blueprint
    else
    {
        UGameplayStatics::SetGamePaused(this, false);
        UpgradeMenuWidget->OnHideFromCpp();
        UpgradeMenuWidget->RemoveFromParent();
        UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
    }
    */

}



void AMyPlayerController::OnQKeyPressed()
{

    QueueBeginAbility("Ability.Ability1");
}

void AMyPlayerController::OnEKeyPressed()
{
    QueueBeginAbility("Ability.Ability2");
    
}

void AMyPlayerController::OnRKeyPressed()
{
    QueueBeginAbility("Ability.Ability3");
    
}

void AMyPlayerController::OnTKeyPressed()
{
    QueueBeginAbility("Ability.Ability4");
}


void AMyPlayerController::OnFKeyPressed()
{
    
}


void AMyPlayerController::OnHKeyPressed()
{
    ASCharacterBase* PawnCharacter = this->GetPawnCharacter();
    if (PawnCharacter == nullptr)
    {
        return;
    }
    
    PawnCharacter->Debug_TestHit();
}

void AMyPlayerController::OnGKeyPressed()
{
    ASHumanoidPlayerCharacter* HumanoidCharacter = this->GetPawnCharacterLegacy();
    if (HumanoidCharacter == nullptr)
    {
        return;
    }

    //HumanoidCharacter->EndShooting();
}

void AMyPlayerController::OnLeftControlPressed()
{
    QueueAction([=]()
    {
        ASCharacterBase* PawnCharacter = this->GetPawnCharacter();
        if (PawnCharacter == nullptr)
        {
            return;
        }

        
       PawnCharacter->DoJump();
    }, ECharacterActionPriority::Roll, true);
}

void AMyPlayerController::OnSpaceBarPressed()
{
    QueueAction([=]()
    {
        ASHumanoidCharacter* PlayableCharacter = this->GetPawnPlayableCharacter();
        PlayableCharacter->RotateTowardsDirection();
        PlayableCharacter->UseAbility(USGAbilitySystemGlobals::GetSG().Tag_Ability_Dash.ToString());
    }, ECharacterActionPriority::Roll);
}

void AMyPlayerController::OnLeftAltPressed()
{
    QueueAction([=]()
    {
        ASHumanoidCharacter* PlayableCharacter = this->GetPawnPlayableCharacter();
        
        PlayableCharacter->UseAbility(USGAbilitySystemGlobals::GetSG().Tag_Ability_Backstep.ToString());
    }, ECharacterActionPriority::Roll);
}

void AMyPlayerController::OnZKeyPressed()
{
    QueueBeginAbility(USGAbilitySystemGlobals::GetSG().Tag_Ability_UseItem.ToString());
}

void AMyPlayerController::OnXKeyPressed()
{
    ASCharacterBase* PawnCharacter = this->GetPawnCharacter();
    UMyAttributeSet* AttributeSet = PawnCharacter->GetAttributeSet();
    UE_LOG(LogTemp, Warning, TEXT("Health: %f"), AttributeSet->GetHealth());
}

void AMyPlayerController::OnCKeyPressed()
{
    QueueBeginAbility(USGAbilitySystemGlobals::GetSG().Tag_Ability_Parry.ToString());
}



void AMyPlayerController::OnOneKeyPressed()
{
    SwitchPawnIndex(0);


}

void AMyPlayerController::OnTwoKeyPressed()
{
    SwitchPawnIndex(1);
}

void AMyPlayerController::OnZeroKeyPressed()
{
    ASCharacterBase * CharacterBase = GetPawnCharacter();
    if (!CharacterBase)
    {
        return;
    }

    USGTeamComponent* TeamComponent = CharacterBase->GetTeamComponent();

    if (TeamComponent == nullptr)
    {
        return;
    }
    
    ESGTeam OpposingTeam = TeamComponent->GetOpposingTeam();
    AActor* ClosestTarget = FSUtils::GetClosestTeamCharacterFromLocation(GetWorld(), CharacterBase->GetActorLocation(), OpposingTeam);

    if (ClosestTarget == nullptr)
    {
        return;
    }
    
    float Distance = FVector::Distance(CharacterBase->GetActorLocation(), ClosestTarget->GetActorLocation());
    UE_LOG(LogTemp, Warning, TEXT("Distance %f"), Distance);
}

void AMyPlayerController::OnNumPadOneKeyPressed()
{
    //this->GetPawnCharacter()->GetConfectioniteCharacter()->SetDebugMode(true);
    this->GetPawnCharacter()->ToggleGodMode();
}

void AMyPlayerController::OnNumPadTwoKeyPressed()
{
    USGameInstance::Debug_ShowHitboxes = !USGameInstance::Debug_ShowHitboxes;
    UE_LOG(LogTemp, Warning, TEXT("Show hitboxes: %d"), USGameInstance::Debug_ShowHitboxes);
}

void AMyPlayerController::OnNumPadThreeKeyPressed()
{

    // Deal damage to boss
    if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawnCharacter()))
    {
        UMyGameplayEffect* TestEffect = NewObject<UMyGameplayEffect>(GetTransientPackage(), FName(TEXT("TestEffect")));
        TestEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

        FGameplayModifierInfo& ModifierInfo = TestEffect->Modifiers.Add_GetRef(FGameplayModifierInfo());
        ModifierInfo.Attribute = UMyAttributeSet::GetDamageAttribute();
        ModifierInfo.ModifierMagnitude = FScalableFloat(500);
        ModifierInfo.ModifierOp = EGameplayModOp::Additive;

        if (BossActorPtr.IsValid())
        {
            TWeakObjectPtr<ASCharacterBase> BossActor = BossActorPtr;
            if (UAbilitySystemComponent* BossAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(BossActor.Get()))
            {
                AbilitySystemComponent->ApplyGameplayEffectToTarget(TestEffect, BossAbilitySystemComponent, 1.0f, AbilitySystemComponent->MakeEffectContext());
            }
        }
    }
    


}

void AMyPlayerController::OnNumPadFourKeyPressed()
{

}

void AMyPlayerController::OnNumPadFiveKeyPressed()
{
    ASCharacterBase* HumanoidCharacter = this->GetPawnCharacter();

    UCharacterAbilitySystemComponent* AbilitySystem = Cast<UCharacterAbilitySystemComponent>(HumanoidCharacter->GetAbilitySystemComponent());
    FGameplayAbilitySpecWrapper* Wrapper = AbilitySystem->GetGameplayWrapperWithTag(USGAbilitySystemGlobals::GetSG().Tag_Ability_Shoot);
    
    USGProjectilePowerupAbility* Ability = Cast<USGProjectilePowerupAbility>(Wrapper->GetPrimaryInstancedAbility());
    if (Ability != nullptr)
    {
        int32 CurrentLevel = Ability->GetLevel();
        Ability->SetLevel(CurrentLevel + 1);
    }
}

void AMyPlayerController::OnNumPadSixKeyPressed()
{
    ASCharacterBase* HumanoidCharacter = this->GetPawnCharacter();

    UCharacterAbilitySystemComponent* AbilitySystem = Cast<UCharacterAbilitySystemComponent>(HumanoidCharacter->GetAbilitySystemComponent());
    FGameplayAbilitySpecWrapper* Wrapper = AbilitySystem->GetGameplayWrapperWithTag(USGAbilitySystemGlobals::GetSG().Tag_Ability_Shoot);
    
    USGProjectilePowerupAbility* Ability = Cast<USGProjectilePowerupAbility>(Wrapper->GetPrimaryInstancedAbility());
    if (Ability != nullptr)
    {
        int32 CurrentLevel = Ability->GetLevel();
        if (CurrentLevel > 0)
        {
            Ability->SetLevel(CurrentLevel - 1);
        }
    }
}

void AMyPlayerController::OnNumPadSevenKeyPressed()
{
    ASCharacterBase* HumanoidCharacter = this->GetPawnCharacter();
    if (!FSUtils::bIsTimestopped)
    {
        FSUtils::StopTimeForCharacter(this, HumanoidCharacter);
    }
    else
    {
        FSUtils::ResumeTimeForCharacter(this, HumanoidCharacter);
    }
}

void AMyPlayerController::OnNumPadNineKeyPressed()
{
    if (AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GameMode->bDisableSpawning = !GameMode->bDisableSpawning;
    }
}

void AMyPlayerController::OnBackspaceKeyPressed()
{
    if (AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GameMode->TogglePlayerAutoBattle();
    }
}

void AMyPlayerController::OnRightMouseButtonPressed()
{
    if (FSUtils::bIsTimestopped)
    {
        return;
    }
    
    ASHumanoidPlayerCharacter* HumanoidCharacter = this->GetPawnCharacterLegacy();
    if (HumanoidCharacter == nullptr)
    {
        return;
    }
    
    //HumanoidCharacter->BeginShooting();
    const FString Tag = "Ability.Shoot";
    bool IsUsingAbility = HumanoidCharacter->GetCharacterAbilitySystemComponent()->IsUsingAbilityWithTag(Tag);
    
    if (!IsUsingAbility)
    {
        QueueBeginAbility(Tag, ECharacterActionHint::Hint_Default, true);
    }
    else if (IsUsingAbility && GetCanQueueNextMove() == true)
    {
        QueueAction([=]
        {
            const FRotator Rotator = PlayerCameraManager->GetCameraRotation();

            FRotator Rotation = HumanoidCharacter->GetActorRotation();
            Rotation.Yaw = Rotator.Yaw;
            HumanoidCharacter->SetActorRotation(Rotation);
            
            this->GetPawnPlayableCharacter()->TriggerJumpSectionCombo();
        }, ECharacterActionPriority::Attack, false, ECharacterActionHint::Hint_BasicAttack);
    }

    
}

void AMyPlayerController::OnRightMouseButtonReleased()
{

}

void AMyPlayerController::PushInputComponent(UInputComponent* Input)
{
    Super::PushInputComponent(Input);

    if (Input != this->InputComponent)
    {
        // SetupGlobalInputComponents(Input);
    }
}

bool AMyPlayerController::PopInputComponent(UInputComponent* Input)
{
    return Super::PopInputComponent(Input);
}

void AMyPlayerController::SetupGlobalInputComponents(UInputComponent* Input)
{
    this->InputComponent->BindKey(EKeys::NumPadOne, IE_Pressed, this, &AMyPlayerController::OnNumPadOneKeyPressed);

    this->InputComponent->BindKey(EKeys::NumPadTwo, IE_Pressed, this, &AMyPlayerController::OnNumPadTwoKeyPressed);
    this->InputComponent->BindKey(EKeys::NumPadThree, IE_Pressed, this, &AMyPlayerController::OnNumPadThreeKeyPressed);
    this->InputComponent->BindKey(EKeys::NumPadFour, IE_Pressed, this, &AMyPlayerController::OnNumPadFourKeyPressed);
    this->InputComponent->BindKey(EKeys::NumPadFive, IE_Pressed, this, &AMyPlayerController::OnNumPadFiveKeyPressed);
    this->InputComponent->BindKey(EKeys::NumPadSix, IE_Pressed, this, &AMyPlayerController::OnNumPadSixKeyPressed);
    this->InputComponent->BindKey(EKeys::NumPadSeven, IE_Pressed, this, &AMyPlayerController::OnNumPadSevenKeyPressed);

    this->InputComponent->BindKey(EKeys::NumPadNine, IE_Pressed, this, &AMyPlayerController::OnNumPadNineKeyPressed);
}

void AMyPlayerController::AddInputBinding(const FKey& Key, TFunction<void()> Callback)
{
    FInputKeyBinding Binding(Key, IE_Pressed);
    Binding.KeyDelegate.GetDelegateForManualSet().BindLambda( [=] ()
   {
       Callback();
   });
    
    this->InputComponent->KeyBindings.Add(Binding);
}


// ReSharper disable once CppMemberFunctionMayBeConst
void AMyPlayerController::QueueNormalAttack()
{
    ASHumanoidCharacter* PawnCharacter = this->GetPawnPlayableCharacter();
    if (PawnCharacter == nullptr)
    {
        return;
    }

    const FString Tag = "Ability.Melee";

    // TODO: Move this logic into the pawn itself....
    bool IsUsingAbility = PawnCharacter->GetCharacterAbilitySystemComponent()->IsUsingAbilityWithTag(Tag);
    
    if (!IsUsingAbility)
    {
        QueueBeginAbility(Tag);
    }
    else if (IsUsingAbility && GetCanQueueNextMove() == true)
    {
        QueueAction([=]
        {
            this->GetPawnPlayableCharacter()->TriggerJumpSectionCombo();
        }, ECharacterActionPriority::Attack, false, ECharacterActionHint::Hint_BasicAttack);
    }
}

void AMyPlayerController::Debug_QueueEmpoweredAttack()
{
    ASHumanoidCharacter* PawnCharacter = this->GetPawnPlayableCharacter();
    if (PawnCharacter == nullptr)
    {
        return;
    }

    const FString Tag = "Ability.Ability3";

    bool IsUsingAbility = PawnCharacter->GetCharacterAbilitySystemComponent()->IsUsingAbilityWithTag(Tag);
    
    if (!IsUsingAbility)
    {
        QueueBeginAbility(Tag, ECharacterActionHint::Hint_BasicAttack);
    }
    else if (IsUsingAbility && GetCanQueueNextMove() == true)
    {
        QueueAction([=]
        {
            this->GetPawnPlayableCharacter()->TriggerJumpSectionCombo();
        }, ECharacterActionPriority::Attack, false, ECharacterActionHint::Hint_BasicAttack);
    }
}

void AMyPlayerController::QueueAction(TFunction<void()> Action, int32 ActionPriority, bool ForceCloseMontage,
                                      ECharacterActionHint ActionHint)
{
    bool PlayActionNow = true;

    if (FSUtils::bIsTimestopped && ActionHint != ECharacterActionHint::Hint_BasicAttack)
    {
        return;
    }

    ASCharacterBase* PawnCharacter = this->GetPawnCharacter();
    if (PawnCharacter == nullptr)
    {
        return;
    }
    
    if (!PawnCharacter->GetCanMove()) // Might be changed in the future if I want root/stun to be different
    {
        return;
    }

    const bool bPlayingRootMotion = PawnCharacter->IsPlayingRootMotion();
    const bool bDisableInputForNonRootMotion = PawnCharacter->GetDisableInputForNonRootMotion();
    const bool bGetAllowInputDuringMontage = PawnCharacter->GetAllowInputDuringMontage();
    if ((!bPlayingRootMotion && !bDisableInputForNonRootMotion) || (bPlayingRootMotion && bGetAllowInputDuringMontage))
    {
        // Play immediately
        if (bQueuedActionForceStopMontage || PawnCharacter->GetMontageCancellable())
        {
            PawnCharacter->StopPlayingMontage();
        }
		
        Action();
        //ClearQueuedAction();
    }
    else if (bCanQueueNextMove)
    {
        if (this->QueuedAction != nullptr && this->QueuedActionPriority > ActionPriority)
        {
            return;
        }

        this->QueuedAction = Action;
        this->QueuedActionPriority = ActionPriority;
        this->bQueuedActionForceStopMontage = ForceCloseMontage;
    }
    else
    {
        // Do nothing
    }
}

void AMyPlayerController::PlayQueuedAction()
{
    if (this->QueuedAction == nullptr)
    {
        return;
    }

    ASCharacterBase* PawnCharacter = this->GetPawnCharacter();
    if (bQueuedActionForceStopMontage || (PawnCharacter->IsPlayingRootMotion() && PawnCharacter->GetMontageCancellable()))
    {
        PawnCharacter->StopPlayingMontage();
    }
	
    if (!PawnCharacter->GetCanMove()) // Might be changed in the future if I want root/stun to be different
    {
        ClearQueuedAction();
        return;
    }

    this->QueuedAction();

    ClearQueuedAction();
}

void AMyPlayerController::ClearQueuedAction()
{
    this->QueuedAction = nullptr;
    this->QueuedActionPriority = 0;
    this->bCanQueueNextMove = false;
    this->bQueuedActionForceStopMontage = false;
}

void AMyPlayerController::OnKeyPressed(EKeys Key, EInputEvent EventType)
{
    
}



void AMyPlayerController::OnUpgradeOptionApplied(FUpgradeOption UpgradeOption, AActor* Actor)
{
    AppliedUpgrades.Add(UpgradeOption);
    
    UpdateHUD();
}

void AMyPlayerController::UpdateHUD()
{
    if (USGUISubsystem * Subsystem = FSUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
    {
        if (UHUDWidget* HUD = Subsystem->GetHUDInstance())
        {
            HUD->UpdateHUDStatsFromCpp();
        }
    }
}

void AMyPlayerController::OnLevelUp(ASCharacterBase* CharacterBase, int32 NewLevel)
{
    ShowUpgradeMenu();
}

void AMyPlayerController::SwitchPawnIndex(int Index)
{
    if (GeneratedPawns.Num() >= Index + 1 && CurrentPawnIndex != Index)
    {
        const int PreviousPawnIndex = CurrentPawnIndex;
        const FTransform ActorTransform = GeneratedPawns[PreviousPawnIndex]->GetActorTransform();
        CurrentPawnIndex = Index;
        GeneratedPawns[Index]->TeleportTo(ActorTransform.GetLocation(), FRotator(ActorTransform.GetRotation()));

        const FRotator ViewRotation = GetControlRotation();
        
        FSUtils::SetActorEnabled(GeneratedPawns[PreviousPawnIndex], false);
        this->Possess(GeneratedPawns[Index]);
        FSUtils::SetActorEnabled(GeneratedPawns[Index], true);

        // Fix rotation
        ControlRotation = ViewRotation;

        // Fix lag
        if (bHasCameraLag)
        {
            FSUtils::SetSpringArmLagOnActor(GeneratedPawns[PreviousPawnIndex], false);
            FSUtils::SetSpringArmLagOnActor(GeneratedPawns[Index], false);
            
            GetWorld()->GetTimerManager().SetTimerForNextTick([=]()
            {
                FSUtils::SetSpringArmLagOnActor(GeneratedPawns[Index], true);
            });
        }
        
    }
}

void AMyPlayerController::QueueBeginAbility(const FString AbilityName, ECharacterActionHint ActionHint, bool bRotateTowardsReticle)
{
    ASCharacterBase* PlayerPawn = GetPawnCharacter();
    if (PlayerPawn->GetCharacterMovementComponent()->IsFalling())
    {
        if (UsedFallingEffects.Contains(AbilityName))
        {
            return;
        }
        else
        {
            UsedFallingEffects.Add(AbilityName);
        }
    }
    
    QueueAction([=]
    {
        if (bRotateTowardsReticle)
        {
            const FRotator Rotator = PlayerCameraManager->GetCameraRotation();

            FRotator Rotation = PlayerPawn->GetActorRotation();
            Rotation.Yaw = Rotator.Yaw;
            PlayerPawn->SetActorRotation(Rotation);
        }
        
        UCharacterAbilitySystemComponent* AbilitySystemComponent = PlayerPawn->GetCharacterAbilitySystemComponent();
        AbilitySystemComponent->ActivateAbilityWithTag(AbilityName);
    }, ECharacterActionPriority::Attack, false, ActionHint);
}
