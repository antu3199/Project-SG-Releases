// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"



#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SoulsGame/MyGameModeBase.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/SGameInstance.h"
#include "SoulsGame/SHitboxManager.h"
#include "SoulsGame/SUtils.h"
#include "SoulsGame/Abilities/PowerUps/Components/Spawned/ProjectileActor.h"

// Sets default values
AWeaponActor::AWeaponActor()
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

void AWeaponActor::PostInitProperties()
{
	Super::PostInitProperties();

	// TArray<USceneComponent> Class;
	// TArray<UNiagaraComponent*> Components;
	// GetComponents<UNiagaraComponent>(Components);

	//TArray<UObject*> SubObjects;
	//GetDefaultSubobjects(SubObjects);
	//this->P_FlameSword = Cast<UNiagaraComponent>(GetDefaultSubobjectByName("P_FlameSword"));

}

void AWeaponActor::BeginDestroy()
{
	USHitboxManager* HitboxManager = USHitboxManager::GetSingleton();
	HitboxManager->RemoveHitbox(HitboxId);
	HitboxId = -1;
	
	Super::BeginDestroy();
}

void AWeaponActor::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
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
				const bool bIsValidLocation =  FSUtils::GetGroundLocation(this, FXSpawnLocation->GetComponentLocation(), SpawnLocation);
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

bool AWeaponActor::CheckEmpowerWeapon()
{
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
		/*
		TArray<AActor*> TimestopActors;
		TSet<AActor*> IgnoreActors = TSet<AActor*>{InstigatorActor, this};
		FSUtils::GetAllActorsInWorld(this->GetWorld(), TimestopActors, IgnoreActors);
		FSUtils::StopTimeForActors(this->GetWorld(), TimestopActors);

		if (AMyGameModeBase* GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			GameMode->AddTimestopMaterial();
		}
		*/

	}

	if (bIsUsingEmpoweredAttack)
	{
		FX_P_FlameSword->Activate();
	}

	return bIsUsingEmpoweredAttack;
}

void AWeaponActor::DisableTimestopIfEmpoweredWeapon()
{
	if (!bTimestoppedByEmpoweredAttack)
	{
		return;
	}
	
	ASCharacterBase * InstigatorActor = Cast<ASCharacterBase>(GetInstigator());
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
	FSUtils::ResumeTimeForCharacter(this, InstigatorActor);
	
}

void AWeaponActor::DisableEmpowerWeapon()
{
	if (bIsUsingEmpoweredAttack)
	{
		DisableTimestopIfEmpoweredWeapon();
		FX_P_FlameSword->Deactivate();
		bIsUsingEmpoweredAttack = false;
		ASCharacterBase * InstigatorActor = Cast<ASCharacterBase>(GetInstigator());
		InstigatorActor->SetGodMode(false);
	}
}

void AWeaponActor::BroadcastWeaponBeginVisuals()
{
	OnWeaponAttackBeganVisuals.Broadcast();
}

void AWeaponActor::BroadcastWeaponEndVisuals()
{
	OnWeaponAttackEndVisuals.Broadcast();
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	this->FX_P_FlameSword = FSUtils::GetComponentByName<UNiagaraComponent>(this, "P_FlameSword");
	
	USHitboxManager* HitboxManager = USHitboxManager::GetSingleton();
	HitboxId = HitboxManager->RegisterExistingComponent(CapsuleComponent);
	//HitboxManager->AddOnOverlapBeginCallback(HitboxId, std::bind(&AAbilityActor::OnOverlapBegin, this, _1, _2, _3, _4, _5, _6));
	
	this->CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

bool AWeaponActor::CanSpawnFireActor() const
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
	const bool bIsValidLocation = FSUtils::GetGroundLocation(this, FXSpawnLocation->GetComponentLocation(), SpawnLocation);
	if (!bIsValidLocation)
	{
		return false;
	}
	
	float Distance = FMath::Abs(FXSpawnLocation->GetComponentLocation().Z - SpawnLocation.Z); 
	UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);

	return true;
}

void AWeaponActor::SetCanHitTimer()
{
	//this->CanHit = true;
	HitActors.Reset();
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (this->IsAttacking && USGameInstance::Debug_ShowHitboxes)
	if (USGameInstance::Debug_ShowHitboxes)
	{
		FSUtils::DrawCapsule(GetWorld(), this->CapsuleComponent, 1);
	}
}

void AWeaponActor::BeginWeaponAttack()
{
	
	this->IsAttacking = true;
	// this->CanHit = true
	HitActors.Reset();
	
	if (GetWorld())
	{
    	FTimerManager &WorldTimerManager = GetWorldTimerManager();
    	WorldTimerManager.ClearTimer(CanHitTimer);
	}

	this->CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly); // No Physics collision

	OnWeaponAttackBeganVisuals.Broadcast();
	OnWeaponAttackBegan.Broadcast();
}

void AWeaponActor::EndWeaponAttack()
{

	this->IsAttacking = false;
	this->CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);


	OnWeaponAttackEndVisuals.Broadcast();
	OnWeaponAttackEnd.Broadcast();
}

void AWeaponActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
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

	if (this->HitActors.Contains(TargetCharacter))
	{
		return;
	}
	
	HitActors.Add(TargetCharacter);

	// TODO: Scale with attack number
	ApplyEffectToActor(0, TargetCharacter);

	/*
	// Sending gameplay event data - I don't really like this way, but I might go back to it someday...
	FGameplayEventData GameplayEventData;
	GameplayEventData.Instigator = InstigatorActor;
	GameplayEventData.Target = OtherActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstigatorActor, this->AttackEventTag, GameplayEventData);
	*/
}

void AWeaponActor::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (HitActors.Contains(OtherActor) && GetWorld())
	{
		GetWorldTimerManager().SetTimer(CanHitTimer, this, &AWeaponActor::SetCanHitTimer,2.f /*Time you want*/, false /*if you want loop set to true*/);
	}
}

