#include "stdafx.h"

#include <cmath>
#include "float.h"

#include "MajorJoining.h"
#include "IniLoader.h"
#include "RaceController.h"
#include "General/Statistics.h"
#include "Major.h"
#include "Ships/ShipMap.h"
#include "Ships/Ships.h"
#include "Races/Minor.h"
#include "General/Loc.h"


//////////////////////////////////////////////////////////////////////
// construction
//////////////////////////////////////////////////////////////////////

CMajorJoining::CMajorJoining(void) :
	m_TimesOccured(0),
	m_StartTurn(160),
	m_RisingTurns(140),
	m_Pause(80),
	m_Randomize_by(20)
{
}

CMajorJoining::~CMajorJoining(void)
{
}

/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
/// @return Instanz dieser Klasse
CMajorJoining* CMajorJoining::GetInstance(void)
{
	static CMajorJoining instance;
	return &instance;
}

//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////

void CMajorJoining::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << m_TimesOccured;
		ar << m_StartTurn;
	}
	else
	{
		ar >> m_TimesOccured;
		ar >> m_StartTurn;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

static int randomize_by(int amount)
{
	const int sign = ((rand() % 2) == 0) ? 1 : -1;
	return  sign * (rand() % (amount + 1));
}

void CMajorJoining::Calculate(int turn, const CStatistics& stats, CRaceController& race_ctrl,
		CShipMap& ships, std::vector<CSystem>& systems)
{
	if(turn == 2)
		m_StartTurn += randomize_by(m_Randomize_by);

	if(!ShouldHappenNow(turn))
		return;

	const std::map<CString, CMajor*>& majors = *race_ctrl.GetMajors();
	CMajor* best = NULL;
	CMajor* worst = NULL;
	float best_mark = FLT_MAX;
	float worst_mark = 1.0f;
	for(std::map<CString, CMajor*>::const_iterator it = majors.begin(); it != majors.end(); ++it)
	{
		if(it->second->AHumanPlays())
			continue;

		const float mark = stats.GetMark(it->first);
		if(mark < best_mark)
		{
			best_mark = mark;
			best = it->second;
		}
		if(mark > worst_mark)
		{
			worst_mark = mark;
			worst = it->second;
		}
	}

	if(!best || !worst || best == worst)
		return;
	AssertBotE(best->GetRaceID() != worst->GetRaceID());

	m_TimesOccured++;
	m_StartTurn = turn + m_Pause + randomize_by(m_Randomize_by);

	for(CShipMap::iterator it = ships.begin(); it != ships.end(); ++it)
	{
		if(it->second->OwnerID() == worst->GetRaceID())
		{
			it->second->SetOwner(best->GetRaceID());
			it->second->SetIsShipFlagShip(FALSE);
			it->second->UnsetCurrentOrder();
			it->second->SetCombatTacticAccordingToType();
			it->second->SetTargetKO(CPoint(-1, -1));
		}
	}

	for(std::vector<CSystem>::iterator it = systems.begin(); it != systems.end(); ++it)
	{
		if(!it->Majorized() || it->OwnerID() != worst->GetRaceID())
			continue;

		const bool taken = it->Taken();
		it->ChangeOwner(
			best->GetRaceID(),
			taken ? CSystem::OWNING_STATUS_TAKEN : CSystem::OWNING_STATUS_COLONIZED_MEMBERSHIP_OR_HOME,
			false);
		it->SetFullKnown(best->GetRaceID());
		boost::shared_ptr<CRace> home_of = it->HomeOf();
		if(home_of && home_of->IsMinor())
		{
			boost::shared_ptr<CMinor> minor = boost::dynamic_pointer_cast<CMinor>(home_of);
			minor->SetOwner(race_ctrl.GetMajorSafe(best->GetRaceID()));

			if(!taken)
			{
				minor->SetRelation(best->GetRaceID(), minor->GetRelation(worst->GetRaceID()));
				minor->SetAgreement(best->GetRaceID(), DIPLOMATIC_AGREEMENT::MEMBERSHIP);
			}
		}
	}

	CEmpireNews message1;
	CString sMsgText = CLoc::GetString("MAJOR_JOINING_PART_ONE", false, worst->GetRaceNameWithArticle(),
		best->GetRaceNameWithArticle());
	message1.CreateNews(sMsgText,EMPIRE_NEWS_TYPE::DIPLOMACY);
	CEmpireNews message2;
	sMsgText = CLoc::GetString("MAJOR_JOINING_PART_TWO", false, worst->GetHomesystemName(),
		best->GetRaceNameWithArticle());
	message2.CreateNews(sMsgText,EMPIRE_NEWS_TYPE::DIPLOMACY);

	for(std::map<CString, CMajor*>::const_iterator it = majors.begin(); it != majors.end(); ++it)
	{
		it->second->GetEmpire()->AddMsg(message1);
		it->second->GetEmpire()->AddMsg(message2);
	}
}

bool CMajorJoining::ShouldHappenNow(int turn)
{

	const int times = CIniLoader::GetInstance()->ReadValueDefault("Special", "MAJORJOINING", 0);
	if(m_TimesOccured >= times)
		return false;
	if(turn < m_StartTurn)
		return false;
	if(turn > m_StartTurn + m_RisingTurns)
	{
		AssertBotE(false);
		return true;
	}

	const int prob = 100.0f *
		(pow(2.0f, pow((turn -  m_StartTurn) / static_cast<float>(m_RisingTurns), 3.0f)) - 1);
	return rand() % 100 < prob;
}