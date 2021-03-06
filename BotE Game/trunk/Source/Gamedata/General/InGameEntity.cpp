
#include "stdafx.h"
#include "General/InGameEntity.h"
#include "Races/Race.h"
#include "Resources.h"
#include "BoteDoc.h"
#include "Races/RaceController.h"


//////////////////////////////////////////////////////////////////////
// construction/destruction
//////////////////////////////////////////////////////////////////////
CInGameEntity::CInGameEntity(void) :
	m_Co(-1, -1),
	m_sName(),
	m_Owner(),
	m_sDescription()
{
	Reset();
}

CInGameEntity::CInGameEntity(int x, int y) :
	m_Co(x, y),
	m_sName(),
	m_Owner(),
	m_sDescription()
{
	Reset();
}

CInGameEntity::CInGameEntity(const CInGameEntity& other) :
	m_Co(other.m_Co),
	m_sName(other.m_sName),
	m_Owner(other.m_Owner),
	m_sDescription(other.m_sDescription)
{
}

CInGameEntity& CInGameEntity::operator=(const CInGameEntity& other){
	if(this == &other )
		return *this;

	m_Co = other.m_Co;
	m_sName = other.m_sName;
	m_Owner = other.m_Owner;
	m_sDescription = other.m_sDescription;

	return *this;
};

CInGameEntity::~CInGameEntity(void)
{
	Reset();
}

/// Resetfunktion f�r die Klasse CMapTile
void CInGameEntity::Reset()
{
	m_sName.Empty();
	m_Owner.reset();
	m_sDescription.Empty();
}

//////////////////////////////////////////////////////////////////////
// serialization
//////////////////////////////////////////////////////////////////////
void CInGameEntity::Serialize(CArchive &ar)
{
	if (ar.IsStoring())
	{
		ar << m_Co;
		ar << m_sName;
		ar << OwnerID();
		ar << m_sDescription;
	}
	else
	{
		ar >> m_Co;
		ar >> m_sName;
		CString owner;
		ar >> owner;
		if(owner != OwnerID())
			SetOwner(owner);
		ar >> m_sDescription;
	}
}

//////////////////////////////////////////////////////////////////////
// getting
//////////////////////////////////////////////////////////////////////

const boost::shared_ptr<CRace> CInGameEntity::Owner() const
{
	return m_Owner;
}

CString CInGameEntity::OwnerID() const
{
	if(!m_Owner)
		return CString();
	AssertBotE(!m_Owner->Deleted());
	return m_Owner->GetRaceID();
}

//////////////////////////////////////////////////////////////////////
// setting
//////////////////////////////////////////////////////////////////////

void CInGameEntity::SetOwner(const CString& id)
{
	if(id.IsEmpty())
	{
		m_Owner.reset();
		return;
	}
	m_Owner = resources::pDoc->GetRaceCtrl()->GetRaceSafe(id);
	AssertBotE(m_Owner);
}
