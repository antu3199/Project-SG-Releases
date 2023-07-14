#include "SGSwordRainActor.h"

#include "NiagaraComponent.h"
#include "SGAbility_SwordRain.h"
#include "SoulsGame/SGUtils.h"
#include "SoulsGame/Character/SGCharacterBase.h"
#include "SoulsGame/Character/SGTeamComponent.h"
using namespace std::placeholders;

ASGSwordRainActor::ASGSwordRainActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	this->CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	this->CapsuleComponent->SetupAttachment(GetRootComponent());
}

void ASGSwordRainActor::BeginPlay()
{
	Super::BeginPlay();

	/*
	USHitboxManager* HitboxManager = USHitboxManager::GetSingleton();
	if (HitboxManager)
	{
		HitboxId = HitboxManager->RegisterExistingComponent(CapsuleComponent);
		FSGCollisionParams Params;
		Params.ProfileType = ESGCollisionProfileType::Profile_OverlapAllDynamic;
		Params.OverlapBehaviour = ESGTeamOverlapBehaviour::OnlyEnemyPawns;
		HitboxManager->SetCollisionProfile(HitboxId, Params);
		
		HitboxManager->AddOnOverlapTickCallback(HitboxId, std::bind(&ASGSwordRainActor::OnDynamicHitboxTickOverlap, this, _1, _2), HitboxOverlapInterval);
	}
	*/
	if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(GetWorld()))
	{
		FSGHitboxParams HitboxParams;
		HitboxParams.CollisionParams.ProfileType = ESGCollisionProfileType::Profile_OverlapAll;
		HitboxParams.CollisionParams.OverlapBehaviour = ESGTeamOverlapBehaviour::OnlyEnemyPawns;
		HitboxParams.TickInterval = HitboxOverlapInterval;
		HitboxParams.EffectContext = EffectContext;
		HitboxParams.bStartEnabled = false;

		HitboxId = HitboxSubsystem->RegisterExistingComponent(HitboxParams, CapsuleComponent);
		HitboxSubsystem->OnHitboxTriggered.AddDynamic(this, &ASGSwordRainActor::OnHitboxTriggered);
	}
	
}

void ASGSwordRainActor::Initialize(USGAbility_SwordRain* InAbility)
{
	Ability = MakeWeakObjectPtr(InAbility);
}

void ASGSwordRainActor::PlayEffect()
{
	BP_PlayEffect();
}

void ASGSwordRainActor::StopEffect()
{
	BP_StopEffect();
}

void ASGSwordRainActor::BP_PlayEffect_Implementation()
{
	this->GetNiagaraComponent()->Activate();
}

void ASGSwordRainActor::BP_StopEffect_Implementation()
{
	this->GetNiagaraComponent()->Deactivate();
}

void ASGSwordRainActor::DestroySelf()
{
	if (Ability.IsValid())
	{
		Ability->OnSwordRainActorDestroy(this);
	}
	
	this->Destroy();
}

void ASGSwordRainActor::SetEffectContext(const FSGEffectInstigatorContext& InEffectContext)
{
	EffectContext = InEffectContext;

	if (HitboxId != INDEX_NONE)
	{
		if (USGHitboxSubsystem* HitboxSubsystem = FSGUtils::GetGameInstanceSubsystem<USGHitboxSubsystem>(GetWorld()))
		{
			if (USGHitboxObject* HitboxObject = HitboxSubsystem->GetHitbox(HitboxId))
			{
				HitboxObject->SetEffectContext(InEffectContext);
				HitboxObject->SetEnabled(true);
			}
		}
	}
}

void ASGSwordRainActor::OnHitboxTriggered(USGHitboxObject* HitboxObject, const FSGHitboxOverlapContext& OverlapContext)
{
	
}
