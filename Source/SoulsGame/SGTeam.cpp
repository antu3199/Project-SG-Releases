#include "SGTeam.h"

#include "SGUtils.h"
#include "Character/SGTeamComponent.h"

FSGOverlapContext::FSGOverlapContext()
{
}

FSGOverlapContext::FSGOverlapContext(AActor* InInstigator, AActor* InTarget, ESGTeamOverlapBehaviour InOverlapBehaviour)
{
	Instigator = MakeWeakObjectPtr(InInstigator);
	Target = MakeWeakObjectPtr(InTarget);
	OverlapBehaviour = InOverlapBehaviour;
}

bool FSGOverlapContext::ShouldOverlap() const
{
	if (OverlapBehaviour == ESGTeamOverlapBehaviour::DoNotOverlap)
	{
		return false;
	}
	
	if (!Instigator.IsValid() || !Target.IsValid())
	{
		return false;
	}

	if (OverlapBehaviour == ESGTeamOverlapBehaviour::OverlapAll)
	{
		return true;
	}

	if (OverlapBehaviour == ESGTeamOverlapBehaviour::OnlyPawns && Target->IsA<APawn>())
	{
		return true;
	}

	const USGTeamComponent* InstigatorTeamComponent =  FSGUtils::GetTeamComponent(Instigator.Get());
	check(InstigatorTeamComponent);
	
	const USGTeamComponent* TargetTeamComponent =  FSGUtils::GetTeamComponent(Target.Get());
	if (TargetTeamComponent == nullptr)
	{
		return false;
	}

	if (OverlapBehaviour == ESGTeamOverlapBehaviour::OnlyAllyPawns)
	{
		return InstigatorTeamComponent->GetTeam() == TargetTeamComponent->GetTeam();
	}
	else if (OverlapBehaviour == ESGTeamOverlapBehaviour::OnlyEnemyPawns)
	{
		return InstigatorTeamComponent->GetTeam() != TargetTeamComponent->GetTeam();
	}
	else if (OverlapBehaviour == ESGTeamOverlapBehaviour::OnlyAllies_ExceptSelf)
	{
		return InstigatorTeamComponent->GetTeam() == TargetTeamComponent->GetTeam() && Instigator != Target;
	}
	else if (OverlapBehaviour == ESGTeamOverlapBehaviour::OnlySelf)
	{
		return Instigator.Get() == Target.Get();
	}

	return true;
}
