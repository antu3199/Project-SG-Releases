// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAbilityActor.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SoulsGame/AI/SGAIController.h"
#include "BrainComponent.h"
#include "SoulsGame/Character/SGCharacterBase.h"
#include <functional>
using namespace std::placeholders;

// Sets default values
ASGAbilityActor::ASGAbilityActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Base = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Base->SetHiddenInGame(true);
	SetRootComponent(Base);
}

void ASGAbilityActor::BeginPlay()
{
	Super::BeginPlay();

	CollisionType = Base->GetCollisionEnabled();

	EnableCollision(CollisionEnabledAtStart);
	IsCollisionEnabled = CollisionEnabledAtStart;

	// TODO: Use new Subsystem
	//USHitboxManager* HitboxManager = USHitboxManager::GetSingleton();
	//HitboxId = HitboxManager->RegisterExistingComponent(Base);
	//HitboxManager->SetDebugLevel(HitboxId, ESGDebugCollisionLevel::All);

	Base->OnComponentBeginOverlap.AddDynamic(this, &ASGAbilityActor::OnOverlapBegin);
	Base->OnComponentEndOverlap.AddDynamic(this, &ASGAbilityActor::OnOverlapEnd);
}

void ASGAbilityActor::BeginDestroy()
{
	// TODO: Use new Subsystem
	// USHitboxManager* HitboxManager = USHitboxManager::GetSingleton();
	// HitboxManager->RemoveHitbox(HitboxId);
	// HitboxId = -1;
	
	Super::BeginDestroy();
}

void ASGAbilityActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsOverlapping && OtherOverlappingActor)
	{
		ACharacter * Character = Cast<ACharacter>(OtherOverlappingActor);
		if (Character)
		{
		//	FVector Direction = GetActorLocation() - OtherOverlappingActor->GetActorLocation();
//
		//	Direction.Z = 0;
		//	Direction.Normalize();
		//	Character->GetController()->StopMovement();
		//	UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
		//	CharacterMovement->AddForce(Direction * 100000);
		}
	}
}

void ASGAbilityActor::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                              bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	
}

void ASGAbilityActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ASGAbilityActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IsOverlapping = false;
	OtherOverlappingActor = nullptr;
}

void ASGAbilityActor::EnableCollision(const bool Set)
{
	if (Set)
	{
		Base->SetCollisionEnabled(CollisionType);
	}
	else
	{
		Base->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}

	IsCollisionEnabled = Set;
}

void ASGAbilityActor::ToggleCollision()
{
	EnableCollision(!IsCollisionEnabled);
}

