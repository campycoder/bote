#include "stdafx.h"
#include "TradeRoute.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"

IMPLEMENT_SERIAL (CTradeRoute, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion und Destruktion
//////////////////////////////////////////////////////////////////////
CTradeRoute::CTradeRoute(void)
{
	m_KO = CPoint(0,0);
	m_iLatinum = 0;
	m_iDuration = 0;
}

CTradeRoute::~CTradeRoute(void)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CTradeRoute::CTradeRoute(const CTradeRoute &rhs)
{
	m_KO = rhs.m_KO;
	m_iLatinum = rhs.m_iLatinum;
	m_iDuration = rhs.m_iDuration;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CTradeRoute & CTradeRoute::operator=(const CTradeRoute & rhs)
{
	if (this == &rhs)
		return *this;
	m_KO = rhs.m_KO;
	m_iLatinum = rhs.m_iLatinum;
	m_iDuration = rhs.m_iDuration;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CTradeRoute::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_KO;
		ar << m_iLatinum;
		ar << m_iDuration;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_KO;
		ar >> m_iLatinum;
		ar >> m_iDuration;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion legt die Handelsroute zum System mit der Koordinate <code>ko</code> an.
void CTradeRoute::GenerateTradeRoute(CPoint ko)
{
	m_KO = ko;
	m_iDuration = 20;
}

/// Funktion gibt das Latinum inkl. Boni, welches man durch die Handelsroute bekommt zur�ck
USHORT CTradeRoute::GetLatinum(short boni) const
{
	return (USHORT)(m_iLatinum + m_iLatinum * boni/100);
}

/// Funktion �berpr�ft, ob die Handelsroute noch Bestand haben darf und setzt das Latinum, welches
/// diese Handelsroute fabriziert. Dabei werden noch keinerlei Boni auf die Latinumproduktion angerechnet.
/// Die Funktion gibt einen Wahrheitswert zur�ck, der sagt, ob die Handelsroute noch Bestand haben darf.
BOOLEAN CTradeRoute::CheckTradeRoute(const CPoint& pFrom, const CPoint& pDest, CBotf2Doc* pDoc)
{
	ASSERT(pDoc);

	CSector* pDestSector = &(pDoc->GetSector(pDest.x, pDest.y));
	CSystem* pDestSystem = &(pDoc->GetSystem(pDest.x, pDest.y));
	CString  sOwner = pDoc->GetSystem(pFrom.x, pFrom.y).GetOwnerOfSystem();

	// wurde der Zielsektor durch uns gescannt
	if (pDestSector->GetScanned(sOwner) == FALSE)
		return FALSE;	
	// zu allererst das Latinum berechnen
	float habitants = pDestSector->GetCurrentHabitants();
	// wenn keine Leute in dem System leben, so gibt es auch keine Handelsroute.
	if (habitants == 0.0f)
		return FALSE;
	
	// zuf�llig wird das Latinum bei der Handelsroute noch modifiziert (+-)20%
	// also (rand()%41 + 80) / 100 -> [0,40]+80 = [80,120] / 100 -> [0.8,1.2]
	float mod = (float)((float)(rand()%41 + 80) / 100);

	m_iLatinum = (USHORT)(habitants * mod);
	// geh�rt der Zielsektor einer Majorrace und nicht uns?
	if (pDestSystem->GetOwnerOfSystem() != "" && pDestSystem->GetOwnerOfSystem() != sOwner)
	{
		CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(pDestSystem->GetOwnerOfSystem()));
		if (pMajor)
			if (pMajor->GetAgreement(sOwner) >= TRADE_AGREEMENT)
				return TRUE;		
	}
	// geh�rt der Zielsektor einer Minorrace
	else if (pDestSector->GetOwnerOfSector() != "" && pDestSector->GetMinorRace() == TRUE)
	{
		CMinor* pMinor = dynamic_cast<CMinor*>(pDoc->GetRaceCtrl()->GetRace(pDestSector->GetOwnerOfSector()));
		if (pMinor)
		{
			if (pMinor->GetAgreement(sOwner) >= TRADE_AGREEMENT && pMinor->GetAgreement(sOwner) != MEMBERSHIP)
				return TRUE;
			else
				return FALSE;
		}
	}
	return FALSE;
}

/// Diese Funktion verbessert manchmal die Beziehung zu der Minorrace, die in dem betroffenem Sektor lebt.
/// Als Parameter wird hierf�r die Ausgangskoordinate der Handerlsroute, die Zielkoordinate <code>pDest</code>
/// und ein Zeiger auf das Dokument <code>pDoc</code> �bergeben.
void CTradeRoute::PerhapsChangeRelationship(const CPoint& pFrom, const CPoint& pDest, CBotf2Doc* pDoc)
{
	ASSERT(pDoc);

	CSector* pDestSector = &(pDoc->GetSector(pDest.x, pDest.y));
	ASSERT(pDestSector);

	CString  sOwner = pDoc->GetSystem(pFrom.x, pFrom.y).GetOwnerOfSystem();
	if (sOwner == "")
		return;

	if (pDestSector->GetOwnerOfSector() != "" && pDestSector->GetMinorRace() == TRUE)
	{
		// Minorrace holen
		CMinor* pMinor = dynamic_cast<CMinor*>(pDoc->GetRaceCtrl()->GetRace(pDestSector->GetOwnerOfSector()));
		if (pMinor)
		{
			short relAdd = rand()%(pMinor->GetCorruptibility()+1);
			if (pDestSector->GetOwned())
				relAdd /= 2;
			/*CString s;
			s.Format("Verbesserung um %d Punkte",relAdd);
			AfxMessageBox(s);*/
			pMinor->SetRelation(sOwner, relAdd);
		}		
	}
}

/// Funktion zeichnet die Handelsroute auf der Galaxiekarte
void CTradeRoute::DrawTradeRoute(CDC* pDC, CPoint start, const CMajor* pMajor)
{
	COLORREF color = RGB(255,0,255);
	if (m_iDuration >= 0)
		color = pMajor->GetDesign()->m_clrRouteColor;
	
	CPen pen(PS_DOT, 0, color);
	pDC->SelectObject(&pen);
	pDC->MoveTo(start.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH/2,start.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT/2);
	pDC->LineTo(m_KO.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH/2,m_KO.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT/2);	
	pen.DeleteObject();
}