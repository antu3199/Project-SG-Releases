#include "SUtils.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "MyGameModeBase.h"
#include "NiagaraComponent.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/Character/SHumanoidCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Character/SGTeamComponent.h"
#include "Engine/LevelStreaming.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

bool FSUtils::bIsTimestopped = false;

void FSUtils::DrawSphere(const UWorld* World, FVector Location, float Radius, float Lifetime, FColor Color)
{
    DrawDebugSphere(World, Location, Radius, 32, Color, false, Lifetime);
}

void FSUtils::DrawBox(const UWorld* World, UBoxComponent* BoxComponent, float Lifetime, FColor Color)
{
    if (World == nullptr)
    {
        return;
    }

    FTransform ComponentTransform = BoxComponent->GetComponentTransform();

    FVector Center = ComponentTransform.GetLocation();
    FVector Box = BoxComponent->GetScaledBoxExtent();
    //FVector Box = BoxComponent->GetUnscaledBoxExtent();
    FQuat Rot = BoxComponent->GetComponentQuat();
    // void DrawDebugBox(const UWorld* InWorld, FVector const& Center, FVector const& Box, const FQuat& Rotation, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority, float Thickness)
    DrawDebugBox(World, Center, Box, Rot, Color, false, Lifetime);
}

void FSUtils::DrawSphere(const UWorld* World, USphereComponent* SphereComponent, float Lifetime, FColor Color)
{
    if (World == nullptr)
    {
        return;
    }

    FTransform ComponentTransform = SphereComponent->GetComponentTransform();

    FVector Center = ComponentTransform.GetLocation();
    //float Radius = SphereComponent->GetUnscaledSphereRadius();
    float Radius =  FMath::Abs(SphereComponent->GetScaledSphereRadius());

    DrawDebugSphere(World, Center, Radius, 4, Color, false, Lifetime);
}


void FSUtils::DrawCapsule(const UWorld* World, UCapsuleComponent* Capsule, float Lifetime, FColor Color)
{
    if (World == nullptr || Capsule == nullptr)
    {
        return;
    }
    
    FVector Center = Capsule->GetComponentLocation();
    FQuat Rotation = Capsule->GetComponentQuat();
    FVector Scale = Capsule->GetComponentScale();
    float HalfHeight = FMath::Abs(Capsule->GetScaledCapsuleHalfHeight());
    float Radius = FMath::Abs(Capsule->GetScaledCapsuleRadius());
    //float HalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
    //float Radius = Capsule->GetUnscaledCapsuleRadius();
    DrawDebugCapsule(World, Center, HalfHeight, Radius, Rotation, Color, false, Lifetime);
}

void FSUtils::NormalizeCurveWithDuration(UCurveFloat* Curve, float Duration)
{
}

float FSUtils::RoundFloatToPrecision(float TheFloat, int32 Precision)
{
    float Pow =  FMath::Pow(10.0f,1 * Precision);
    return FMath::RoundToFloat(TheFloat * Pow) / Pow;
}

FString FSUtils::GetFloatAsStringWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero)
{
    //Round to integral if have something like 1.9999 within precision
    float Rounded = roundf(TheFloat);
    if(FMath::Abs(TheFloat - Rounded) < FMath::Pow(10.0f,-1 * Precision))
    {       
        TheFloat = Rounded;
    }
    FNumberFormattingOptions NumberFormat;
    NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
    NumberFormat.MaximumIntegralDigits = 10000;
    NumberFormat.MinimumFractionalDigits = Precision;
    NumberFormat.MaximumFractionalDigits = Precision;
    return FText::AsNumber(TheFloat, &NumberFormat).ToString(); 
}

void FSUtils::DoTimeDilationEffect_DEPRECATED(const UObject* WorldContextObject, TArray<AActor*> IgnoreActors)
{
    // Note: Cannot do 0 right now...
    float TimeDilation = 0.0001f;

    AWorldSettings* const WorldSettings = WorldContextObject->GetWorld()->GetWorldSettings();
    if (WorldSettings != nullptr)
    {
        TimeDilation = WorldSettings->MinGlobalTimeDilation;
    }
    
    UGameplayStatics::SetGlobalTimeDilation(WorldContextObject, TimeDilation);
    for (const auto& Actor : IgnoreActors)
    {
        Actor->CustomTimeDilation = 1.0f / TimeDilation;

        // Niagara components have their own custom time dilation
        TArray<UNiagaraComponent*> Components;
        Actor->GetComponents<UNiagaraComponent>(Components);
        for (const auto& Component : Components)
        {
            Component->SetCustomTimeDilation(1.0f / TimeDilation);
        }
        
    }
    
}

void FSUtils::DoReverseDilationEffect_DEPRECATED(const UObject* WorldContextObject, TArray<AActor*> IgnoreActors)
{
    UGameplayStatics::SetGlobalTimeDilation(WorldContextObject, 1.0f);
    for (const auto& Actor : IgnoreActors)
    {
        Actor->CustomTimeDilation = 1.0f;
        // Niagara components have their own custom time dilation
        TArray<UNiagaraComponent*> Components;
        Actor->GetComponents<UNiagaraComponent>(Components);
        for (const auto& Component : Components)
        {
            Component->SetCustomTimeDilation(1.0f);
        }
        
    }
}

void FSUtils::StopTimeForCharacter(const UObject* WorldContextObject, ASCharacterBase* Character)
{
    TArray<AActor*> TimestopActors;
    TArray<AActor*> ChildActors;
    Character->GetAllChildActors(ChildActors);

    TSet<AActor*> IgnoreActors;
    IgnoreActors.Append(ChildActors);
    IgnoreActors.Add(Character);
    IgnoreActors.Add(Character->GetController());
    if (ASHumanoidCharacter* HumanoidCharacter = Cast<ASHumanoidCharacter>(Character))
    {
        if (HumanoidCharacter->WeaponActor != nullptr)
        {
            IgnoreActors.Add(HumanoidCharacter->WeaponActor);
        }
    }
    
    FSUtils::GetAllActorsInWorld(WorldContextObject, TimestopActors, IgnoreActors);
    FSUtils::StopTimeForActors(WorldContextObject, TimestopActors);
    if (AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
    {
        GameMode->AddTimestopMaterial();
    }

    Character->SetGodMode(true);
    FSUtils::bIsTimestopped = true;
}

void FSUtils::ResumeTimeForCharacter(const UObject* WorldContextObject, ASCharacterBase* Character)
{
    TArray<AActor*> TimestopActors;
    TArray<AActor*> ChildActors;
    Character->GetAllChildActors(ChildActors);
    
    TSet<AActor*> IgnoreActors;
    IgnoreActors.Append(ChildActors);
    IgnoreActors.Add(Character);
    IgnoreActors.Add(Character->GetController());
    if (ASHumanoidCharacter* HumanoidCharacter = Cast<ASHumanoidCharacter>(Character))
    {
        if (HumanoidCharacter->WeaponActor != nullptr)
        {
            IgnoreActors.Add(HumanoidCharacter->WeaponActor);
        }
    }
    
    FSUtils::GetAllActorsInWorld(WorldContextObject, TimestopActors, IgnoreActors);
    FSUtils::ResumeTimeForActors(WorldContextObject, TimestopActors);
    if (AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
    {
        GameMode->RemoveTimestopMaterial();
    }
    Character->SetGodMode(false);
    FSUtils::bIsTimestopped = false;
}

void FSUtils::StopTimeForActors(const UObject* WorldContextObject, TArray<AActor*> ActorsToStop)
{
    for (const auto& Actor : ActorsToStop)
    {
        Actor->CustomTimeDilation = 0.0f;
        // Niagara components have their own custom time dilation
        TArray<UNiagaraComponent*> Components;
        Actor->GetComponents<UNiagaraComponent>(Components);
        for (const auto& Component : Components)
        {
            Component->SetCustomTimeDilation(0.0f);
        }
        
    }
}

void FSUtils::ResumeTimeForActors(const UObject* WorldContextObject, TArray<AActor*> ActorsToResume)
{
    for (const auto& Actor : ActorsToResume)
    {
        Actor->CustomTimeDilation = 1.0f;
        // Niagara components have their own custom time dilation
        TArray<UNiagaraComponent*> Components;
        Actor->GetComponents<UNiagaraComponent>(Components);
        for (const auto& Component : Components)
        {
            Component->SetCustomTimeDilation(1.0f);
        }
    }
}

void FSUtils::GetAllActorsInWorld(const UObject* WorldContextObject, TArray<AActor*>& OutActors, const TSet<AActor*>& IgnoreActors)
{
   if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
   {
       for (TActorIterator<AActor> It(World->GetWorld()); It; ++It)
       {
           if (!IgnoreActors.Contains(*It))
           {
               OutActors.Add(*It);
           }
       }
   }
}

bool FSUtils::GetGroundLocation(const UObject* WorldContextObject, const FVector& Location, FVector& OutLocation)
{
    OutLocation = FVector::ZeroVector;
    if (WorldContextObject == nullptr)
    {
        return false;
    }
    
    UWorld* World = WorldContextObject->GetWorld();

    const FVector Start = Location;
    const FVector End = Start + FVector::DownVector * 10000000.0f;
    FHitResult HitResult;
    ECollisionChannel CollisionChannel = ECollisionChannel::ECC_WorldStatic;
    FCollisionQueryParams CollisionParams;
    FCollisionResponseParams CollisionResponseParams;
    CollisionResponseParams.CollisionResponse.SetAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionResponseParams.CollisionResponse.SetResponse(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
    bool bHit = World->LineTraceSingleByChannel(HitResult, Start, End, CollisionChannel, CollisionParams, CollisionResponseParams);
    if (!bHit)
    {
        return false;
    }

    OutLocation = HitResult.Location;
    return true;
}

void FSUtils::SetActorEnabled(AActor* Actor, bool bEnabled, bool bModifyTick, bool bDisableChildren)
{
    if (Actor == nullptr)
    {
        return;
    }

    if (bModifyTick)
    {
        Actor->SetActorTickEnabled(bEnabled);
    }
    
    Actor->SetActorHiddenInGame(!bEnabled);
    Actor->SetActorEnableCollision(bEnabled);

    if (bDisableChildren)
    {
        TArray<USceneComponent*> Components;
        Actor->GetComponents<USceneComponent>(Components);
        for (const auto& Component : Components)
        {
            Component->SetVisibility(bEnabled);
            if (bModifyTick)
            {
                // Component->SetComponentTickEnabled(bEnabled);
            }
        }

        TArray<AActor*> AttachedActors; 
        Actor->GetAttachedActors(AttachedActors); 
        for (auto* Attached : AttachedActors)
        {
            SetActorEnabled(Attached, bEnabled, bModifyTick, bDisableChildren);
        }
        
        TArray<AActor*> ChildActors;
        Actor->GetAllChildActors(ChildActors);
        for (auto& Child : ChildActors)
        {
            SetActorEnabled(Child, bEnabled, bModifyTick, bDisableChildren);
        }
    }
}

USpringArmComponent* FSUtils::GetSpringArmComponent(AActor* Actor)
{
    TArray<USpringArmComponent*> Components;
    Actor->GetComponents<USpringArmComponent>(Components);
    for (const auto& Component : Components)
    {
        return Component;
    }

    return nullptr;
}

void FSUtils::SetSpringArmLagOnActor(AActor* Actor, bool bLag)
{
    USpringArmComponent* Component = GetSpringArmComponent(Actor);
    if (Component == nullptr)
    {
        return;
    }

    Component->bEnableCameraLag = bLag;
    Component->bEnableCameraRotationLag = bLag;
}

USGTeamComponent* FSUtils::GetTeamComponent(AActor* Actor)
{
    ASCharacterBase* CastedCharacter = Cast<ASCharacterBase>(Actor);
    if (CastedCharacter == nullptr)
    {
        return nullptr;
    }

    return CastedCharacter->GetTeamComponent();
}

ASCharacterBase* FSUtils::GetClosestTeamCharacterFromLocation(const UWorld* World, FVector Location, ESGTeam Team, float MaxDistance,
    bool bCreateDebugSphere, bool bIgnoreDisabledActors)
{

    TArray<AActor*> CheckedActors;

    if (MaxDistance == -1)
    {
        // Loop through all actors in the owrld
        for (TActorIterator<AActor> It(World->GetWorld()); It; ++It)
        {
            CheckedActors.Add(*It);
        }
    }
    else
    {
        TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
        TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
        TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
        TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));
	
        UClass * SeekClass = ASCharacterBase::StaticClass();

        TArray<AActor*> IgnoreActors;
        UKismetSystemLibrary::SphereOverlapActors(World, Location, MaxDistance, TraceObjectTypes, SeekClass, IgnoreActors, CheckedActors);

        if (bCreateDebugSphere)
        {
            FSUtils::DrawSphere(World, Location, MaxDistance);
        }
    }

    ASCharacterBase* Character = nullptr;
    float RunningDistance = -1;
    
    for (auto& Actor : CheckedActors)
    {
        if (ASCharacterBase* CharacterCasted = Cast<ASCharacterBase>(Actor))
        {
            if (CharacterCasted == nullptr || (bIgnoreDisabledActors && CharacterCasted->IsHidden()))
            {
                continue;
            }
            
            if (USGTeamComponent* Component = CharacterCasted->GetTeamComponent())
            {
                if (Component->GetTeam() != Team)
                {
                    continue;
                }

                float Distance = FVector::Distance(Location, CharacterCasted->GetActorLocation());
                if (RunningDistance == -1 || Distance < RunningDistance)
                {
                    Character = CharacterCasted;
                    RunningDistance = Distance;
                }
            }
        }
    }

    if (Character == nullptr)
    {
        return nullptr;
    }

    return Character;
}

void FSUtils::RotateCharacterToLookTarget(AActor* Actor, const FVector& LookTarget, bool bLockZAxis)
{
    if (Actor == nullptr)
    {
        return;
    }
  
    // Simple version
    FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), LookTarget);
    if (bLockZAxis)
    {
        Rotation.Pitch = Actor->GetActorRotation().Pitch;
    }
    Actor->SetActorRotation(Rotation.Quaternion());
}

/*
void FSUtils::LoadAllStreamingLevels(const UWorld* WorldContextObject, bool bVisibleAfterLoad)
{
    const TArray<ULevelStreaming*>& Levels = WorldContextObject->GetStreamingLevels();
    for (int32 i = 0; i < Levels.Num(); i++)
    {
        const ULevelStreaming* LevelToLoad = Levels[i];
        const FName LevelName =  LevelToLoad->GetWorldAssetPackageFName();
		
        FLatentActionInfo LatentInfo;
        //LatentInfo.CallbackTarget = this;
        //LatentInfo.ExecutionFunction = FName("Debug_StreamingFunction");
        // Note: Important to set the UUID otherwise some will not be called!
        LatentInfo.UUID = i;
        LatentInfo.Linkage = LatentInfo.UUID;
        UGameplayStatics::LoadStreamLevel(WorldContextObject, LevelName, bVisibleAfterLoad, true, LatentInfo);

    }

    UGameplayStatics::FlushLevelStreaming(WorldContextObject);
}
*/

void FSUtils::GetAllActorsOfStreamingLevel(const UWorld* WorldContextObject, const ULevelStreaming* LevelToStream, const TSubclassOf<AActor> ActorClass, bool bVisibleAfterLoad, TArray<AActor*>& OutActors)
{
    FLatentActionInfo LatentInfo;
    LatentInfo.UUID = WorldContextObject->GetUniqueID();
    LatentInfo.Linkage = LatentInfo.UUID;
    UGameplayStatics::LoadStreamLevel(WorldContextObject, LevelToStream->GetWorldAssetPackageFName(), bVisibleAfterLoad, true, LatentInfo);
    UGameplayStatics::FlushLevelStreaming(WorldContextObject);

    ULevel* Level = LevelToStream->GetLoadedLevel();
    if (Level != nullptr)
    {
        for (AActor* Actor : Level->Actors)
        {
            if (Actor->IsA(ActorClass))
            {
                OutActors.Add(Actor);
            }
        }
    }
}


