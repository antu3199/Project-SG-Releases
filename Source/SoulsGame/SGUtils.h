#pragma once
#include "CoreMinimal.h"
#include "SGTeam.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

class FSGUtils
{
public:
    static bool bIsTimestopped;
    
    static void DrawSphere(const UWorld* World, FVector Location, float Radius = 100, float Lifetime = 1.0, FColor Color = FColor(0, 0, 128));

    static void DrawBox(const UWorld* World, class UBoxComponent* BoxComponent, float Lifetime = 1.0, FColor Color = FColor(0, 0, 128));

    static void DrawSphere(const UWorld* World, class USphereComponent* SphereComponent, float Lifetime = 1.0, FColor Color = FColor(0, 0, 128));

    static void DrawCapsule(const UWorld* World, UCapsuleComponent * Capsule, float Lifetime = 1.0, FColor Color = FColor(0, 0, 128));
    static void NormalizeCurveWithDuration(UCurveFloat* Curve, float Duration);

    static float RoundFloatToPrecision(float TheFloat, int32 Precision);
    static FString GetFloatAsStringWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero=true);

    static void DoTimeDilationEffect_DEPRECATED(const UObject* WorldContextObject, TArray<AActor*> IgnoreActors);
    static void DoReverseDilationEffect_DEPRECATED(const UObject* WorldContextObject, TArray<AActor*> IgnoreActors);

    static void StopTimeForCharacter(const UObject* WorldContextObject, class ASGCharacterBase* Character);
    static void ResumeTimeForCharacter(const UObject* WorldContextObject, class ASGCharacterBase* Character);
    
    static void StopTimeForActors(const UObject* WorldContextObject, TArray<AActor*> ActorsToStop);
    static void ResumeTimeForActors(const UObject* WorldContextObject, TArray<AActor*> ActorsToResume);
    
    static void GetAllActorsInWorld(const UObject* WorldContextObject, TArray<AActor*>& OutActors, const TSet<AActor*>& IgnoreActors = TSet<AActor*>());

    static bool GetGroundLocation(const UObject* WorldContextObject, const FVector& Location, FVector& OutLocation);

    static void SetActorEnabled(AActor* Actor, bool bEnabled, bool bModifyTick = true, bool bDisableChildren = true);
    static class USpringArmComponent*  GetSpringArmComponent(AActor* Actor);
    static void SetSpringArmLagOnActor(AActor* Actor, bool bLag);

    static class USGTeamComponent* GetTeamComponent(AActor* Actor);

    static ASGCharacterBase* GetClosestTeamCharacterFromLocation(const UWorld* World, FVector Location, ESGTeam Team, float MaxDistance = -1, bool bCreateDebugSphere = false, bool bIgnoreDisabledActors = true);

    template<class T>
    static T* GetComponentByName(AActor * Actor, FString Name)
    {
        TArray<T*> Components;
        Actor->GetComponents<T>(Components);
        for (auto& Component : Components)
        {
            const UObject* BaseObject = Cast<UObject>(Component);
            if (BaseObject->GetName() == Name)
            {
                return Component;
            }
        }

        return nullptr;
    }

    static void RotateCharacterToLookTarget(AActor* Actor, const FVector& LookTarget, bool bLockZAxis = false);

    // Loads all streaming levels immediately. Note: The config will automatically unload them.
    // Note: bVisibleAfterLoad is required if you care about location, not sure what else
    //static void LoadAllStreamingLevels(const UWorld* WorldContextObject, bool bVisibleAfterLoad = false);

    // Get all actors of streaming level
    static void GetAllActorsOfStreamingLevel(const UWorld* WorldContextObject, const ULevelStreaming* LevelToStream, const TSubclassOf<AActor> ActorClass, bool bVisibleAfterLoad, TArray<AActor*>& OutActors);


    template <typename T>
    static void ShuffleArray(TArray<T>& Arr)
    {
        for (int32 i = Arr.Num() - 1; i > 0; --i)
        {
            int32 RandInt = FMath::RandRange(0, i);
            Swap(Arr[i], Arr[RandInt]);
        }
    }

    template <typename T>
    static T* GetGameInstanceSubsystem(const UObject* WorldContextObject)
    {
        if (const UGameInstance* GameInstance =  UGameplayStatics::GetGameInstance(WorldContextObject))
        {
            if (T* SubSystem = GameInstance->GetSubsystem<T>())
            {
                return SubSystem;
            }
        }

        return nullptr;
    }
};
