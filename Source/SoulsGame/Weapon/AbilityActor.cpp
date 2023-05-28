// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityActor.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SoulsGame/AI/MyAIController.h"
#include "BrainComponent.h"
#include "SoulsGame/Character/SCharacterBase.h"
#include "SoulsGame/SHitboxManager.h"
#include <functional>
using namespace std::placeholders;

// Sets default values
AAbilityActor::AAbilityActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Base = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Base->SetHiddenInGame(true);
	SetRootComponent(Base);
}

void AAbilityActor::BeginPlay()
{
	Super::BeginPlay();

	CollisionType = Base->GetCollisionEnabled();

	EnableCollision(CollisionEnabledAtStart);
	IsCollisionEnabled = CollisionEnabledAtStart;

	
	USHitboxManager* HitboxManager = USHitboxManager::GetSingleton();
	HitboxId = HitboxManager->RegisterExistingComponent(Base);
	HitboxManager->SetDebugLevel(HitboxId, ESGDebugCollisionLevel::All);
	//HitboxManager->AddOnOverlapBeginCallback(HitboxId, std::bind(&AAbilityActor::OnOverlapBegin, this, _1, _2, _3, _4, _5, _6));

	Base->OnComponentBeginOverlap.AddDynamic(this, &AAbilityActor::OnOverlapBegin);
	Base->OnComponentEndOverlap.AddDynamic(this, &AAbilityActor::OnOverlapEnd);
}

void AAbilityActor::BeginDestroy()
{
	USHitboxManager* HitboxManager = USHitboxManager::GetSingleton();
	HitboxManager->RemoveHitbox(HitboxId);
	HitboxId = -1;
	
	Super::BeginDestroy();
}

void AAbilityActor::Tick(float DeltaSeconds)
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

void AAbilityActor::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                              bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	
}

void AAbilityActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!CanApplyEffectToActor(0, OtherActor))
	{
		return;
	}
	
	IsOverlapping = true;
	ApplyEffectToActor(0, OtherActor);

	OtherOverlappingActor = OtherActor;
	/*
	// TODO: Support impulse

	ACharacterBase * Character = Cast<ACharacterBase>(OtherOverlappingActor);
	if (Character)
	{

		FVector Direction = GetActorLocation() - OtherOverlappingActor->GetActorLocation();
		//Direction = FVector(1,0,0);

		Direction.Normalize();
		UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
	    CharacterMovement->AddImpulse(Direction * 100000);
	}
	
	*/

}

void AAbilityActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IsOverlapping = false;
	OtherOverlappingActor = nullptr;


}

void AAbilityActor::EnableCollision(const bool Set)
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

void AAbilityActor::ToggleCollision()
{
	EnableCollision(!IsCollisionEnabled);
}

