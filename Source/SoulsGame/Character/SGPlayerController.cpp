// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulsGame/Character/SGPlayerController.h"

#include "GameDelegates.h"
#include "NativeGameplayTags.h"
#include "SGSlotComponent.h"
#include "SGTeamComponent.h"
#include "SoulsGame/SGGameModeBase.h"
#include "SoulsGame/SGGameInstance.h"
#include "SoulsGame/SGUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityBlueprintLibrary.h"
#include "SoulsGame/SGAbilitySystem/SGAbilityComponent.h"
#include "SoulsGame/Subsystems/SGUISubsystem.h"
#include "SoulsGame/UE4Overrides/SGCharacterMovementComponent.h"
#include "SoulsGame/UI/SGHUDWidget.h"

ASGPlayerController::ASGPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASGPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (ASGCharacterBase* PlayerPawn = GetPawnCharacter())
    {
        if (USGTeamComponent* TeamComponent = PlayerPawn->GetTeamComponent())
        {
            TeamComponent->SetTeam(ESGTeam::Player);
        }
    }
}

void ASGPlayerController::SetPlayer(UPlayer* InPlayer)
{
    Super::SetPlayer(InPlayer);

}

APawn* ASGPlayerController::GetCurrentPawnBasedOnIndex() const
{
    return GeneratedPawns[CurrentPawnIndex];
}

FVector ASGPlayerController::GetCameraForwardVector(float FocusPoint) const
{
    FVector Location;
    FRotator Rotation;
    GetPlayerViewPoint(Location, Rotation);
    return Location + Rotation.Vector() * FocusPoint;
}

void ASGPlayerController::SetBossActor(ASGCharacterBase* Actor)
{
    BossActorPtr = MakeWeakObjectPtr(Actor);
    if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
    {
        if (USGHUDWidget* HUD = Subsystem->GetHUDInstance())
        {
            HUD->SetBossActor(Actor);
        }
    }
}

void ASGPlayerController::UnsetBossActor()
{
    BossActorPtr.Reset();
    if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
    {
        if (USGHUDWidget* HUD = Subsystem->GetHUDInstance())
        {
            HUD->UnsetBossActor();
        }
    }
}

void ASGPlayerController::OnCharacterAppliedEffects(ASGCharacterBase* AppliedEffectCharacter)
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

void ASGPlayerController::BeginPlay()
{
    Super::BeginPlay();


    if (ASGCharacterBase* PlayerPawn = GetPawnCharacter())
    {
        LevelUpHandle = PlayerPawn->OnLevelUpDelegate.AddUObject(this, &ASGPlayerController::OnLevelUp);
    }
     
    GetWorld()->OnWorldBeginPlay.AddUObject(this, &ASGPlayerController::PostBeginPlay);

    ASGCharacterBase* PlayerPawn = GetPawnCharacter();
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined; //Default
    SpawnParameters.Instigator = nullptr; // Needed for WeaponActor.cpp
    SpawnParameters.Owner = nullptr;
    GeneratedPawns.Add(PlayerPawn);
    CurrentPawnIndex = 0;

    USpringArmComponent* SpringArm = FSGUtils::GetSpringArmComponent(PlayerPawn);
    bHasCameraLag = SpringArm->bEnableCameraLag || SpringArm->bEnableCameraRotationLag;
    
    for (auto& PawnClass : GeneratedPawnClasses)
    {
        APawn * SpawnedObject = GetWorld()->SpawnActor<APawn>(PawnClass, PlayerPawn->GetActorLocation(), PlayerPawn->GetActorRotation(), SpawnParameters);
        if (bHasCameraLag)
        {
            FSGUtils::SetSpringArmLagOnActor(SpawnedObject, false);
        }

        FSGUtils::SetActorEnabled(SpawnedObject, false);
        
        GeneratedPawns.Add(SpawnedObject);
    }
    
    if (PlayerPawn->HasAppliedInitialEffects())
    {
        OnCharacterAppliedEffects(PlayerPawn);
    }
    else
    {
        PlayerPawn->OnCharacterAppliedInitialEffects.AddDynamic(this, &ASGPlayerController::OnCharacterAppliedEffects);
    }
    
}

void ASGPlayerController::PostBeginPlay()
{
    // FSUtils::SetActorEnabled(GetPawn(), false, true, true);
}

void ASGPlayerController::TryUseQueuedMove()
{
    ASGCharacterBase* PawnCharacter = this->GetPawnCharacter();
    if (PawnCharacter == nullptr)
    {
        return;
    }
    
    if (PawnCharacter->GetAllowInputDuringMontage() && QueuedAction != nullptr)
    {
        PlayQueuedAction();
    }
}

void ASGPlayerController::BeginDestroy()
{
    FGameDelegates::Get().GetViewTargetChangedDelegate().RemoveAll(this);
    
    Super::BeginDestroy();
}

void ASGPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ASGCharacterBase* PawnCharacter = this->GetPawnCharacter();
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
void ASGPlayerController::UpdateRotation(float DeltaTime)
{
    Super::UpdateRotation(DeltaTime);
    
    this->HandleCameraRotation();
}

void ASGPlayerController::SetupInputComponent()
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
    this->InputComponent->BindAxis(TEXT("MoveForward"), this, &ASGPlayerController::MoveForward);
    this->InputComponent->BindAxis(TEXT("MoveRight"), this, &ASGPlayerController::MoveRight);

    this->InputComponent->BindAction("NormalAttack", IE_Pressed, this, &ASGPlayerController::QueueNormalAttack);
    //this->InputComponent->BindAction("BAttack", IE_Pressed, this, &AMyPlayerController::BAttack);

    // Note: BindKey requires Slate/SlateCore
    this->InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &ASGPlayerController::OnQKeyPressed);
    this->InputComponent->BindKey(EKeys::E, IE_Pressed, this, &ASGPlayerController::OnEKeyPressed);
    this->InputComponent->BindKey(EKeys::R, IE_Pressed, this, &ASGPlayerController::OnRKeyPressed);
    this->InputComponent->BindKey(EKeys::T, IE_Pressed, this, &ASGPlayerController::OnTKeyPressed);


    this->InputComponent->BindKey(EKeys::H, IE_Pressed, this, &ASGPlayerController::OnHKeyPressed);

    this->InputComponent->BindKey(EKeys::LeftControl, IE_Pressed, this, &ASGPlayerController::OnLeftControlPressed);
    this->InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ASGPlayerController::OnSpaceBarPressed);
    this->InputComponent->BindKey(EKeys::LeftAlt, IE_Pressed, this, &ASGPlayerController::OnLeftAltPressed);


    this->InputComponent->BindKey(EKeys::Z, IE_Pressed, this, &ASGPlayerController::OnZKeyPressed);
    this->InputComponent->BindKey(EKeys::Z, IE_Released, this, &ASGPlayerController::OnZKeyReleased);

    
    this->InputComponent->BindKey(EKeys::X, IE_Pressed, this, &ASGPlayerController::OnXKeyPressed);
    this->InputComponent->BindKey(EKeys::C, IE_Pressed, this, &ASGPlayerController::OnCKeyPressed);
    this->InputComponent->BindKey(EKeys::R, IE_Pressed, this, &ASGPlayerController::OnRKeyPressed);

    SetupGlobalInputComponents(this->InputComponent);
    
    this->InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &ASGPlayerController::OnRightMouseButtonPressed);

    FInputKeyBinding Binding = this->InputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &ASGPlayerController::OnRightMouseButtonReleased);

    this->InputComponent->BindKey(EKeys::One, IE_Pressed, this, &ASGPlayerController::OnOnePressed);
    this->InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ASGPlayerController::OnTwoPressed);
    this->InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ASGPlayerController::OnThreePressed);
    this->InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &ASGPlayerController::OnFourPressed);
    
    this->InputComponent->BindKey(EKeys::Zero, IE_Pressed, this, &ASGPlayerController::OnZeroKeyPressed);

    this->InputComponent->BindKey(EKeys::BackSpace, IE_Pressed, this, &ASGPlayerController::OnBackspaceKeyPressed);
}


void ASGPlayerController::HandleCameraRotation()
{
    const float RotateCameraValue = GetInputAxisValue(TEXT("RotateCamera"));
    const float ChangePitchValue = GetInputAxisValue(TEXT("ChangePitch"));

    this->AddYawInput(RotateCameraValue);
    this->AddPitchInput(ChangePitchValue);

    //UE_LOG(LogTemp, Warning, TEXT("Bind axes camera %f"), RotateCameraValue);
    //UE_LOG(LogTemp, Warning, TEXT("Bind axes pitch %f"), ChangePitchValue);
}

void ASGPlayerController::RotateCamera(const float InputAxis)
{
    this->AddYawInput(InputAxis);
    //UE_LOG(LogTemp, Warning, TEXT("Bind axes camera %f"), InputAxis);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ASGPlayerController::MoveForward(const float InputAxis)
{
    ASGCharacterBase* PawnCharacter = this->GetPawnCharacter();
    if (PawnCharacter == nullptr)
    {
        return;
    }
    
    FRotator Rotator = PlayerCameraManager->GetCameraRotation();
    Rotator.Pitch = PawnCharacter->GetActorRotation().Pitch;

    const FVector Forward = UKismetMathLibrary::GetForwardVector(Rotator);
    
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
void ASGPlayerController::MoveRight(const float InputAxis)
{

    ASGCharacterBase* PawnCharacter = this->GetPawnCharacter();
    if (PawnCharacter == nullptr)
    {
        return;
    }
    
    FRotator Rotator = PlayerCameraManager->GetCameraRotation();
    Rotator.Pitch = PawnCharacter->GetActorRotation().Pitch;

    const FVector Right = UKismetMathLibrary::GetRightVector(Rotator);

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



ASGCharacterBase* ASGPlayerController::GetPawnCharacter() const
{
    return Cast<ASGCharacterBase>(GetPawn());
}

ASGHumanoidCharacter* ASGPlayerController::GetPawnPlayableCharacter() const
{
    return Cast<ASGHumanoidCharacter>(GetPawn());
}

ASGHumanoidPlayerCharacter* ASGPlayerController::GetPawnCharacterLegacy() const
{
    return Cast<ASGHumanoidPlayerCharacter>(GetPawn());
}

void ASGPlayerController::ShowUpgradeMenu()
{
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



void ASGPlayerController::OnQKeyPressed()
{
    QueueBeginAbility("Ability.Ability1");
}

void ASGPlayerController::OnEKeyPressed()
{
    QueueBeginAbility("Ability.Ability2");
    
}

void ASGPlayerController::OnRKeyPressed()
{
    QueueBeginAbility("Ability.Ability3");
    
}

void ASGPlayerController::OnTKeyPressed()
{
    QueueBeginAbility("Ability.Ability4");
}


void ASGPlayerController::OnFKeyPressed()
{
    
}


void ASGPlayerController::OnHKeyPressed()
{
    ASGCharacterBase* PawnCharacter = this->GetPawnCharacter();
    if (PawnCharacter == nullptr)
    {
        return;
    }
    
    PawnCharacter->Debug_TestHit();
}

void ASGPlayerController::OnGKeyPressed()
{
    ASGHumanoidPlayerCharacter* HumanoidCharacter = this->GetPawnCharacterLegacy();
    if (HumanoidCharacter == nullptr)
    {
        return;
    }

    //HumanoidCharacter->EndShooting();
}

void ASGPlayerController::OnLeftControlPressed()
{
    QueueAction([=]()
    {
        ASGCharacterBase* PawnCharacter = this->GetPawnCharacter();
        if (PawnCharacter == nullptr)
        {
            return;
        }

        
       PawnCharacter->DoJump();
    }, ECharacterActionPriority::Roll, true);
}

void ASGPlayerController::OnSpaceBarPressed()
{
    static const FGameplayTag Tag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("Ability.Player.Dash"));
    QueueAction([=]()
    {
        ASGHumanoidCharacter* PlayableCharacter = this->GetPawnPlayableCharacter();
        PlayableCharacter->RotateTowardsDirection();
        //PlayableCharacter->UseAbility(USGAbilitySystemGlobals::GetSG().Tag_Ability_Dash.ToString());
        if (USGAbilityComponent* AbilityComponent = PlayableCharacter->FindComponentByClass<USGAbilityComponent>())
        {
            AbilityComponent->RequestActivateAbilityByTag(Tag);
        }
        
    }, ECharacterActionPriority::Roll);
}

void ASGPlayerController::OnLeftAltPressed()
{
    /*
    QueueAction([=]()
    {
        ASHumanoidCharacter* PlayableCharacter = this->GetPawnPlayableCharacter();
        
        PlayableCharacter->UseAbility(USGAbilitySystemGlobals::GetSG().Tag_Ability_Backstep.ToString());
    }, ECharacterActionPriority::Roll);
    */
}

void ASGPlayerController::OnZKeyPressed()
{
    static const FGameplayTag Tag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("Ability.Player.Heal"));
    QueueBeginSGAbility(Tag);
}

void ASGPlayerController::OnZKeyReleased()
{
    static const FGameplayTag Tag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("Ability.Player.Heal"));
    if (ASGCharacterBase* PawnCharacter = this->GetPawnCharacter())
    {
        if (USGAbilityComponent* AbilityComponent = PawnCharacter->FindComponentByClass<USGAbilityComponent>())
        {
            if (AbilityComponent->IsAbilityActive(Tag))
            {
                static const FGameplayTag TriggerTag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("Triggers.Ability.Shared.Deactivate"));
                FSGAbilityEventData Payload;
                Payload.EventTag = TriggerTag;
                USGAbilitySystemBlueprintLibrary::SendAbilityEventToActor(PawnCharacter, TriggerTag, Payload);
            }
        }
    }

}

void ASGPlayerController::OnXKeyPressed()
{
    /*
    ASCharacterBase* PawnCharacter = this->GetPawnCharacter();
    UMyAttributeSet* AttributeSet = PawnCharacter->GetAttributeSet();
    UE_LOG(LogTemp, Warning, TEXT("Health: %f"), AttributeSet->GetHealth());
    */
}

void ASGPlayerController::OnCKeyPressed()
{
    static const FGameplayTag Tag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("Ability.Player.Counter"));
    QueueBeginSGAbility(Tag);
}


void ASGPlayerController::OnZeroKeyPressed()
{
    ASGCharacterBase * CharacterBase = GetPawnCharacter();
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
    AActor* ClosestTarget = FSGUtils::GetClosestTeamCharacterFromLocation(GetWorld(), CharacterBase->GetActorLocation(), OpposingTeam);

    if (ClosestTarget == nullptr)
    {
        return;
    }
    
    float Distance = FVector::Distance(CharacterBase->GetActorLocation(), ClosestTarget->GetActorLocation());
    UE_LOG(LogTemp, Warning, TEXT("Distance %f"), Distance);
}

void ASGPlayerController::OnNumPadOneKeyPressed()
{
    //this->GetPawnCharacter()->GetConfectioniteCharacter()->SetDebugMode(true);
    this->GetPawnCharacter()->ToggleGodMode();
}

void ASGPlayerController::OnNumPadTwoKeyPressed()
{
    USGGameInstance::Debug_ShowHitboxes = !USGGameInstance::Debug_ShowHitboxes;
    UE_LOG(LogTemp, Warning, TEXT("Show hitboxes: %d"), USGGameInstance::Debug_ShowHitboxes);
}

void ASGPlayerController::OnNumPadThreeKeyPressed()
{

    // Deal damage to boss
    // TODO: 
    /*
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
    */
    


}

void ASGPlayerController::OnNumPadFourKeyPressed()
{

}

void ASGPlayerController::OnNumPadFiveKeyPressed()
{
    /*
    ASCharacterBase* HumanoidCharacter = this->GetPawnCharacter();

    UCharacterAbilitySystemComponent* AbilitySystem = Cast<UCharacterAbilitySystemComponent>(HumanoidCharacter->GetAbilitySystemComponent());
    FGameplayAbilitySpecWrapper* Wrapper = AbilitySystem->GetGameplayWrapperWithTag(USGAbilitySystemGlobals::GetSG().Tag_Ability_Shoot);
    
    USGProjectilePowerupAbility* Ability = Cast<USGProjectilePowerupAbility>(Wrapper->GetPrimaryInstancedAbility());
    if (Ability != nullptr)
    {
        int32 CurrentLevel = Ability->GetLevel();
        Ability->SetLevel(CurrentLevel + 1);
    }
    */
}

void ASGPlayerController::OnNumPadSixKeyPressed()
{
    /*
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
    */
}

void ASGPlayerController::OnNumPadSevenKeyPressed()
{
    ASGCharacterBase* HumanoidCharacter = this->GetPawnCharacter();
    if (!FSGUtils::bIsTimestopped)
    {
        FSGUtils::StopTimeForCharacter(this, HumanoidCharacter);
    }
    else
    {
        FSGUtils::ResumeTimeForCharacter(this, HumanoidCharacter);
    }
}

void ASGPlayerController::OnNumPadNineKeyPressed()
{
    if (ASGGameModeBase* GameMode = Cast<ASGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GameMode->bDisableSpawning = !GameMode->bDisableSpawning;
    }
}

void ASGPlayerController::OnBackspaceKeyPressed()
{
    if (ASGGameModeBase* GameMode = Cast<ASGGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GameMode->TogglePlayerAutoBattle();
    }
}

void ASGPlayerController::OnRightMouseButtonPressed()
{
    if (FSGUtils::bIsTimestopped)
    {
        return;
    }
    
    ASGHumanoidPlayerCharacter* HumanoidCharacter = this->GetPawnCharacterLegacy();
    if (HumanoidCharacter == nullptr)
    {
        return;
    }

    /*
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
    */
    static const FGameplayTag Tag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("Ability.Player.Ranged"));
    if (USGAbilityComponent* AbilityComponent = HumanoidCharacter->FindComponentByClass<USGAbilityComponent>())
    {
        bool IsUsingAbility = AbilityComponent->IsAbilityActive(Tag);
    
        if (!IsUsingAbility)
        {
            QueueBeginSGAbility(Tag);
        }
        else if (IsUsingAbility && GetCanQueueNextMove() == true)
        {
            if (AbilityComponent->CanActivateAbilityWithTag(Tag))
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
    }
}

void ASGPlayerController::OnRightMouseButtonReleased()
{
    static const FGameplayTag Tag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("Ability.Player.Ranged"));
    QueueBeginSGAbility(Tag);
}

void ASGPlayerController::OnOnePressed()
{
    // UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_Relic_DamageRelic, "Ability.Relic.DamageRelic")
    // const FGameplayTag Tag = Tag_Relic_DamageRelic.GetTag();
    // QueueBeginSGAbility(Tag);
    QueueUseAbilityOnSlot(0);
}

void ASGPlayerController::OnTwoPressed()
{
    QueueUseAbilityOnSlot(1);
}

void ASGPlayerController::OnThreePressed()
{
    QueueUseAbilityOnSlot(2);
}

void ASGPlayerController::OnFourPressed()
{
    QueueUseAbilityOnSlot(3);
}

void ASGPlayerController::PushInputComponent(UInputComponent* Input)
{
    Super::PushInputComponent(Input);

    if (Input != this->InputComponent)
    {
        // SetupGlobalInputComponents(Input);
    }
}

bool ASGPlayerController::PopInputComponent(UInputComponent* Input)
{
    return Super::PopInputComponent(Input);
}

void ASGPlayerController::SetupGlobalInputComponents(UInputComponent* Input)
{
    this->InputComponent->BindKey(EKeys::NumPadOne, IE_Pressed, this, &ASGPlayerController::OnNumPadOneKeyPressed);

    this->InputComponent->BindKey(EKeys::NumPadTwo, IE_Pressed, this, &ASGPlayerController::OnNumPadTwoKeyPressed);
    this->InputComponent->BindKey(EKeys::NumPadThree, IE_Pressed, this, &ASGPlayerController::OnNumPadThreeKeyPressed);
    this->InputComponent->BindKey(EKeys::NumPadFour, IE_Pressed, this, &ASGPlayerController::OnNumPadFourKeyPressed);
    this->InputComponent->BindKey(EKeys::NumPadFive, IE_Pressed, this, &ASGPlayerController::OnNumPadFiveKeyPressed);
    this->InputComponent->BindKey(EKeys::NumPadSix, IE_Pressed, this, &ASGPlayerController::OnNumPadSixKeyPressed);
    this->InputComponent->BindKey(EKeys::NumPadSeven, IE_Pressed, this, &ASGPlayerController::OnNumPadSevenKeyPressed);

    this->InputComponent->BindKey(EKeys::NumPadNine, IE_Pressed, this, &ASGPlayerController::OnNumPadNineKeyPressed);
}

void ASGPlayerController::AddInputBinding(const FKey& Key, TFunction<void()> Callback)
{
    FInputKeyBinding Binding(Key, IE_Pressed);
    Binding.KeyDelegate.GetDelegateForManualSet().BindLambda( [=] ()
   {
       Callback();
   });
    
    this->InputComponent->KeyBindings.Add(Binding);
}


// ReSharper disable once CppMemberFunctionMayBeConst
void ASGPlayerController::QueueNormalAttack()
{
    ASGHumanoidCharacter* PawnCharacter = this->GetPawnPlayableCharacter();
    if (PawnCharacter == nullptr)
    {
        return;
    }

    /*
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
    */

    const FGameplayTag Tag =  UGameplayTagsManager::Get().RequestGameplayTag(FName("Ability.Player.Melee"));
    if (USGAbilityComponent* AbilityComponent = PawnCharacter->FindComponentByClass<USGAbilityComponent>())
    {
        bool IsUsingAbility = AbilityComponent->IsAbilityActive(Tag);
    
        if (!IsUsingAbility)
        {
            QueueBeginSGAbility(Tag);
        }
        else if (IsUsingAbility && GetCanQueueNextMove() == true)
        {
            QueueAction([=]
            {
                this->GetPawnPlayableCharacter()->TriggerJumpSectionCombo();
            }, ECharacterActionPriority::Attack, false, ECharacterActionHint::Hint_BasicAttack);
        }
    }
}

void ASGPlayerController::Debug_QueueEmpoweredAttack()
{
    /*
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
    */
}

void ASGPlayerController::QueueAction(TFunction<void()> Action, int32 ActionPriority, bool ForceCloseMontage,
                                      ECharacterActionHint ActionHint)
{
    bool PlayActionNow = true;

    if (FSGUtils::bIsTimestopped && ActionHint != ECharacterActionHint::Hint_BasicAttack)
    {
        return;
    }

    ASGCharacterBase* PawnCharacter = this->GetPawnCharacter();
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

void ASGPlayerController::PlayQueuedAction()
{
    if (this->QueuedAction == nullptr)
    {
        return;
    }

    ASGCharacterBase* PawnCharacter = this->GetPawnCharacter();
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

void ASGPlayerController::ClearQueuedAction()
{
    this->QueuedAction = nullptr;
    this->QueuedActionPriority = 0;
    this->bCanQueueNextMove = false;
    this->bQueuedActionForceStopMontage = false;
}

void ASGPlayerController::OnKeyPressed(EKeys Key, EInputEvent EventType)
{
    
}

void ASGPlayerController::UpdateHUD()
{
    if (USGUISubsystem * Subsystem = FSGUtils::GetGameInstanceSubsystem<USGUISubsystem>(GetWorld()))
    {
        if (USGHUDWidget* HUD = Subsystem->GetHUDInstance())
        {
            HUD->UpdateHUDStatsFromCpp();
        }
    }
}

void ASGPlayerController::OnLevelUp(ASGCharacterBase* CharacterBase, int32 NewLevel)
{
    ShowUpgradeMenu();
}

void ASGPlayerController::SwitchPawnIndex(int Index)
{
    if (GeneratedPawns.Num() >= Index + 1 && CurrentPawnIndex != Index)
    {
        const int PreviousPawnIndex = CurrentPawnIndex;
        const FTransform ActorTransform = GeneratedPawns[PreviousPawnIndex]->GetActorTransform();
        CurrentPawnIndex = Index;
        GeneratedPawns[Index]->TeleportTo(ActorTransform.GetLocation(), FRotator(ActorTransform.GetRotation()));

        const FRotator ViewRotation = GetControlRotation();
        
        FSGUtils::SetActorEnabled(GeneratedPawns[PreviousPawnIndex], false);
        this->Possess(GeneratedPawns[Index]);
        FSGUtils::SetActorEnabled(GeneratedPawns[Index], true);

        // Fix rotation
        ControlRotation = ViewRotation;

        // Fix lag
        if (bHasCameraLag)
        {
            FSGUtils::SetSpringArmLagOnActor(GeneratedPawns[PreviousPawnIndex], false);
            FSGUtils::SetSpringArmLagOnActor(GeneratedPawns[Index], false);
            
            GetWorld()->GetTimerManager().SetTimerForNextTick([=]()
            {
                FSGUtils::SetSpringArmLagOnActor(GeneratedPawns[Index], true);
            });
        }
        
    }
}

void ASGPlayerController::QueueBeginAbility(const FString AbilityName, ECharacterActionHint ActionHint, bool bRotateTowardsReticle)
{
    /*
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
    */
}

void ASGPlayerController::QueueBeginSGAbility(const FGameplayTag& Tag, ECharacterActionHint ActionHint,
    bool bRotateTowardsReticle)
{
    ASGCharacterBase* PlayerPawn = GetPawnCharacter();
    if (PlayerPawn->GetCharacterMovementComponent()->IsFalling())
    {
        if (UsedFallingEffects.Contains(Tag.ToString()))
        {
            return;
        }
        else
        {
            UsedFallingEffects.Add(Tag.ToString());
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

        if (USGAbilityComponent* AbilityComponent = PlayerPawn->FindComponentByClass<USGAbilityComponent>())
        {
            AbilityComponent->RequestActivateAbilityByTag(Tag);
        }
    }, ECharacterActionPriority::Attack, false, ActionHint);
}

void ASGPlayerController::QueueUseAbilityOnSlot(const int32 Index, ECharacterActionHint ActionHint,
    bool bRotateTowardsReticle)
{
    ASGCharacterBase* PlayerPawn = GetPawnCharacter();

    if (USGSlotComponent* SlotComponent = PlayerPawn->FindComponentByClass<USGSlotComponent>())
    {
        if (FSGSlot* Slot = SlotComponent->GetSlotPtr(Index))
        {
            if (Slot->AbilitySpec == nullptr)
            {
                return;
            }

            FGameplayTag Tag;
            if (USGAbility* AbilityCDO = Slot->AbilitySpec->AbilityClass.GetDefaultObject())
            {
                Tag = AbilityCDO->GetIdentifierTag();
            }

            check(Tag.IsValid());
            if (PlayerPawn->GetCharacterMovementComponent()->IsFalling())
            {
                if (UsedFallingEffects.Contains(Tag.ToString()))
                {
                    return;
                }
                else
                {
                    UsedFallingEffects.Add(Tag.ToString());
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

                SlotComponent->UseAbilityOnSlot(Index);
            }, ECharacterActionPriority::Attack, false, ActionHint);
            
        }
    }
}
