// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfectioniteCharacter.h"



#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SoulsGame/Abilities/CharacterAbilitySystemComponent.h"
#include "SoulsGame/Abilities/Attributes/MyAttributeSet.h"
#include "SoulsGame/Projectiles/ProjectileShooterComponent.h"

#include "Niagara/Public/NiagaraComponent.h"
#include "SoulsGame/Character/SHumanoidPlayerCharacter.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Projectiles/SShooterEnums.h"

// Sets default values
AConfectioniteCharacter::AConfectioniteCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollisionComponent);

	//AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	//AbilitySystemComponent->SetIsReplicated(true);


	//AttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("AttributeSet"));
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(BoxCollisionComponent);

	FaceActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("FaceActorComponent"));
	FaceActorComponent->SetupAttachment(SkeletalMeshComponent, TEXT("confectionite_face_location"));

	HitboxActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("HitboxActorComponent"));
	HitboxActorComponent->SetupAttachment(BoxCollisionComponent);

	
	OriginSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SGProjectilePowerup_SceneComponent0"));
	OriginSceneComponent->SetupAttachment(SkeletalMeshComponent);

	
	BaseAuraPS = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BaseAuraParticleSystem"));
	BaseAuraPS->SetupAttachment(BoxCollisionComponent);

	AuraPS = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AuraParticleSystem"));
	AuraPS->SetupAttachment(SkeletalMeshComponent);

	GlowPS = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GlowParticleSystem"));
	GlowPS->SetupAttachment(SkeletalMeshComponent);
}

// Called when the game starts or when spawned
void AConfectioniteCharacter::BeginPlay()
{
	Super::BeginPlay();

	HitboxActor = Cast<AAbilityActor>(HitboxActorComponent->GetChildActor());

	// Get the spawner at runtime so we can swap it out easily

	//GetComponents<UProjectileShooterComponent>(ProjectileSpawners);

	//InitializeSShooter(OriginSceneComponent);

	InitialBaseAuraTransform = BaseAuraPS->GetRelativeTransform();

	//this->AbilitySystemComponent->GrantAbilities(this->DefaultAbilities);

	
}



// Called every frame
void AConfectioniteCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (ConfectioniteBehaviour)
	{
		case ANIMATION:
			// TOODO: Disable input
			break;
		case LOOK_AT:
			if (LookAtTarget)
			{
				FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookAtTarget->GetActorLocation());
				SetActorRotation( Rotation.Quaternion() * FQuat::MakeFromEuler(ActorToConfectioniteOffset));

				if (LookAtTarget != PreviousLookAtTarget)
				{
					//for (auto & Spawner : ProjectileSpawners)
					//{
					//	Spawner->SetTarget(LookAtTarget->GetRootComponent());
					//}
				}
				
				PreviousLookAtTarget = LookAtTarget;
				
			}
			break;
	}

	if (BaseAuraPS->HasBegunPlay())
	{
		FTransform CurTransform = GetTransform();
		//CurTransform.SetLocation(CurTransform.GetLocation() + InitialAuraTransform.GetLocation());
		//FTransform SetTransform = InitialAuraTransform * this->GetTransform();
		//AuraPS->SetWorldTransform(InitialAuraTransform.GetLocation());
	
		BaseAuraPS->SetWorldLocation(InitialBaseAuraTransform.GetLocation() + CurTransform.GetLocation());
		BaseAuraPS->SetWorldRotation(InitialBaseAuraTransform.GetRotation());
	}
}

void AConfectioniteCharacter::InitializeAttachment(ASHumanoidPlayerCharacter* ParentCharacter)
{
	this->SetOwner(ParentCharacter);
}

/*
void AConfectioniteCharacter::DoDonutBlast(const TFunction<void()> & OnFinished)
{
	float Range = 2750.0f;
	float Duration = 3.0f;

	AActor * Target = GetClosestTarget(Range);
	if (Target == nullptr)
	{
		ReattachAfterSeconds(Duration, OnFinished);
		return;
	}

	ShotIndex = 2;

	ASHumanoidPlayerCharacter* Character = GetAttachedCharacter();

	if (Character)
	{
		Character->DetachConfectioniteCharacterTransform();

		SetActorRotation(FQuat::MakeFromEuler(ActorToConfectioniteOffset) * Character->GetActorRotation().Quaternion());
	}

	ConfectioniteBehaviour = EConfectioniteBehaviour::LOOK_AT;
	LookAtTarget = Target;

	if (ShotIndex < ProjectileSpawners.Num())
	{
		ProjectileSpawners[ShotIndex]->StartShooting();
	}

	SkeletalMeshComponent->SetMaterial(0, GlowMaterial);
	BaseAuraPS->Activate();

	FTimerHandle Handle;
	auto Callback = [=]()
	{
		ReattachToCharacter();
		if (OnFinished != nullptr)
		{
			OnFinished();
		}
		
	};

	GetWorld()->GetTimerManager().SetTimer(Handle, Callback, 1.f, false, Duration);
}

void AConfectioniteCharacter::DoDonutSpiral(const TFunction<void()>& OnFinished)
{
	ShotIndex = 4;
	float Duration = 5.0f;

	ASHumanoidPlayerCharacter* Character = GetAttachedCharacter();
	Character->DetachConfectioniteCharacterTransform();
	
	ConfectioniteBehaviour = EConfectioniteBehaviour::NONE;


	if (ShotIndex < ProjectileSpawners.Num())
	{
		ProjectileSpawners[ShotIndex]->StartShooting();
	}

	SkeletalMeshComponent->SetMaterial(0, GlowMaterial);
	BaseAuraPS->Activate();

	FTimerHandle Handle;
	auto Callback = [=]()
	{
		ReattachToCharacter();
		if (OnFinished != nullptr)
		{
			OnFinished();
		}
		
	};

	GetWorld()->GetTimerManager().SetTimer(Handle, Callback, 1.f, false, Duration);
}

void AConfectioniteCharacter::SetDebugMode(bool Set)
{
	DebugMode = Set;

	if (Set)
	{
		ASHumanoidPlayerCharacter* Character = GetAttachedCharacter();
		Character->DetachConfectioniteCharacterTransform();

		SetActorRotation(FQuat::MakeFromEuler(ActorToConfectioniteOffset) * Character->GetActorRotation().Quaternion());

		ConfectioniteBehaviour = EConfectioniteBehaviour::LOOK_AT;
		LookAtTarget = Character;

		if (ShotIndex < ProjectileSpawners.Num())
		{
			ProjectileSpawners[ShotIndex]->StartShooting();
		}

		SkeletalMeshComponent->SetMaterial(0, GlowMaterial);
		BaseAuraPS->Activate();
		//AuraPS->Activate();
	}
	else
	{
		ConfectioniteBehaviour = EConfectioniteBehaviour::NONE;

		ProjectileSpawners[ShotIndex]->StopShooting();
	}


	//FVector CharacterRotation = Character->GetActorRotation().Euler();

	//SetActorRotation(FQuat::MakeFromEuler(FVector(CharacterRotation.X + 90, -CharacterRotation.Y, -CharacterRotation.Z + 180 )));
}
*/

ASHumanoidPlayerCharacter* AConfectioniteCharacter::GetAttachedCharacter() const
{
	return Cast<ASHumanoidPlayerCharacter>(GetOwner());
}

/*
bool AConfectioniteCharacter::UseAbility(FString Name)
{
	if (!this->AbilitySystemComponent)
	{
		return false;
	}

	if (this->AbilitySystemComponent->ActivateAbilityWithTag(Name))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Activate ability %s"), *AbilityTag.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter::DoRangedAttack SUCCESSED!"));
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Confectionite Failed to activate ability %s"), *Name);
		return false;
	}
}
*/

AAbilityActor* AConfectioniteCharacter::GetHitboxActor() const
{
	return HitboxActor;
}

void AConfectioniteCharacter::ReattachToCharacter()
{

	ASHumanoidPlayerCharacter* AttachedCharacter = GetAttachedCharacter();
	if (!AttachedCharacter)
	{
		return;
	}
	
	ConfectioniteBehaviour = EConfectioniteBehaviour::NONE;
	LookAtTarget = nullptr;

	BaseAuraPS->Deactivate();
	
	if (GlowPS) GlowPS->Activate();

	// AttachedCharacter->ReattachConfectioniteCharacterTransform();

	SkeletalMeshComponent->bHiddenInGame = true;
	
	auto Callback = [&]()
	{
		SkeletalMeshComponent->SetMaterial(0, RegularMaterial);
		SkeletalMeshComponent->bHiddenInGame = false;
		if (GlowPS) GlowPS->Deactivate();
	};

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, Callback, 1.f, false, 1.f);
	

}

void AConfectioniteCharacter::ReattachAfterSeconds(float Seconds, const TFunction<void()> & OnReattachConfectionite)
{
	FTimerHandle Handle;
	auto Callback = [=]()
	{
		ReattachToCharacter();
		OnReattachConfectionite();
	};

	GetWorld()->GetTimerManager().SetTimer(Handle, Callback, 1.f, false, Seconds);
}

/*
void AConfectioniteCharacter::SetProjectileGameplayEffect(TWeakPtr<FGameplayEffectsWrapperContainer> Container)
{
	if (ShotIndex < ProjectileSpawners.Num())
	{
		ProjectileSpawners[ShotIndex]->SetGameplayEffectsUsingContainer(Container);
	}
}
*/

void AConfectioniteCharacter::DeactivatePS()
{
	BaseAuraPS->Deactivate();
	AuraPS->Deactivate();
		
	SkeletalMeshComponent->SetMaterial(0, RegularMaterial);
}

/*
void AConfectioniteCharacter::Shoot()
{
	ShotIndex = 0;

	FVector ShootLocation = GetShootLocation();
	FSUtils::DrawSphere(GetWorld(), ShootLocation, 10);

	if (ShotIndex < ProjectileSpawners.Num())
	{
		ProjectileSpawners[ShotIndex]->ActivateDebugEffects(nullptr);
		
		ProjectileSpawners[ShotIndex]->SetTargetVector(ShootLocation);
		ProjectileSpawners[ShotIndex]->StartShooting();
	}

}

void AConfectioniteCharacter::EndShoot()
{
	ShotIndex = 0;
	if (ShotIndex < ProjectileSpawners.Num())
	{
		ProjectileSpawners[ShotIndex]->StopShooting();
	}
}
*/

FVector AConfectioniteCharacter::GetShootLocation() const
{
	ASHumanoidPlayerCharacter* AttachedCharacter = GetAttachedCharacter();
	if (!AttachedCharacter)
	{
		return FVector::ZeroVector;
	}

	AController* PlayerController = AttachedCharacter->GetController();
	if (!PlayerController)
	{
		return FVector::ZeroVector;
	}
	
	FVector Location;
	FRotator Rotation;
	
	EShooterType ShooterType = EShooterType::PlayerCamera;
	float Distance = 1000;

	FVector TargetLocation;

	switch (ShooterType)
	{
		case EShooterType::PlayerForward:
			TargetLocation = AttachedCharacter->GetTransform().TransformPosition(FVector::ForwardVector * Distance);
			TargetLocation.Z = OriginSceneComponent->GetComponentLocation().Z;
			break;
		case EShooterType::PlayerCamera:

			PlayerController->GetPlayerViewPoint(Location, Rotation);
			TargetLocation = Location + Rotation.Vector() * Distance;
			break;
		default:
			break;
	}
	
	
	return TargetLocation;
}

AActor* AConfectioniteCharacter::GetClosestTarget(float Radius)
{
	if (Radius <= 0) return nullptr;

	FVector Center = GetActorLocation();

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	UClass * SeekClass = ASCharacterBase::StaticClass();


	TArray<AActor*> IgnoreActors;
	ASCharacterBase* AttachedActor = GetAttachedCharacter();
	if (AttachedActor)
	{
		IgnoreActors.Add(AttachedActor);
		IgnoreActors.Add(this);
	}

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Center, Radius, TraceObjectTypes, SeekClass, IgnoreActors, OutActors);
	
	// Visualization:
	// DrawDebugSphere(GetWorld(), Center, Radius, 12, FColor::Red, false, 3.0f);

	float ClosestActorDistance = INT_MAX;
	AActor * ClosestActor = nullptr;
	
	for (AActor* OverlappedActor : OutActors)
	{
		if (OverlappedActor == nullptr) continue;
		
		//UE_LOG(LogTemp, Log, TEXT("OverlappedActor: %s"), *overlappedActor->GetName());
		ASCharacterBase * OverlappedCharacter = Cast<ASCharacterBase>(OverlappedActor);
		if (OverlappedCharacter == nullptr || OverlappedCharacter->GetIsDead()) continue;
		
		FVector OtherActorLocation = OverlappedActor->GetActorLocation();
		float Distance = FVector::Distance(Center, OtherActorLocation);
		if (Distance < ClosestActorDistance)
		{
			ClosestActor = OverlappedActor;
			ClosestActorDistance = Distance;
		}
		
	}

	return ClosestActor;
}

/*
FVector AConfectioniteCharacter::GetDefaultTarget() const
{
	return GetShootLocation();
}
*/