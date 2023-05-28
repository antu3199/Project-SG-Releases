#include "SGTeamComponent.h"

ESGTeam USGTeamComponent::GetTeam() const
{
	return Team;
}

void USGTeamComponent::SetTeam(ESGTeam NewTeam)
{
	Team = NewTeam;
}

ESGTeam USGTeamComponent::GetOpposingTeam() const
{
	switch (Team)
	{
	case ESGTeam::Neutral:
		return ESGTeam::Player; // Can be player or enemy
	case ESGTeam::Enemy:
		return ESGTeam::Player;
	case ESGTeam::Player:
		return ESGTeam::Enemy;
	}

	return ESGTeam::Enemy;
}
