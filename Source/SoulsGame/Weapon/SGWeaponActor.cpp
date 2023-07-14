// Fill out your copyright notice in the Description page of Project Settings.


#include "SGWeaponActor.h"



#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SoulsGame/SGGameModeBase.h"
#include "SoulsGame/Character/SGCharacterBase.h"
#include "SoulsGame/SGGameInstance.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/Subsystems/SGHitboxSubsystem.h"

// Sets default values
ASGWeaponActor::ASGWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	this->CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	this->CapsuleComponent->SetupAttachment(Root);

	this->CapsuleComponent->IgnoreActorWhenMoving(this, true);

	this->SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	this->SkeletalMeshComponent->SetupAttachment(Root);
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	this->SceneComponent->SetupAttachment(Root);

	FXSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("FXSpawnLocation"));
	FXSpawnLocation->SetupAttachment(SceneComponent);

	// Note: Unfortunately, you cannot set niagara component custom parameters if done in C++ which is why this is commented out
	// WeaponSwingFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WeaponSwingFX"));
	// WeaponSwingFX->SetupAttachment(Root);
}

void ASGWeaponActor::PostInitProperties()
{
	Super::PostInitProperties();

	// TArray<USceneComponent> Class;
	// TArray<UNiagaraComponent*> Components;
	// GetComponents<UNiagaraComponent>(Components);

	//TArray<UObject*> SubObjects;
	//GetDefaultSubobjects(SubObjects);
	//this->P_FlameSword = Cast<UNiagaraComponent>(GetDefaultSubobjectByName("P_FlameSword"));

}

void ASGWeaponActor::BeginDestroy()
{
	//USHitboxManager* HitboxManager = USHitboxManager::GetSingleton();
	//HitboxManager->RemoveHitbox(HitboxId);
	//HitboxId = -1;

	if (HitboxId != INDEX_NONE)
	{
		if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(GetWorld()))
		{
			HitboxSubsystem->RemoveHitbox(HitboxId);
		}
	}

	HitboxId = INDEX_NONE;
	Super::BeginDestroy();
}

void ASGWeaponActor::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (bIsUsingEmpoweredAttack)
	{
		EmpoweredTimer += DeltaTime;
		if (EmpoweredTimer >= EmpoweredTimeForSpawningFire)
		{
			if (CanSpawnFireActor())
			{
				// Spawn fire
				FTransform ComponentTransform;
				FVector SpawnLocation;
				const bool bIsValidLocation =  FSGUtils::GetGroundLocation(this, FXSpawnLocation->GetComponentLocation(), SpawnLocation);
				if (!bIsValidLocation)
				{
					return;
				}
				
				ComponentTransform.SetLocation(SpawnLocation);
				ComponentTransform.SetScale3D(FVector(1,1,1));
	
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; //Default
				SpawnParameters.bNoFail = true;
				SpawnParameters.Instigator = GetInstigator();

				AActor * SpawnedObject = GetWorld()->SpawnActor<AActor>(this->FireTemplate, ComponentTransform, SpawnParameters);
				EmpoweredTimer = 0.0f;
			}

		}
	}
}

void ASGWeaponActor::SetEffectContext(FSGEffectInstigatorContext InEffectContext)
{
	Super::SetEffectContext(InEffectContext);
	if (HitboxId != INDEX_NONE)
	{
		if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(GetWorld()))
		{
			if (USGHitboxObject* HitboxObject = HitboxSubsystem->GetHitbox(HitboxId))
			{
				HitboxObject->SetEffectContext(InEffectContext);
			}
		}
	}
	
}

bool ASGWeaponActor::CheckEmpowerWeapon()
{
	// DEPRECATED
	/*
	if (bIsUsingEmpoweredAttack)
	{
		DisableEmpowerWeapon();
	}


	
	bIsUsingEmpoweredAttack = false;
	bTimestoppedByEmpoweredAttack = true;

	
	float ManaCost = 2.0f;
	ASCharacterBase * InstigatorActor = Cast<ASCharacterBase>(GetInstigator());
	UMyAttributeSet* AttributeSet = InstigatorActor->GetAttributeSet();
	if (AttributeSet->GetMana() - ManaCost >= 0)
	{
		UAbilitySystemComponent* AbilitySystemComponent = InstigatorActor->GetAbilitySystemComponent();
		AbilitySystemComponent->ApplyModToAttributeUnsafe(UMyAttributeSet::GetManaAttribute(), EGameplayModOp::Additive, -ManaCost);
		bIsUsingEmpoweredAttack = true;

		FSUtils::StopTimeForCharacter(this, InstigatorActor);
	}

	if (bIsUsingEmpoweredAttack)
	{
		FX_P_FlameSword->Activate();
	}


	return bIsUsingEmpoweredAttack;
	*/

	return false;
}

void ASGWeaponActor::DisableTimestopIfEmpoweredWeapon()
{
	if (!bTimestoppedByEmpoweredAttack)
	{
		return;
	}
	
	ASGCharacterBase * InstigatorActor = Cast<ASGCharacterBase>(GetInstigator());
	/*
	TArray<AActor*> TimestopActors;
	TSet<AActor*> IgnoreActors = TSet<AActor*>{InstigatorActor, this};
	FSUtils::GetAllActorsInWorld(this->GetWorld(), TimestopActors, IgnoreActors);
	FSUtils::ResumeTimeForActors(this->GetWorld(), TimestopActors);
	bTimestoppedByEmpoweredAttack = false;

	if (AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->RemoveTimestopMaterial();
	}
	*/
	FSGUtils::ResumeTimeForCharacter(this, InstigatorActor);
	
}

void ASGWeaponActor::DisableEmpowerWeapon()
{
	if (bIsUsingEmpoweredAttack)
	{
		DisableTimestopIfEmpoweredWeapon();
		FX_P_FlameSword->Deactivate();
		bIsUsingEmpoweredAttack = false;
		ASGCharacterBase * InstigatorActor = Cast<ASGCharacterBase>(GetInstigator());
		InstigatorActor->SetGodMode(false);
	}
}

void ASGWeaponActor::BroadcastWeaponBeginVisuals()
{
	OnWeaponAttackBeganVisuals.Broadcast();
}

void ASGWeaponActor::BroadcastWeaponEndVisuals()
{
	OnWeaponAttackEndVisuals.Broadcast();
}

// Called when the game starts or when spawned
void ASGWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	this->FX_P_FlameSword = FSGUtils::GetComponentByName<UNiagaraComponent>(this, "P_FlameSword");

	/*
	USHitboxManager* HitboxManager = USHitboxManager::GetSingleton();
	HitboxId = HitboxManager->RegisterExistingComponent(CapsuleComponent);

	*/
	
	if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(GetWorld()))
	{
		FSGHitboxParams HitboxParams;
		HitboxParams.CollisionParams.ProfileType = ESGCollisionProfileType::Profile_OverlapAll;
		HitboxParams.CollisionParams.OverlapBehaviour = ESGTeamOverlapBehaviour::OnlyEnemyPawns;
		HitboxParams.TickInterval = 1.0f;
		HitboxParams.EffectContext = EffectContext;
		HitboxParams.bStartEnabled = false;

		HitboxId = HitboxSubsystem->RegisterExistingComponent(HitboxParams, CapsuleComponent);	
	}
	
	//this->CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

bool ASGWeaponActor::CanSpawnFireActor() const
{
	if (!IsAttacking)
	{
		return false;
	}

	if (FireTemplate == nullptr)
	{
		return false;
	}

	FVector SpawnLocation;
	const bool bIsValidLocation = FSGUtils::GetGroundLocation(this, FXSpawnLocation->GetComponentLocation(), SpawnLocation);
	if (!bIsValidLocation)
	{
		return false;
	}
	
	float Distance = FMath::Abs(FXSpawnLocation->GetComponentLocation().Z - SpawnLocation.Z); 
	UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);

	return true;
}


// Called every frame
void ASGWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (this->IsAttacking && USGameInstance::Debug_ShowHitboxes)
	if (USGGameInstance::Debug_ShowHitboxes)
	{
		FSGUtils::DrawCapsule(GetWorld(), this->CapsuleComponent, 1);
	}
}

void ASGWeaponActor::BeginWeaponAttack()
{
	
	this->IsAttacking = true;
	// this->CanHit = true
	
	if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(GetWorld()))
	{
		if (USGHitboxObject* HitboxObject = HitboxSubsystem->GetHitbox(HitboxId))
		{
			HitboxObject->SetEnabled(true);
		}
	}
	
	//this->CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly); // No Physics collision

	OnWeaponAttackBeganVisuals.Broadcast();
	OnWeaponAttackBegan.Broadcast();
}

void ASGWeaponActor::EndWeaponAttack()
{
	this->IsAttacking = false;

	if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(GetWorld()))
	{
		if (USGHitboxObject* HitboxObject = HitboxSubsystem->GetHitbox(HitboxId))
		{
			HitboxObject->SetEnabled(false);
		}
	}
	
	//this->CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);


	OnWeaponAttackEndVisuals.Broadcast();
	OnWeaponAttackEnd.Broadcast();
}

void ASGWeaponActor::NotifyActorBeginOverlap(AActor* OtherActor)
{

	/*
	// Tests to check if you can collide with it.
	// Probably want to override this later...
	AActor * InstigatorActor = GetInstigator();
	if (InstigatorActor == nullptr || OtherActor == nullptr)
	{
		return;
	}
	
	if (InstigatorActor->GetClass() == OtherActor->GetClass())
	{
		return;
	}

	if (this->IsAttacking == false)
	{
		return;
	}

	// Do destroy effect by casting in place
	// In the future, may require more complex collision logic, but this works for now.
	if (AProjectileActor* ProjectileActor = Cast<AProjectileActor>(OtherActor))
	{
		ProjectileActor->DoDestroyEffectInPlace();
		return;
	}

	ASCharacterBase * TargetCharacter = Cast<ASCharacterBase>(OtherActor);

	if (TargetCharacter == nullptr)
	{
		return;
	}
	
	if (TargetCharacter)
	{
		if (!TargetCharacter->CanGetDamaged())
		{
			return;
		}
	}



	// TODO: Scale with attack number
	ApplyEffectToActor(0, TargetCharacter);
*/
	/*
	// Sending gameplay event data - I don't really like this way, but I might go back to it someday...
	FGameplayEventData GameplayEventData;
	GameplayEventData.Instigator = InstigatorActor;
	GameplayEventData.Target = OtherActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstigatorActor, this->AttackEventTag, GameplayEventData);
	*/
}

void ASGWeaponActor::NotifyActorEndOverlap(AActor* OtherActor)
{

}

