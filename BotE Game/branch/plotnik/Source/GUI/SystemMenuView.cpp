// SystemMenuView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "GalaxyMenuView.h"
#include "MenuChooseView.h"
#include "SystemMenuView.h"
#include "Races\RaceController.h"
#include "HTMLStringBuilder.h"
#include "RoundRect.h"
#include "Graphic\memdc.h"

short CSystemMenuView::m_iClickedOn = 0;
BYTE CSystemMenuView::m_byResourceRouteRes = TITAN;

// CSystemMenuView
IMPLEMENT_DYNCREATE(CSystemMenuView, CMainBaseView)

CSystemMenuView::CSystemMenuView()
{
}

CSystemMenuView::~CSystemMenuView()
{
	for (int i = 0; i < m_BuildMenueMainButtons.GetSize(); i++)
	{
		delete m_BuildMenueMainButtons[i];
		m_BuildMenueMainButtons[i] = 0;
	}
	m_BuildMenueMainButtons.RemoveAll();

	for (int i = 0; i < m_WorkerButtons.GetSize(); i++)
	{
		delete m_WorkerButtons[i];
		m_WorkerButtons[i] = 0;
	}
	m_WorkerButtons.RemoveAll();

	for (int i = 0; i < m_SystemTradeButtons.GetSize(); i++)
	{
		delete m_SystemTradeButtons[i];
		m_SystemTradeButtons[i] = 0;
	}
	m_SystemTradeButtons.RemoveAll();
}

BEGIN_MESSAGE_MAP(CSystemMenuView, CMainBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_XBUTTONDOWN()
END_MESSAGE_MAP()

void CSystemMenuView::OnNewRound()
{	
	m_byStartList = 0;
	m_iBOPage = 0;
	m_iELPage = 0;
	m_iSTPage = 0;
	m_iClickedOn = 0;
	m_bClickedOnBuyButton = FALSE;
	m_bClickedOnDeleteButton = FALSE;	
	m_byResourceRouteRes = TITAN;
}
// CSystemMenuView drawing

void CSystemMenuView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	SetFocus();
	
	// ZU ERLEDIGEN: Code zum Zeichnen hier einf�gen
	// Doublebuffering wird initialisiert
	CMyMemDC pDC(dc);
	CRect client;
	GetClientRect(&client);
		
	// Graphicsobjekt, in welches gezeichnet wird anlegen
	Graphics g(pDC->GetSafeHdc());
	
	g.SetSmoothingMode(SmoothingModeHighSpeed);
	g.SetInterpolationMode(InterpolationModeLowQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	g.SetCompositingQuality(CompositingQualityHighSpeed);
	g.ScaleTransform((REAL)client.Width() / (REAL)m_TotalSize.cx, (REAL)client.Height() / (REAL)m_TotalSize.cy);
	g.Clear(Color::Black);
					
	if (m_bySubMenu == 0)
		DrawBuildMenue(&g);
	else if (m_bySubMenu == 1 || m_bySubMenu == 12)
		DrawWorkersMenue(&g);
	else if (m_bySubMenu == 2)
		DrawEnergyMenue(&g);
	else if (m_bySubMenu == 3)
		DrawBuildingsOverviewMenue(&g);
	else if (m_bySubMenu == 4)
		DrawSystemTradeMenue(&g);
	DrawButtonsUnderSystemView(&g);

	g.ReleaseHDC(pDC->GetSafeHdc());
}


// CSystemMenuView diagnostics

#ifdef _DEBUG
void CSystemMenuView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CSystemMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSystemMenuView message handlers

void CSystemMenuView::OnInitialUpdate()
{
	CMainBaseView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	// Baumen�rechtecke
	m_iClickedOn = 0;
	BuildingDescription.SetRect(30,410,290,620);
	BuildingInfo.SetRect(340,560,700,650);
	AssemblyListRect.SetRect(740,400,1030,620);
	m_bySubMenu = 0;
	m_bClickedOnBuyButton = FALSE;
	m_bClickedOnDeleteButton = FALSE;
	m_bClickedOnBuildingInfoButton = TRUE;
	m_bClickedOnBuildingDescriptionButton = FALSE;
	m_iWhichSubMenu = 0;

	// Die Koodinaten der Rechtecke f�r die ganzen Buttons
	CRect r;
	GetClientRect(&r);
	// Kleine Buttons in der Baumen�ansicht
	BuildingListButton.SetRect(r.left+325,r.top+510,r.left+445,r.top+540);
	ShipyardListButton.SetRect(r.left+460,r.top+510,r.left+580,r.top+540);
	TroopListButton.SetRect(r.left+595,r.top+510,r.left+715,r.top+540);
	// Kleine Buttons unter der Bauliste/Assemblylist
	BuyButton.SetRect(r.left+750,r.top+625,r.left+870,r.top+655);
	DeleteButton.SetRect(r.left+900,r.top+625,r.left+1020,r.top+655);
	// Kleine Buttons unter der Geb�udebeschreibungs/Informations-Ansicht
	BuildingInfoButton.SetRect(r.left+30,r.top+625,r.left+150,r.top+655);
	BuildingDescriptionButton.SetRect(r.left+165,r.top+625,r.left+285,r.top+655);
	// Kleiner Umschaltbutton in der Arbeitermen�ansicht
	ChangeWorkersButton.SetRect(r.left+542,r.top+645,r.left+662,r.top+675);
	// Handelsrouten und Globales Lager Ansicht
	m_iGlobalStoreageQuantity = 1;	

	// View bei den Tooltipps anmelden
	pDoc->GetMainFrame()->AddToTooltip(this);
}

/// Funktion l�dt die rassenspezifischen Grafiken.
void CSystemMenuView::LoadRaceGraphics()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	// Alle Buttons in der View erstellen
	CreateButtons();
		
	CString sPrefix = pMajor->GetPrefix();
		
	bg_buildmenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "buildmenu.boj");
	bg_workmenu		= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "workmenu.boj");
	bg_overviewmenu = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "overviewmenu.boj");
	bg_energymenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "energymenu.boj");
	bg_systrademenu	= pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "systrademenu.boj");
}

void CSystemMenuView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	CMainBaseView::OnPrepareDC(pDC, pInfo);
}

void CSystemMenuView::OnXButtonDown(UINT nFlags, UINT nButton, CPoint point)
{
	// Dieses Feature erfordert mindestens Windows 2000.
	// Die Symbole _WIN32_WINNT und WINVER m�ssen >= 0x0500 sein.
	// TODO: F�gen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// Wenn wir in irgendeinem System sind, k�nnen wir mit dem links rechts Pfeil zu einem anderen System wechseln
	CPoint p = pDoc->GetKO();
	
	// an welcher Stelle in der Liste befindet sich das aktuelle System?
	short pos = 0;
	for (int i = 0; i < pMajor->GetEmpire()->GetSystemList()->GetSize(); i++)
	{
		if (pMajor->GetEmpire()->GetSystemList()->GetAt(i).ko == p)
		{
			pos = i;
			break;
		}
	}

	if (nButton == XBUTTON2)
	{
		pos++;
		if (pos == pMajor->GetEmpire()->GetSystemList()->GetSize())
			pos = 0;
	}
	else
	{
		pos--;
		if (pos < 0)
			pos = pMajor->GetEmpire()->GetSystemList()->GetUpperBound();
	}

	if (pMajor->GetEmpire()->GetSystemList()->GetSize() > 1)
	{
		m_iClickedOn = 0;
		m_byStartList = 0;
		pDoc->SetKO(pMajor->GetEmpire()->GetSystemList()->GetAt(pos).ko.x, pMajor->GetEmpire()->GetSystemList()->GetAt(pos).ko.y);
		Invalidate(FALSE);
	}	

	CMainBaseView::OnXButtonDown(nFlags, nButton, point);
}

BOOL CSystemMenuView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

// Funktion zum Zeichnen der Baumen�ansicht
void CSystemMenuView::DrawBuildMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	SolidBrush fontBrush(Color::White);

	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color color;
	CFontLoader::GetGDIFontColor(pMajor, 3, color);
	fontBrush.SetColor(color);
	// alte Schriftfarbe merken
	Gdiplus::Color oldColor = color;

	CString s;
	CPoint p = pDoc->GetKO();
	if (bg_buildmenu)
		g->DrawImage(bg_buildmenu, 0, 0, 1075, 750);
	
	DrawSystemProduction(g);
	
	// Farbe f�r die Markierungen ausw�hlen
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	Gdiplus::Pen pen(color);
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	fontBrush.SetColor(color);	
	
	// Anzeige der Moral und der Runden �ber der Bauliste
	s.Format("%s: %i",CResourceManager::GetString("MORAL"), pDoc->GetSystem(p).GetMoral());
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(370, 106, 335, 25), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentFar);
	g->DrawString(CResourceManager::GetString("ROUNDS").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(370, 106, 335, 25), &fontFormat, &fontBrush);
	fontFormat.SetAlignment(StringAlignmentNear);
	g->DrawString(CResourceManager::GetString("JOB").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(370, 106, 335, 25), &fontFormat, &fontBrush);
	
	// Die Struktur BuildList erstmal l�schen, alle Werte auf 0
	m_vBuildlist.RemoveAll();
	
	// Wenn man keine Schiffe zur Auswahl hat oder keine Truppen bauen kann, dann wird wieder auf das normale
	// Geb�udebaumen� umgeschaltet
	if (m_iWhichSubMenu == 1 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() == 0)
		m_iWhichSubMenu = 0;
	else if (m_iWhichSubMenu == 2 && pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() == 0)
		m_iWhichSubMenu = 0;
		
	// hier Anzeige der baubaren Geb�ude und Upgrades
	if (m_iWhichSubMenu == 0)				// Sind wir im Geb�ude/Update Untermen�
	{
		// Zuerst werden die Upgrades angezeigt
		for (int i = 0; i < pDoc->GetSystem(p).GetBuildableUpdates()->GetSize(); i++)
			m_vBuildlist.Add(pDoc->GetSystem(p).GetBuildableUpdates()->GetAt(i) * (-1));

		// Dann werden die baubaren Geb�ude angezeigt
		for (int i = 0; i < pDoc->GetSystem(p).GetBuildableBuildings()->GetSize(); i++)
			m_vBuildlist.Add(pDoc->GetSystem(p).GetBuildableBuildings()->GetAt(i));
	}
	else if (m_iWhichSubMenu == 1)		// Sind wir im Werftuntermen�?
	{
		for (int i = 0; i < pDoc->GetSystem(p).GetBuildableShips()->GetSize(); i++)
			m_vBuildlist.Add(pDoc->GetSystem(p).GetBuildableShips()->GetAt(i));
	}
	else if (m_iWhichSubMenu == 2)		// Sind wir im Kasernenuntermen�?
	{
		for (int i = 0; i < pDoc->GetSystem(p).GetBuildableTroops()->GetSize(); i++)
			m_vBuildlist.Add(pDoc->GetSystem(p).GetBuildableTroops()->GetAt(i) + 20000);
	}

	if (m_iClickedOn > 0 && m_iClickedOn > m_vBuildlist.GetUpperBound())
		m_iClickedOn = m_vBuildlist.GetUpperBound();
	if (m_iClickedOn < m_byStartList || m_iClickedOn > m_byEndList)
		m_iClickedOn = m_byStartList;
	
	// Eintr�ge der Bauliste anzeigen
	m_byEndList = m_byStartList + NOEIBL;	// auf maximal Zw�lf Eintr�ge in der Bauliste begrenzt
	int y = 150;
	for (int i = m_byStartList; i < m_vBuildlist.GetSize(); i++)
	{
		if (i > m_byEndList)
			break;

		y += 25;
		fontBrush.SetColor(oldColor);
		fontFormat.SetAlignment(StringAlignmentNear);
		fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
		
		// Markierung zeichen
		if (i == m_iClickedOn)
		{
			// Markierung worauf wir geklickt haben
			g->FillRectangle(&SolidBrush(Color(50,200,200,200)), RectF(319,y-25,403,25));
			g->DrawLine(&pen, 319, y-25, 722,y-25);
			g->DrawLine(&pen, 319, y, 722,y);
			// Farbe der Schrift w�hlen, wenn wir den Eintrag markiert haben
			fontBrush.SetColor(color);						
		}

		// noch verbleibende Runden bis das Projekt fertig wird
		int nRounds = pDoc->GetSystem(p).GetNeededRoundsToCompleteProject(m_vBuildlist[i]);
		BOOLEAN bCanAddToAssemblyList = pDoc->GetSystem(p).GetAssemblyList()->MakeEntry(m_vBuildlist[i], p, pDoc->m_System, true);
		if (!bCanAddToAssemblyList)
		{
			// Schrift dunkler darstellen, wenn das Projekt aufgrund nicht ausreichender Rohstoffe nicht gebaut werden kann
			Gdiplus::Color color2;
			fontBrush.GetColor(&color2);
			color2 = Color(100, color2.GetR(), color2.GetG(), color2.GetB());
			fontBrush.SetColor(color2);
		}
		
		// handelt es sich um ein Update?
		if (m_vBuildlist[i] < 0)
		{
			s = CResourceManager::GetString("UPGRADING", FALSE, pDoc->GetBuildingName(abs(m_vBuildlist[i])));			
		}
		// handelt es sich um ein Geb�ude?
		else if (m_vBuildlist[i] < 10000)
		{	
			s = pDoc->GetBuildingName(abs(m_vBuildlist[i]));
			CBuildingInfo* pInfo = &pDoc->GetBuildingInfo(abs(m_vBuildlist[i]));
			if (pInfo->GetWorker() == false && pInfo->GetNeverReady() == false)
			{
				Bitmap* graphic = NULL;
				if (pInfo->GetFoodProd() > 0 || pInfo->GetFoodBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\foodSmall.bop");
				else if (pInfo->GetIPProd() > 0 || pInfo->GetIndustryBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\industrySmall.bop");
				else if (pInfo->GetEnergyProd() > 0 || pInfo->GetEnergyBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\energySmall.bop");
				else if (pInfo->GetSPProd() > 0 || pInfo->GetSecurityBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\securitySmall.bop");
				else if (pInfo->GetFPProd() > 0 || pInfo->GetResearchBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\researchSmall.bop");
				else if (pInfo->GetCredits() > 0 || pInfo->GetCreditsBoni() > 0)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\creditsSmall.bop");
				if (graphic)
					g->DrawImage(graphic, 355, y-21, 20, 16);
			}
		}
		// handelt es sich um ein Schiff
		else if (m_vBuildlist[i] < 20000)
		{
			s.Format("%s-%s", pDoc->m_ShipInfoArray.GetAt(m_vBuildlist[i] - 10000).GetShipClass(), CResourceManager::GetString("CLASS"));
		}
		// handelt es sich um eine Truppe
		else
		{
			s.Format("%s", pDoc->m_TroopInfo.GetAt(m_vBuildlist[i] - 20000).GetName());
		}

		// Projekteintrag hinschreiben
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(380, y-25, 290, 25), &fontFormat, &fontBrush);
		// Anzahl der ben�tigten Runden hinschreiben
		if (nRounds)
		{
			s.Format("%i", nRounds);
			fontFormat.SetAlignment(StringAlignmentFar);
			fontFormat.SetTrimming(StringTrimmingNone);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(580, y-25, 105, 25), &fontFormat, &fontBrush);
		}
	}
	
	fontBrush.SetColor(oldColor);
	
	// Hier die Eintr�ge in der Bauliste
	this->DrawBuildList(g);

	// Arbeiterzuweisung unter dem Bild der Rasse zeichnen
	// kleine Bilder von den Rohstoffen zeichnen
	Bitmap* graphic = NULL;
	CPoint px[IRIDIUM_WORKER+1] = {CPoint(0,0)};
	
	px[FOOD_WORKER].SetPoint(80,230); px[INDUSTRY_WORKER].SetPoint(80,255); px[ENERGY_WORKER].SetPoint(80,280);
	px[SECURITY_WORKER].SetPoint(80,305); px[RESEARCH_WORKER].SetPoint(80,330);
	px[TITAN_WORKER].SetPoint(185,230); px[DEUTERIUM_WORKER].SetPoint(185,255); px[DURANIUM_WORKER].SetPoint(185,280);
	px[CRYSTAL_WORKER].SetPoint(185,305); px[IRIDIUM_WORKER].SetPoint(185,330); 

	for (int i = FOOD_WORKER; i <= IRIDIUM_WORKER; i++)
	{
		switch (i)
		{
		case FOOD_WORKER:		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\foodSmall.bop"); break;
		case INDUSTRY_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\industrySmall.bop"); break;
		case ENERGY_WORKER:		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\energySmall.bop"); break;
		case SECURITY_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\securitySmall.bop"); break;
		case RESEARCH_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\researchSmall.bop"); break;
		case TITAN_WORKER:		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\titanSmall.bop"); break;
		case DEUTERIUM_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\deuteriumSmall.bop"); break;
		case DURANIUM_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\duraniumSmall.bop"); break;
		case CRYSTAL_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\crystalSmall.bop"); break;
		case IRIDIUM_WORKER:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\iridiumSmall.bop"); break;
		}
		if (graphic)
		{
			g->DrawImage(graphic, px[i].x, px[i].y, 20, 16);
			graphic = NULL;
		}
		s.Format("%d/%d", pDoc->m_System[p.x][p.y].GetWorker(i), pDoc->m_System[p.x][p.y].GetNumberOfWorkbuildings(i, 0, NULL));
		fontFormat.SetAlignment(StringAlignmentNear);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(30 + px[i].x, px[i].y, 70, 25), &fontFormat, &fontBrush);
		
	}
	
	// Hier die Anzeige der n�tigen Rohstoffe und Industrie zum Bau des Geb�udes und dessen Beschreibung
	// auch das Bild zum aktuellen Projekt wird angezeigt	
	if (m_iClickedOn >= 0 && m_iClickedOn < m_vBuildlist.GetSize())
	{		
		int RunningNumber = abs(m_vBuildlist[m_iClickedOn]);
		
		// Berechnung der n�tigen Industrie und Rohstoffe
		if (m_bClickedOnBuyButton == FALSE && m_bClickedOnDeleteButton == FALSE)
		{
			// also ein Geb�ude oder Geb�udeupdate
			if (m_vBuildlist[m_iClickedOn] < 10000)
				pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAssemblyList()->CalculateNeededRessources(&pDoc->GetBuildingInfo(RunningNumber),0,0, pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAllBuildings(), m_vBuildlist[m_iClickedOn], pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
			// also ein Schiff
			else if (m_vBuildlist[m_iClickedOn] < 20000 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() > 0)
				pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAssemblyList()->CalculateNeededRessources(0,&pDoc->m_ShipInfoArray.GetAt(m_vBuildlist[m_iClickedOn] - 10000), 0, pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAllBuildings(), m_vBuildlist[m_iClickedOn], pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
			// also eine Truppe
			else if (pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() > 0)
				pDoc->m_System[p.x][p.y].GetAssemblyList()->CalculateNeededRessources(0,0,&pDoc->m_TroopInfo.GetAt(m_vBuildlist[m_iClickedOn] - 20000), pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetAllBuildings(), m_vBuildlist[m_iClickedOn], pMajor->GetEmpire()->GetResearch()->GetResearchInfo());

			// Anzeige der ganzen Werte				
			s = CResourceManager::GetString("BUILD_COSTS");
			fontBrush.SetColor(color);			
			fontFormat.SetAlignment(StringAlignmentCenter);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(360,562,340,30), &fontFormat, &fontBrush);
			
			fontBrush.SetColor(oldColor);			
			s.Format("%s: %i",CResourceManager::GetString("INDUSTRY"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIndustryForBuild());
			fontFormat.SetAlignment(StringAlignmentNear);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(360,590,325,25), &fontFormat, &fontBrush);
			s.Format("%s: %i",CResourceManager::GetString("TITAN"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededTitanForBuild());
			fontFormat.SetAlignment(StringAlignmentCenter);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(360,590,325,25), &fontFormat, &fontBrush);
			s.Format("%s: %i",CResourceManager::GetString("DEUTERIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDeuteriumForBuild());
			fontFormat.SetAlignment(StringAlignmentFar);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(360,590,325,25), &fontFormat, &fontBrush);
			
			s.Format("%s: %i",CResourceManager::GetString("DURANIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDuraniumForBuild());
			fontFormat.SetAlignment(StringAlignmentNear);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(360,615,325,25), &fontFormat, &fontBrush);
			s.Format("%s: %i",CResourceManager::GetString("CRYSTAL"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededCrystalForBuild());
			fontFormat.SetAlignment(StringAlignmentCenter);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(360,615,325,25), &fontFormat, &fontBrush);
			s.Format("%s: %i",CResourceManager::GetString("IRIDIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededIridiumForBuild());
			fontFormat.SetAlignment(StringAlignmentFar);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(360,615,325,25), &fontFormat, &fontBrush);
			
			if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDeritiumForBuild() > NULL)
			{				
				s.Format("%s: %i",CResourceManager::GetString("DERITIUM"), pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetNeededDeritiumForBuild());
				fontFormat.SetAlignment(StringAlignmentCenter);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(360,640,325,25), &fontFormat, &fontBrush);				
			}
		}
		
		// Hier die Beschreibung des Geb�udes bzw. die Informationen
		if (m_bClickedOnBuildingDescriptionButton == TRUE)
		{
			if (m_iWhichSubMenu == 0)		// im Geb�udeuntermen�
				s = pDoc->GetBuildingDescription(RunningNumber);
			else if (m_iWhichSubMenu == 1 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() > 0)	// im Schiffsuntermen�
				s = pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetShipDescription();
			else if (m_iWhichSubMenu == 2 && pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() > 0)	// im Kasernenuntermen�
				s = pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetDescription();
			fontFormat.SetAlignment(StringAlignmentNear);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(BuildingDescription.left, BuildingDescription.top, BuildingDescription.Width(), BuildingDescription.Height() - 10), &fontFormat, &fontBrush);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			fontFormat.SetTrimming(StringTrimmingNone);
		}
		// bzw. die Information zu dem, was das Geb�ude produziert
		if (m_bClickedOnBuildingInfoButton == TRUE)
		{
			
			if (m_iWhichSubMenu == 0)
				DrawBuildingProduction(g);
			else if (m_iWhichSubMenu == 1 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() > 0)
			{
				pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).DrawShipInformation(g, BuildingDescription, &(Gdiplus::Font(fontName.AllocSysString(), fontSize)), oldColor, color, pMajor->GetEmpire()->GetResearch());
			}
			else if (m_iWhichSubMenu == 2 && pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() > 0)
			{
				// Anzeige der Truppeneigenschaften
				fontBrush.SetColor(color);
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				fontFormat.SetAlignment(StringAlignmentCenter);
				fontFormat.SetLineAlignment(StringAlignmentNear);
				CRect r(BuildingDescription.left,BuildingDescription.top,BuildingDescription.right,BuildingDescription.bottom);				
				g->DrawString(pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetName().AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				fontBrush.SetColor(oldColor);
				r.top += 60;
				BYTE offPower = pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetOffense();
				if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetFieldStatus(1) == RESEARCHED)
					offPower += (offPower * pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(4)->GetBonus(1) / 100);
				s.Format("%s: %d",CResourceManager::GetString("OPOWER"), offPower);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				r.top += 25;
				s.Format("%s: %d",CResourceManager::GetString("DPOWER"), pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetDefense());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				r.top += 25;
				s.Format("%s: %d",CResourceManager::GetString("MORALVALUE"), pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetMoralValue());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				r.top += 25;
				s.Format("%s: %d",CResourceManager::GetString("PLACE"), pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetSize());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				r.top += 25;
				s.Format("%s: %d",CResourceManager::GetString("MAINTENANCE_COSTS"), pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetMaintenanceCosts());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left,r.top,r.Width(),r.Height()), &fontFormat, &fontBrush);
				fontFormat.SetLineAlignment(StringAlignmentCenter);
				fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			}
		}

		// Hier die Anzeige des Bildes zu dem jeweiligen Projekt		
		CString file;
		if (m_iWhichSubMenu == 0)		// sind im Geb�udeuntermen�
			file.Format("Buildings\\%s",pDoc->GetBuildingInfo(RunningNumber).GetGraphikFileName());
		else if (m_iWhichSubMenu == 1 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() > 0)	// sind im Schiffsuntermen� 
			file.Format("Ships\\%s.bop",pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetShipClass());
		else if (m_iWhichSubMenu == 2 && pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() > 0)	// sind im Kasernenuntermen�
			file.Format("Troops\\%s",pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetGraphicfile());
		graphic = NULL;
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
		if (graphic == NULL)
		{
			if (m_iWhichSubMenu == 0)		// sind im Geb�udeuntermen�
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
			else if (m_iWhichSubMenu == 1 && pDoc->m_System[p.x][p.y].GetBuildableShips()->GetSize() > 0)	// sind im Schiffsuntermen� 
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.bop");
			else if (m_iWhichSubMenu == 2 && pDoc->m_System[p.x][p.y].GetBuildableTroops()->GetSize() > 0)	// sind im Kasernenuntermen�
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Troops\\ImageMissing.bop");
		}
			
		if (graphic)
		{				
			g->DrawImage(graphic, 70, 60, 200, 150);					
		}
	}

	graphic = NULL;
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
	Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
	SolidBrush btnBrush(btnColor);
	
	int nFirstAssemblyListEntry = pDoc->GetSystem(p).GetAssemblyList()->GetAssemblyListEntry(0);
	// Hier die Anzeige der Kaufkosten, wenn wir auf den "kaufen Button" geklickt haben
	if (m_bClickedOnBuyButton == TRUE)
	{
		// Wenn was in der Bauliste steht
		if (nFirstAssemblyListEntry != 0)
		{
			CRect infoRect;
			int RunningNumber = 0;
			if (nFirstAssemblyListEntry < 0)
			{
				RunningNumber = abs(nFirstAssemblyListEntry);
				s = CResourceManager::GetString("BUY_UPGRADE", FALSE, pDoc->GetBuildingName(pDoc->BuildingInfo.GetAt(RunningNumber-1).GetPredecessorID()),pDoc->GetBuildingName(RunningNumber));
			}
			else if (nFirstAssemblyListEntry < 10000)
			{
				RunningNumber = nFirstAssemblyListEntry;
				s = CResourceManager::GetString("BUY_BUILDING", FALSE, pDoc->GetBuildingName(RunningNumber));
			}
			else if (nFirstAssemblyListEntry < 20000)
			{
				RunningNumber = nFirstAssemblyListEntry;
				s = CResourceManager::GetString("BUY_SHIP", FALSE, pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetShipTypeAsString()
					,pDoc->m_ShipInfoArray.GetAt(RunningNumber-10000).GetShipClass());
			}
			else
			{
				RunningNumber = nFirstAssemblyListEntry;
				s = CResourceManager::GetString("BUY_BUILDING", FALSE, pDoc->m_TroopInfo.GetAt(RunningNumber-20000).GetName());
			}
						
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(340,565,360,65), &fontFormat, &fontBrush);
			fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			
			CString costs;
			costs.Format("%d", pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetBuildCosts());
			s = CResourceManager::GetString("CREDITS_COSTS", FALSE, costs);
			
			fontFormat.SetLineAlignment(StringAlignmentFar);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(340,565,360,75), &fontFormat, &fontBrush);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			
			// Ja/Nein Buttons zeichnen
			// Kleine Buttons unter der Kaufkosten�bersicht			
			fontFormat.SetAlignment(StringAlignmentCenter);
			
			if (graphic)
				g->DrawImage(graphic, 355, 645, 120, 30);			
			s = CResourceManager::GetString("BTN_OKAY");
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(355,645,120,30), &fontFormat, &btnBrush);
			
			if (graphic)
				g->DrawImage(graphic, 565, 645, 120, 30);
			s = CResourceManager::GetString("BTN_CANCEL");
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(565,645,120,30), &fontFormat, &btnBrush);

			OkayButton.SetRect(355,645,475,675);
			CancelButton.SetRect(565,645,685,675);
		}
	}
	// Anzeige der Best�tigungsfrage, ob ein Auftrag wirklich abgebrochen werden soll
	if (m_bClickedOnDeleteButton == TRUE)
	{
		fontFormat.SetAlignment(StringAlignmentCenter);
		fontFormat.SetLineAlignment(StringAlignmentCenter);
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		s = CResourceManager::GetString("CANCEL_PROJECT");		
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(350,570,340,70), &fontFormat, &fontBrush);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
				
		// Ja/Nein Buttons zeichnen		
		fontFormat.SetAlignment(StringAlignmentCenter);
		
		if (graphic)
			g->DrawImage(graphic, 355, 645, 120, 30);
		s = CResourceManager::GetString("BTN_OKAY");
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(355,645,120,30), &fontFormat, &btnBrush);
		
		if (graphic)
			g->DrawImage(graphic, 565, 645, 120, 30);
		s = CResourceManager::GetString("BTN_CANCEL");
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(565,645,120,30), &fontFormat, &btnBrush);

		OkayButton.SetRect(355,645,475,675);
		CancelButton.SetRect(565,645,685,675);
	}
	
	// Anzeige der kleinen Buttons (Bauhof, Werft, Kaserne) unter der Bauliste	
	fontFormat.SetAlignment(StringAlignmentCenter);
		
	if (graphic)
		g->DrawImage(graphic, 325, 510, 120, 30);
	s = CResourceManager::GetString("BTN_BAUHOF");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(325,510,120,30), &fontFormat, &btnBrush);

	if (graphic)
		g->DrawImage(graphic, 460, 510, 120, 30);
	s = CResourceManager::GetString("BTN_DOCKYARD");
	if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetBuildableShips()->GetSize() != 0)//Schrift ausgrauen wenn keine Schiffe baubar/keine Schiffswerft 
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(460,510,120,30), &fontFormat, &btnBrush);
	else 
	{
		Gdiplus::Color color2;
		btnBrush.GetColor(&color2);
		btnBrush.SetColor(Color(100, color2.GetR(), color2.GetG(), color2.GetB()));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(460,510,120,30), &fontFormat, &btnBrush);
		btnBrush.SetColor(color2);
	}

	if (graphic)
		g->DrawImage(graphic, 595, 510, 120, 30);
	s = CResourceManager::GetString("BTN_BARRACK");
	if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetBuildableTroops()->GetSize() != 0) //Schrift ausgrauen wenn keine Truppen baubar/keine Kaserne 
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(595,510,120,30), &fontFormat, &btnBrush);
	else 
	{
		Gdiplus::Color color2;
		btnBrush.GetColor(&color2);
		btnBrush.SetColor(Color(100, color2.GetR(), color2.GetG(), color2.GetB()));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(595,510,120,30), &fontFormat, &btnBrush);
		btnBrush.SetColor(color2);
	}
	
	// plus Anzeige der kleinen Button (Info & Beschreibung) unter der Geb�udeinfobox
	if (graphic)
		g->DrawImage(graphic, 30, 625, 120, 30);
	s = CResourceManager::GetString("BTN_INFORMATION");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(30,625,120,30), &fontFormat, &btnBrush);
	if (graphic)
		g->DrawImage(graphic, 165, 625, 120, 30);
	s = CResourceManager::GetString("BTN_DESCRIPTION");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(165,625,120,30), &fontFormat, &btnBrush);	

	// plus Anzeige der kleinen Buttons unter der Assemblylist (kaufen und abbrechen)
	// wenn wir noch nicht in dieser Runde gekauft haben
	if (pDoc->GetSystem(p).GetAssemblyList()->GetWasBuildingBought() == FALSE && nFirstAssemblyListEntry != 0)
	{
		// Bei Geb�uden nur wenn es nicht ein Auftrag mit NeverReady (z.B. Kriegsrecht) ist)
		if ((nFirstAssemblyListEntry < 0)
			||			
			(nFirstAssemblyListEntry > 0 && nFirstAssemblyListEntry < 10000 && pDoc->GetBuildingInfo(nFirstAssemblyListEntry).GetNeverReady() == FALSE)
			||
			// Bei Schiffen nur, wenn eine Werft noch aktiv ist
			(nFirstAssemblyListEntry >= 10000 && nFirstAssemblyListEntry < 20000 && pDoc->GetSystem(p).GetProduction()->GetShipYard())
			||
			// Bei Truppen nur mit aktiver Kaseren
			(nFirstAssemblyListEntry >= 20000 && pDoc->GetSystem(p).GetProduction()->GetBarrack()))
		{	
			if (graphic)
				g->DrawImage(graphic, 750, 625, 120, 30);
			s = CResourceManager::GetString("BTN_BUY");
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(750,625,120,30), &fontFormat, &btnBrush);
		}
	}
	if (nFirstAssemblyListEntry != 0)
	{
		if (graphic)
			g->DrawImage(graphic, 900, 625, 120, 30);
		s = CResourceManager::GetString("BTN_CANCEL");
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(900,625,120,30), &fontFormat, &btnBrush);
	}

	// Systemnamen mit gr��erer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 4, color);
	fontBrush.SetColor(color);
	// Name des Systems oben in der Mitte zeichnen				
	s.Format("%s", pDoc->GetSector(p.x,p.y).GetName());	
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,0,m_TotalSize.cx - 15, 50), &fontFormat, &fontBrush);
}



/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen des Arbeiterzuweisungsmen�es
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawWorkersMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString s;
	CPoint p = pDoc->GetKO();
	
	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);
	
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	Color markPen;
	markPen.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	Gdiplus::Pen pen(markPen);
	
	if (bg_workmenu)
		g->DrawImage(bg_workmenu, 0, 0, 1075, 750);
	
	// Hier die Systemproduktion zeichnen
	DrawSystemProduction(g);
	
	// Hier die Eintr�ge in der Bauliste
	DrawBuildList(g);
	
	// Die Buttons zum Erh�hen bzw. Verringern der Arbeiteranzahl
	DrawGDIButtons(g, &m_WorkerButtons, -1, Gdiplus::Font(NULL), fontBrush);
		
	// Ansicht der "normalen Geb�ude"
	if (m_bySubMenu == 1)
	{
		// Ab hier Anzeige der Besetzungsbalken
		// Balken mit maximal 200 Eintr�gen, d.h. es d�rfen nicht mehr als 200 Geb�ude von einem Typ stehen!!!
		unsigned short number[5] = {0};						// Anzahl der Geb�ude eines Types
		unsigned short online[5] = {0};						// Anzahl der Geb�ude eines Types, die auch online sind
		unsigned short greatestNumber = 0;					// Gr��te Nummer der Number[5]
		unsigned short width = 0;							// Breite eines einzelnen Balkens
		unsigned short size = 0;							// Gr��e der einzelnen Balken
		// Gr��te Nummer berechnen
		for (int i = 0; i < 5; i++)
		{
			number[i] = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i,0,NULL);
			if (number[i] > greatestNumber)
				greatestNumber = number[i];
			online[i] = pDoc->GetSystem(p.x,p.y).GetWorker(i);
			// Die Rechtecke der Arbeiterbalken erstmal l�schen
			for (int j = 0; j < 200; j++)
				Timber[i][j].SetRect(0,0,0,0);
		}
		size = greatestNumber;
		if (size != 0)
			width = (unsigned short)200/size;
		if (width > 10)
			width = 10;
		else if (width < 3)
			width = 3;
		short space = width / 2;
		space = max(2, space);
				
		// Den Balken zeichnen
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < number[i]; j++)
			{
				Color timberColor;
				// Fragen ob die Geb�ude noch online sind
				if (j >= online[i])
				{
					// Dunkle Farbe wenn sie Offline sind
					timberColor = Color(42,46,30);					
				}
				else
				{
					// Helle Farbe wenn sie Online sind
					short color = j * 4;
					if (color > 230)
						color = 200;
					timberColor = Color(230-color,230-color/2,20);					
				}

				//g->FillRectangle(&darkBrush, 220 + j * width * 2, 115 + i * 95, 2 * width - 4, 35);
				CRoundRect::FillRoundRect(g, Rect(220 + j * width * 2, 115 + i * 95, 2 * width - 4, 35), timberColor, 3);
				
				// Hier werden die Rechtecke von der Gr��e noch ein klein wenig ver�ndert, damit man besser drauf klicken kann
				Timber[i][j].SetRect(220+ j*width*2-space,115+i*95,220+width+(j+1)*width*2-space,150+i*95);
			}
		}
		// Das Geb�ude �ber dem Balken zeichnen
		for (int i = 0; i < 5; i++)
		{
			CString name = "";
			USHORT tmp = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i, 1, &pDoc->BuildingInfo);
			if (tmp != 0) 
			{
				// Bild des jeweiligen Geb�udes zeichnen
				CString file;
				file.Format("Buildings\\%s", pDoc->GetBuildingInfo(tmp).GetGraphikFileName());
				Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
				if (graphic == NULL)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
				if (graphic)
				{
					g->DrawImage(graphic, 50, i * 95 + 100, 100, 75);
					graphic = NULL;
				}
				name = pDoc->GetBuildingName(tmp);
				name.Format("%d x %s",number[i],name);
				CString yield = CResourceManager::GetString("YIELD");
				fontFormat.SetAlignment(StringAlignmentNear);
				g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 90 + i * 95, 380, 25), &fontFormat, &fontBrush);
				
				if (i == 0)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetMaxFoodProd(), CResourceManager::GetString("FOOD"));
					g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
				}
				else if (i == 1)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd(), CResourceManager::GetString("INDUSTRY"));
					g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
				}
				else if (i == 2)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetMaxEnergyProd(), CResourceManager::GetString("ENERGY"));
					g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
				}
				else if (i == 3)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd(), CResourceManager::GetString("SECURITY"));
					g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
				}
				else if (i == 4)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd(), CResourceManager::GetString("RESEARCH"));
					g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
				}
			}
		}

		// Ressourcenbutton zeichnen
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
		Color btnColor;
		CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
		SolidBrush btnBrush(btnColor);		
		if (graphic)
			g->DrawImage(graphic, ChangeWorkersButton.left, ChangeWorkersButton.top, 120, 30);
		fontFormat.SetAlignment(StringAlignmentCenter);
		s = CResourceManager::GetString("BTN_RESOURCES");
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(ChangeWorkersButton.left, ChangeWorkersButton.top, 120, 30), &fontFormat, &btnBrush);
	}
	// Wenn wir in die Ressourcenansicht wollen
	if (m_bySubMenu == 12)
	{
		// Ab hier Anzeige der Besetzungsbalken
		// Balken mit maximal 200 Eintr�gen, d.h. es d�rfen nicht mehr als 200 Geb�ude von einem Typ stehen!!!
		CRect workertimber[200];
		unsigned short number[5] = {0};						// Anzahl der Geb�ude eines Types
		unsigned short online[5] = {0};
		unsigned short greatestNumber = 0;					// Gr��te Nummer der Number[5]
		unsigned short width = 0;							// Breite eines einzelnen Balkens
		unsigned short size = 0;							// Gr��e der einzelnen Balken
		// Gr��te Nummer berechnen
		for (int i = 0; i < 5; i++)
		{
			number[i] = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i+5,0,NULL);
			if (number[i] > greatestNumber)
				greatestNumber = number[i];
			online[i] = pDoc->GetSystem(p.x,p.y).GetWorker(i+5);
			// Die Rechtecke der Arbeiterbalken erstmal l�schen
			for (int j = 0; j < 200; j++)
				Timber[i][j].SetRect(0,0,0,0);
		}
		size = greatestNumber;
		if (size != 0)
			width = (unsigned short)200/size;
		if (width > 10)
			width = 10;
		else if (width < 3)
			width = 3;
		short space = width / 2;
		space = max(2, space);

		// Den Balken zeichnen
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < number[i]; j++)
			{
				Color timberColor;
				// Fragen ob die Geb�ude noch online sind
				if (j >= online[i])
				{
					// Dunkle Farbe wenn sie Offline sind
					timberColor = Color(42,46,30);					
				}
				else
				{
					// Helle Farbe wenn sie Online sind
					short color = j * 4;
					if (color > 230)
						color = 200;
					timberColor = Color(230-color,230-color/2,20);					
				}

				//g->FillRectangle(&darkBrush, 220 + j * width * 2 - space, 115 + i * 95, 2 * width - 4, 35);				
				CRoundRect::FillRoundRect(g, Rect(220 + j * width * 2, 115 + i * 95, 2 * width - 4, 35), timberColor, 3);

				// Hier werden die Rechtecke von der Gr��e noch ein klein wenig ver�ndert, damit man besser drauf klicken kann
				Timber[i][j].SetRect(220+ j*width*2-space, 115+i*95, 220+width+(j+1)*width*2-space, 150+i*95);
			}
		}
		// Das Geb�ude �ber dem Balken zeichnen
		for (int i = 0; i < 5; i++)
		{
			CString name = "";
			USHORT tmp = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i+5,1,&pDoc->BuildingInfo);
			if (tmp != 0) 
			{
				// Bild des jeweiligen Geb�udes zeichnen
				CString file;
				file.Format("Buildings\\%s", pDoc->GetBuildingInfo(tmp).GetGraphikFileName());
				Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
				if (graphic == NULL)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
				if (graphic)
				{
					g->DrawImage(graphic, 50, i * 95 + 100, 100, 75);
					graphic = NULL;
				}
				name = pDoc->GetBuildingName(tmp);
				name.Format("%d x %s",number[i],name);
				CString yield = CResourceManager::GetString("YIELD");
				fontFormat.SetAlignment(StringAlignmentNear);
				g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 90 + i * 95, 380, 25), &fontFormat, &fontBrush);
				
				if (i == 0)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetTitanProd(), CResourceManager::GetString("TITAN"));
					g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
				}
				else if (i == 1)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetDeuteriumProd(), CResourceManager::GetString("DEUTERIUM"));
					g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
				}
				else if (i == 2)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetDuraniumProd(), CResourceManager::GetString("DURANIUM"));
					g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
				}
				else if (i == 3)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetCrystalProd(), CResourceManager::GetString("CRYSTAL"));
					g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
				}
				else if (i == 4)
				{
					name.Format("%s: %d %s",yield, pDoc->GetSystem(p.x,p.y).GetProduction()->GetIridiumProd(), CResourceManager::GetString("IRIDIUM"));
					g->DrawString(name.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220, 150 + i * 95, 480, 25), &fontFormat, &fontBrush);
				}
			}
		}

		// Anzeige des Umschaltbuttons und der Erkl�rung
		Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
		Color btnColor;
		CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
		SolidBrush btnBrush(btnColor);		
		if (graphic)
			g->DrawImage(graphic, ChangeWorkersButton.left, ChangeWorkersButton.top, 120, 30);
		fontFormat.SetAlignment(StringAlignmentCenter);
		s = CResourceManager::GetString("BTN_PRODUCTION", TRUE);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(ChangeWorkersButton.left, ChangeWorkersButton.top, 120, 30), &fontFormat, &btnBrush);
	}
	
	// Hier noch die Gesamt- und freien Arbeiter unten in der Mitte zeichnen
	unsigned short width = 0;
	unsigned short size = 0;
	unsigned short worker = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(10,0,NULL);
	size = worker;
	if (size != 0)
		width = (unsigned short)200/size;
	if (width > 10)
		width = 10;
	if (width < 3)
		width = 3;
	
	// Den Balken zeichnen
	for (int i = 0; i < worker; i++)
	{
		Color timberColor;
		if (i < pDoc->GetSystem(p.x,p.y).GetWorker(11))
		{
			// Helle Farbe wenn sie Online sind
			short color = i*4;
			if (color > 230)
				color = 200;
			timberColor = Color(230-color,230-color/2,20);			
		}
		else
		{
			// Dunkle Farbe wenn sie Offline sind
			timberColor = Color(42,46,30);			
		}

		// g->FillRectangle(&lightBrush, 220 + i * width, 600, width - 2, 25);
		CRoundRect::FillRoundRect(g, Rect(220 + i * width, 600, width - 2, 25), timberColor, 2);
	}
	
	// freie Arbeiter �ber dem Balken zeichnen
	fontBrush.SetColor(normalColor);
	s.Format("%s %d/%d",CResourceManager::GetString("FREE_WORKERS"), pDoc->m_System[p.x][p.y].GetWorker(11), pDoc->m_System[p.x][p.y].GetWorker(10));
	fontFormat.SetAlignment(StringAlignmentNear);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(220,575,380,25), &fontFormat, &fontBrush);
	
	// Gro�en Text ("Arbeiterzuweisung in xxx") oben links zeichnen
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 4, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormat.SetAlignment(StringAlignmentCenter);
	// Arbeiterzuweisung auf xxx oben links zeichnen			
	s = CResourceManager::GetString("WORKERS_MENUE")+" "+pDoc->GetSector(p.x,p.y).GetName();
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,720,60), &fontFormat, &fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen der Bauwerkeansicht (auch Abriss der Geb�ude hier mgl.)
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawBuildingsOverviewMenue(Graphics* g)
{
	// Das Feld der ganzen Geb�ude mu� aufsteigend nach der RunningNumber sortiert sein.
	// Ansonsten funktioniert der Algorithmus hier nicht mehr.
	// Sortiert wird das Feld in der CalculateNumberOfWorkBuildings() Funktion der CSystem Klasse.
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CPoint p = pDoc->GetKO();;
	CString s;
	
	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color textMark;
	textMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
		
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	if (bg_overviewmenu)
		g->DrawImage(bg_overviewmenu, 0, 0, 1075, 750);

	// alte Geb�udeliste erstmal l�schen
	m_BuildingOverview.RemoveAll();
	USHORT NumberOfBuildings = pDoc->GetSystem(p.x,p.y).GetAllBuildings()->GetSize();
	// Alle Geb�ude durchgehen, diese m�ssen nach RunningNumber aufsteigend sortiert sein und in die Variable schreiben
	USHORT minRunningNumber = 0;
	short i = 0;
	short spaceX = 0;	// Platz in x-Richtung
	short spaceY = 0;
	while (i < NumberOfBuildings)
	{
		USHORT curRunningNumber = pDoc->m_System[p.x][p.y].GetAllBuildings()->GetAt(i).GetRunningNumber();
		if (curRunningNumber > minRunningNumber)
		{
			BuildingOverviewStruct bos;
			bos.runningNumber = curRunningNumber;
			m_BuildingOverview.Add(bos);
			minRunningNumber = curRunningNumber;
		}
		i++;
	}
	// Geb�ude anzeigen
	// provisorische Buttons f�r vor und zur�ck
	Color redColor;
	redColor.SetFromCOLORREF(RGB(200,50,50));
	SolidBrush redBrush(redColor);

	if (m_BuildingOverview.GetSize() > m_iBOPage * NOBIOL + NOBIOL)
	{
		s = ">";
		g->FillRectangle(&redBrush, RectF(1011,190,63,52));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(1011,190,63,52), &fontFormat, &fontBrush);
	}
	if (m_iBOPage > 0)
	{
		s = "<";
		g->FillRectangle(&redBrush, RectF(1011,490,63,52));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(1011,490,63,52), &fontFormat, &fontBrush);
	}
	
	SolidBrush blackBrush(Color::Black);
	// pr�fen, dass man nicht auf einer zu hohen Seite ist, wenn zu wenig Geb�ude vorhanden sind
	if (m_iBOPage * NOBIOL >= m_BuildingOverview.GetSize())
		m_iBOPage = 0;
	for (int i = m_iBOPage * NOBIOL; i < m_BuildingOverview.GetSize(); i++)
	{
		// Wenn wir auf der richtigen Seite sind
		if (i < m_iBOPage * NOBIOL + NOBIOL)
		{
			// Aller 4 Eintr�ge Y Platzhalter zur�cksetzen und X Platzhalter eins hoch
			if (i%4 == 0 && i != m_iBOPage * NOBIOL)
			{
				spaceX++;
				spaceY = 0;
			}

			// gro�es Rechteck, was gezeichnet wird
			g->FillRectangle(&blackBrush, RectF(60+spaceX*320,80+spaceY*150,290,120));
			CRect r(60+spaceX*320,80+spaceY*150,350+spaceX*320,200+spaceY*150);
			m_BuildingOverview[i].rect = r;
			// kleine Rechtecke, wo die verschiedenen Texte drin stehen
			CRect r1,r2;
			r1.SetRect(r.left+5,r.top,r.right-5,r.top+20);
			r2.SetRect(r.left+140,r.top,r.right-5,r.bottom);

			// Geb�udegrafik laden und anzeigen
			CString file;
			file.Format("Buildings\\%s", pDoc->GetBuildingInfo(m_BuildingOverview.GetAt(i).runningNumber).GetGraphikFileName());
			Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(file);
			if (graphic == NULL)
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
			if (graphic)
				g->DrawImage(graphic, r.left+5, r.top+25, 130, 97);
			
			//Geb�udenamen und Anzahl in den Rechtecken anzeigen
			s.Format("%i x %s",pDoc->m_System[p.x][p.y].GetNumberOfBuilding(m_BuildingOverview.GetAt(i).runningNumber),
				pDoc->GetBuildingName(m_BuildingOverview.GetAt(i).runningNumber));
			fontFormat.SetAlignment(StringAlignmentNear);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r1.left,r1.top,r1.Width(),r1.Height()), &fontFormat, &fontBrush);

			// Geb�udeproduktion zeichnen
			s = pDoc->GetBuildingInfo(m_BuildingOverview.GetAt(i).runningNumber).GetProductionAsString
				(pDoc->m_System[p.x][p.y].GetNumberOfBuilding(m_BuildingOverview.GetAt(i).runningNumber));
			SolidBrush markBrush(textMark);
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(r.left+140,r.top+25,r.Width()-140,r.Height()-5), &fontFormat, &markBrush);
			// Anzahl der abzurei�enden Geb�ude zeichnen
			unsigned short dn = pDoc->m_System[p.x][p.y].GetBuildingDestroy(m_BuildingOverview.GetAt(i).runningNumber);
			if (dn > 0)
			{
				s.Format("%s: %i",CResourceManager::GetString("TALON"), dn);
				COverlayBanner* banner = new COverlayBanner(r.TopLeft(), r.Size(), s, RGB(255,0,0));
				banner->SetBorderWidth(2);
				banner->Draw(g, &Gdiplus::Font(fontName.AllocSysString(), fontSize));
				delete banner;
			}
			spaceY++;
		}
	}

	// Wenn eine 75%ige Blockade erreicht wurde, dann ist ein Abriss nicht mehr m�glich
	if (pDoc->m_System[p.x][p.y].GetBlockade() > NULL)
	{
		COverlayBanner *banner = new COverlayBanner(CPoint(250,250), CSize(560,150), CResourceManager::GetString("ONLY_PARTIAL_BUILDINGSCRAP"), RGB(200,0,0));
		banner->Draw(g, &Gdiplus::Font(fontName.AllocSysString(), fontSize));
		delete banner;
	}

	// Systemnamen mit gr��erer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	
	// Name des Systems oben in der Mitte zeichnen				
	s = CResourceManager::GetString("BUILDING_OVERVIEW_MENUE")+" "+pDoc->GetSector(p.x,p.y).GetName();
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx,60), &fontFormat, &fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen der Energiezuweisungsansicht
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawEnergyMenue(Gdiplus::Graphics *g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CPoint p = pDoc->GetKO();;
	CString s;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);
		
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	if (bg_energymenu)
		g->DrawImage(bg_energymenu, 0, 0, 1075, 750);

	m_EnergyList.RemoveAll();
	// die Inhalte der einzelnen Buttons berechnen, max. 3 vertikal und 3 horizontal
	USHORT NumberOfBuildings = pDoc->GetSystem(p.x,p.y).GetAllBuildings()->GetSize();
	// Alle Geb�ude durchgehen, diese m�ssen nach RunningNumber aufsteigend sortiert sein und in die Variable schreiben
	short spaceX = 0;	// Platz in x-Richtung
	short spaceY = 0;	// Platz in y-Richtung
	for (int i = 0; i < NumberOfBuildings; i++)
	{
		const CBuildingInfo *buildingInfo = &pDoc->BuildingInfo.GetAt(pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(i).GetRunningNumber() - 1);

		// wenn das Geb�ude Energie ben�tigt
		if (buildingInfo->GetNeededEnergy() > 0)
		{
			ENERGYSTRUCT es;
			es.index = i;
			es.status = pDoc->m_System[p.x][p.y].GetAllBuildings()->GetAt(i).GetIsBuildingOnline();
			m_EnergyList.Add(es);
		}
	}
	
	// provisorische Buttons f�r vor und zur�ck
	Color redColor;
	redColor.SetFromCOLORREF(RGB(200,50,50));
	SolidBrush redBrush(redColor);

	if (m_EnergyList.GetSize() > m_iELPage * NOBIEL + NOBIEL)
	{
		s = ">";
		g->FillRectangle(&redBrush, RectF(1011,190,63,52));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(1011,190,63,52), &fontFormat, &fontBrush);
	}
	if (m_iELPage > 0)
	{
		s = "<";
		g->FillRectangle(&redBrush, RectF(1011,490,63,52));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(1011,490,63,52), &fontFormat, &fontBrush);
	}

	// Geb�ude, welche Energie ben�tigen anzeigen
	SolidBrush blackBrush(Color::Black);
	// pr�fen, dass man nicht auf einer zu hohen Seite ist, wenn zu wenig Geb�ude vorhanden sind
	if (m_iELPage * NOBIEL >= m_EnergyList.GetSize())
		m_iELPage = 0;
	
	for (int i = m_iELPage * NOBIEL; i < m_EnergyList.GetSize(); i++)
	{
		// Wenn wir auf der richtigen Seite sind
		if (i < m_iELPage * NOBIEL + NOBIEL)
		{
			// Aller 3 Eintr�ge Y Platzhalter zur�cksetzen und X Platzhalter eins hoch
			if (i%3 == 0 && i != m_iELPage * NOBIEL)
			{
				spaceX++;
				spaceY = 0;
			}
			// gro�es Rechteck, was gezeichnet wird
			CRect r;
			r.SetRect(90+spaceX*320,100+spaceY*170,320+spaceX*320,247+spaceY*170);
			g->FillRectangle(&blackBrush, RectF(r.left, r.top, r.Width(), r.Height()));
			m_EnergyList[i].rect = r;
			// Rechteck machen damit der Text oben rechts steht
			CRect tmpr;
			tmpr.SetRect(r.left+10,r.top+5,r.right-5,r.bottom);
			int id = pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(m_EnergyList.GetAt(i).index).GetRunningNumber();
			
			const CBuildingInfo *buildingInfo = &pDoc->BuildingInfo.GetAt(id - 1);

			fontFormat.SetAlignment(StringAlignmentNear);
			fontFormat.SetLineAlignment(StringAlignmentNear);
			g->DrawString(pDoc->GetBuildingName(id).AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(tmpr.left, tmpr.top, tmpr.Width(), tmpr.Height()), &fontFormat, &fontBrush);
			
			// n�tige Energie �ber den Status zeichnen
			fontFormat.SetAlignment(StringAlignmentCenter);
			fontFormat.SetLineAlignment(StringAlignmentCenter);
			tmpr.SetRect(r.right-70,r.bottom-100,r.right-5,r.bottom);
			s.Format("%d EP", buildingInfo->GetNeededEnergy());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(tmpr.left, tmpr.top, tmpr.Width(), tmpr.Height()), &fontFormat, &fontBrush);
			
			// Rechteck machen damit der Status unten rechts steht
			tmpr.SetRect(r.right-70,r.bottom-50,r.right-5,r.bottom);
			// Wenn es offline ist
			if (!m_EnergyList.GetAt(i).status)
			{
				g->DrawString(L"offline", -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(tmpr.left, tmpr.top, tmpr.Width(), tmpr.Height()), &fontFormat, &SolidBrush(Color::Red));
				
			}
			// Wenn es online ist
			else
			{				
				g->DrawString(L"online", -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(tmpr.left, tmpr.top, tmpr.Width(), tmpr.Height()), &fontFormat, &SolidBrush(Color::Green));
			}
			
			// Das Bild zu dem Geb�ude zeichnen			
			CString fileName;
			fileName.Format("Buildings\\%s", buildingInfo->GetGraphikFileName());
			Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(fileName);
			if (graphic == NULL)
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
			if (graphic)
			{
				g->DrawImage(graphic, r.left + 5, r.top + 32, 150, 113);				
			}
			spaceY++;
		}
	}

	// Oben in der Mitte den aktuellen/verf�gbaren Energiebetrag zeichnen
	int energy = pDoc->GetSystem(p.x,p.y).GetProduction()->GetEnergyProd();
	s.Format("%s: %d",CResourceManager::GetString("USABLE_ENERGY"), energy);
	Color energyColor;
	if (energy < 0)
		energyColor.SetFromCOLORREF(RGB(200,0,0));
	else if (energy == 0)
		energyColor.SetFromCOLORREF(RGB(200,200,0));
	else
		energyColor.SetFromCOLORREF(RGB(0,200,0));
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(190, 65, 675, 30), &fontFormat, &SolidBrush(energyColor));

	// Systemnamen mit gr��erer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	
	// Name des Systems oben in der Mitte zeichnen				
	s = CResourceManager::GetString("ENERGY_MENUE")+" "+pDoc->GetSector(p.x,p.y).GetName();
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx,60), &fontFormat, &fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hier die Funktion zum Zeichnen der Systemhandelsansicht
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawSystemTradeMenue(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CPoint p = pDoc->GetKO();;
	CString s,s2;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	SolidBrush fontBrush(normalColor);

	Gdiplus::Color textMark;
	textMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
		
	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);

	if (bg_systrademenu)
		g->DrawImage(bg_systrademenu, 0, 0, 1075, 750);

	SolidBrush markBrush(textMark);
	g->DrawString(CResourceManager::GetString("TRADE_AND_RESOURCEROUTES").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,80,538,25), &fontFormat, &markBrush);
	g->DrawString(CResourceManager::GetString("STELLAR_STORAGE").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,80,538,25), &fontFormat, &markBrush);
	g->DrawString(CResourceManager::GetString("SYSTEM_STORAGE").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,140,268,25), &fontFormat, &markBrush);
	g->DrawString(CResourceManager::GetString("STELLAR_STORAGE").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(806,140,268,25), &fontFormat, &markBrush);
	g->DrawString(CResourceManager::GetString("TITAN").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,220,538,60), &fontFormat, &markBrush);
	g->DrawString(CResourceManager::GetString("DEUTERIUM").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,280,538,60), &fontFormat, &markBrush);
	g->DrawString(CResourceManager::GetString("DURANIUM").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,340,538,60), &fontFormat, &markBrush);
	g->DrawString(CResourceManager::GetString("CRYSTAL").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,400,538,60), &fontFormat, &markBrush);
	g->DrawString(CResourceManager::GetString("IRIDIUM").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,460,538,60), &fontFormat, &markBrush);
	
	USHORT maxTradeRoutes = (USHORT)(pDoc->m_System[p.x][p.y].GetHabitants() / TRADEROUTEHAB) + pDoc->m_System[p.x][p.y].GetProduction()->GetAddedTradeRoutes();
	short addResRoute = 1;
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Handel" -> mindestens eine Handelsroute
	if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetFieldStatus(3) == RESEARCHED)
		if (maxTradeRoutes == NULL)
		{
			maxTradeRoutes += pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetBonus(3);
			addResRoute += pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(11)->GetBonus(3);
		}
	s.Format("%d", maxTradeRoutes);
	///// HIER DIE BONI DURCH SPEZIALFORSCHUNG //////
	// Hier die Boni durch die Uniqueforschung "Lager und Transport" -> eine Ressourcenroute mehr
	if (pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(10)->GetFieldStatus(3) == RESEARCHED)
		addResRoute += pMajor->GetEmpire()->GetResearch()->GetResearchInfo()->GetResearchComplex(10)->GetBonus(3);
	USHORT maxResourceRoutes = (USHORT)(pDoc->m_System[p.x][p.y].GetHabitants() / TRADEROUTEHAB) + pDoc->m_System[p.x][p.y].GetProduction()->GetAddedTradeRoutes() + addResRoute;
	s2.Format("%d", maxResourceRoutes);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	g->DrawString(CResourceManager::GetString("SYSTEM_SUPPORTS_ROUTES",0,s,s2).AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(60,140,440,40), &fontFormat, &fontBrush);
	
	s.Format("%d",pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetSize());
	s2.Format("%d",pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetSize());
	g->DrawString(CResourceManager::GetString("SYSTEM_HAS_ROUTES",0,s,s2).AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(60,180,440,40), &fontFormat, &fontBrush);
	
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	g->DrawString(CResourceManager::GetString("ROUTES_TO").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(60,220,440,30), &fontFormat, &fontBrush);
	
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);
	// Anzeige von max. NOTRIL Handelsrouten
	// pr�fen, dass man nicht auf einer zu hohen Seite ist, wenn zu wenig Handelsrouten vorhanden sind
	if (m_iSTPage * NOTRIL >= pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetSize())
		m_iSTPage = 0;
	int numberOfTradeRoutes = 0;
	// zuerst die Handelsrouten anzeigen
	for (int i = m_iSTPage * NOTRIL; i < pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetSize(); i++)
	{
		CPoint dest = pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetAt(i).GetDestKO();
		if (pDoc->GetSector(dest).GetKnown(pDoc->m_System[p.x][p.y].GetOwnerOfSystem()) == TRUE)
			s = pDoc->GetSector(dest).GetName();
		else
			s.Format("%s %c%i",CResourceManager::GetString("SECTOR"),(char)(dest.y+97),dest.x+1);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(70,260+i*30,125,25), &fontFormat, &fontBrush);
		
		// Gewinn inkl. der Boni auf Handelsrouten ohne Boni auf Credits und Boni durch Moral
		USHORT lat = pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetAt(i).GetCredits(pDoc->m_System[p.x][p.y].GetProduction()->GetIncomeOnTradeRoutes());
		s.Format("%s: %d %s",CResourceManager::GetString("PROFIT"), lat, CResourceManager::GetString("CREDITS"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,260+i*30,170,25), &fontFormat, &fontBrush);
		
		// verbleibende Dauer der Handelsroute anzeigen
		short duration = pDoc->m_System[p.x][p.y].GetTradeRoutes()->GetAt(i).GetDuration();
		if (duration < 0)
			duration = 6-abs(duration);
		if (duration > 1)
			s.Format("%s %d %s",CResourceManager::GetString("STILL"), duration, CResourceManager::GetString("ROUNDS"));
		else
			s.Format("%s %d %s",CResourceManager::GetString("STILL"), duration, CResourceManager::GetString("ROUND"));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(380,260+i*30,130,25), &fontFormat, &fontBrush);
		numberOfTradeRoutes = i + 1;
		if (i >= m_iSTPage * NOTRIL + NOTRIL - 1)
			break;
	}
	// jetzt die Ressourcenrouten anzeigen
	for (int i = 0; i < pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetSize(); i++)
	{
		int j = i + numberOfTradeRoutes;
		CPoint dest = pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetAt(i).GetKO();
		s.Format("%s", pDoc->GetSector(dest).GetName());
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(70,260+j*30,125,25), &fontFormat, &markBrush);
		switch (pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetAt(i).GetResource())
		{
		case TITAN:		s = CResourceManager::GetString("TITAN"); break;
		case DEUTERIUM: s = CResourceManager::GetString("DEUTERIUM"); break;
		case DURANIUM:	s = CResourceManager::GetString("DURANIUM"); break;
		case CRYSTAL:	s = CResourceManager::GetString("CRYSTAL"); break;
		case IRIDIUM:	s = CResourceManager::GetString("IRIDIUM"); break;
		default:		s = "";
		}
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(200,260+j*30,150,25), &fontFormat, &markBrush);
		if (j >= m_iSTPage * NOTRIL + NOTRIL - 1)
			break;
	}
		
	// Inhalte des system- und globalen Lagers zeichnen
	Gdiplus::Color penMark;
	penMark.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);		
	fontFormat.SetTrimming(StringTrimmingNone);
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		fontFormat.SetAlignment(StringAlignmentNear);
		s.Format("%d",pDoc->m_System[p.x][p.y].GetResourceStore(i));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(668,220+i*60,407,60), &fontFormat, &fontBrush);
		// in Klammern darunter, wieviel Ressourcen ich n�chste Runde aus diesem System ins Globale Lager verschiebe
		s.Format("(%d)", pMajor->GetEmpire()->GetGlobalStorage()->GetSubResource(i,p) - pMajor->GetEmpire()->GetGlobalStorage()->GetAddedResource(i,p));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(668,265+i*60,407,25), &fontFormat, &fontBrush);
		// globalen Lagerinhalt zeichnen
		fontFormat.SetAlignment(StringAlignmentFar);
		s.Format("%d", pMajor->GetEmpire()->GetGlobalStorage()->GetResourceStorage(i));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,220+i*60,407,60), &fontFormat, &fontBrush);
		// in Klammern steht, wieviel dieser Ressource n�chste Runde aus dem Lager entfernt wird
		s.Format("(%d)", pMajor->GetEmpire()->GetGlobalStorage()->GetAllAddedResource(i) - pMajor->GetEmpire()->GetGlobalStorage()->GetAllSubResource(i));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,265+i*60,407,25), &fontFormat, &fontBrush);		
	}
	// Die Buttons zum Ver�ndern der Mengen im stellaren Lager zeichnen
	// Buttons zeichnen
	for (int i = 0; i < m_SystemTradeButtons.GetSize(); i++)
		m_SystemTradeButtons[i]->DrawButton(*g, pDoc->GetGraphicPool(), Gdiplus::Font(NULL), fontBrush);

	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	s.Format("%d",pMajor->GetEmpire()->GetGlobalStorage()->GetTakenRessources());
	s2.Format("%d",pMajor->GetEmpire()->GetGlobalStorage()->GetMaxTakenRessources());
	g->DrawString(CResourceManager::GetString("TAKE_FROM_STORAGE",0,s,s2).AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,190,538,25), &fontFormat, &fontBrush);
		
	s.Format("%d",pMajor->GetEmpire()->GetGlobalStorage()->GetLosing());
	g->DrawString(CResourceManager::GetString("LOST_PER_ROUND",0,s).AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,550,538,30), &fontFormat, &fontBrush);
	
	fontFormat.SetAlignment(StringAlignmentFar);
	g->DrawString(CResourceManager::GetString("MULTIPLIER").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(538,600,182,30), &fontFormat, &fontBrush);

	// kleine Buttons zeichnen
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\" + pMajor->GetPrefix() + "button_small.bop");
	Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 1, btnColor);
	SolidBrush btnBrush(btnColor);
	fontFormat.SetAlignment(StringAlignmentCenter);
	
	// Button zum Anlegen einer Handelsroute zeichnen
	if (graphic)
		g->DrawImage(graphic, 60, 600, 120, 30);
	s = CResourceManager::GetString("BTN_TRADEROUTE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(60, 600, 120, 30), &fontFormat, &btnBrush);
	
	// Button zum Anlegen einer Ressourcenroute zeichnen
	if (graphic)
		g->DrawImage(graphic, 360, 600, 120, 30);
	s = CResourceManager::GetString("BTN_RESOURCEROUTE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(360, 600, 120, 30), &fontFormat, &btnBrush);
	
	// Button zum �ndern der Ressource bei einer Ressourcenroute
	if (graphic)
		g->DrawImage(graphic, 360, 640, 120, 30);
	switch (m_byResourceRouteRes)
	{
	case TITAN:		s = CResourceManager::GetString("TITAN"); break;
	case DEUTERIUM: s = CResourceManager::GetString("DEUTERIUM"); break;
	case DURANIUM:	s = CResourceManager::GetString("DURANIUM"); break;
	case CRYSTAL:	s = CResourceManager::GetString("CRYSTAL"); break;
	case IRIDIUM:	s = CResourceManager::GetString("IRIDIUM"); break;
	default:		s = "";
	}
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(360, 640, 120, 30), &fontFormat, &btnBrush);
	
	// Buttons zum K�ndigen/Aufheben einer Ressourcenroute zeichnen
	for (int i = 0; i < pDoc->m_System[p.x][p.y].GetResourceRoutes()->GetSize(); i++)
	{
		int j = i + numberOfTradeRoutes;
		if (graphic)
			g->DrawImage(graphic, 360, 260 + j * 30, 120, 30);
		g->DrawString(CResourceManager::GetString("BTN_ANNUL").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(360, 260+j*30, 120, 30), &fontFormat, &btnBrush);
	}

	// Button zum �ndern der Menge, wieviel pro Klick vom oder ins Globale Lager verschoben werden zeichnen
	if (graphic)
		g->DrawImage(graphic, 747, 600, 120, 30);
	s.Format("%d",m_iGlobalStoreageQuantity);
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(747, 600, 120, 30), &fontFormat, &btnBrush);	
	
	// Systemnamen mit gr��erer Schrift in der Mitte zeichnen
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	// Schriftfarbe w�hlen
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	fontBrush.SetColor(normalColor);

	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	
	// Name des Systems oben in der Mitte zeichnen				
	s.Format("%s %s",CResourceManager::GetString("TRADEOVERVIEW_IN"),pDoc->GetSector(p.x,p.y).GetName());
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(0,10,m_TotalSize.cx,60), &fontFormat, &fontBrush);
}


/////////////////////////////////////////////////////////////////////////////////////////	
// Ab hier die ganzen Buttons mit ihrer Beschreibung am unteren Bildschirmrand
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawButtonsUnderSystemView(Graphics* g)
{	
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	
	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color btnColor;
	CFontLoader::GetGDIFontColor(pMajor, 2, btnColor);
	SolidBrush fontBrush(btnColor);
		
	DrawGDIButtons(g, &m_BuildMenueMainButtons, m_bySubMenu, Gdiplus::Font(fontName.AllocSysString(), fontSize), fontBrush);
}

/////////////////////////////////////////////////////////////////////////////////////////	
// Funktion zeichnet die Bauliste in der Baumen�- und Arbeitermen�ansicht
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawBuildList(Graphics* g)
{
	// Hier die Eintr�ge in der Bauliste
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	SolidBrush fontBrush(Color::White);

	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
	
	Color firstColor;
	firstColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	SolidBrush usedBrush(normalColor);

	Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	Pen rectPen(markColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);	
	
	int RoundToBuild;
	CPoint p = pDoc->GetKO();
	CString m_strAssemblyListEntry("");
	
	int y = 410;
	for (int i = 0; i < ALE; i++)
	{
		int nAssemblyListEntry = pDoc->GetSystem(p).GetAssemblyList()->GetAssemblyListEntry(i);
		if (nAssemblyListEntry != 0)
		{
			// ersten Eintrag in der Bauliste
			if (i == 0)
			{
				// Farbe der Schrift f�r den ersten Eintrag in der Bauliste w�hlen
				usedBrush.SetColor(firstColor);
			}
			
			CString sFile;	
			// ist es ein Update
			if (nAssemblyListEntry < 0)
			{
				m_strAssemblyListEntry = CResourceManager::GetString("UPGRADING", FALSE, pDoc->GetBuildingName(abs(nAssemblyListEntry)));
				sFile = "Buildings//" + pDoc->GetBuildingInfo(abs(nAssemblyListEntry)).GetGraphikFileName();
			}
			// ist es ein Geb�ude
			else if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i) < 10000)
			{
				m_strAssemblyListEntry = pDoc->GetBuildingName(nAssemblyListEntry);
				sFile = "Buildings//" + pDoc->GetBuildingInfo(nAssemblyListEntry).GetGraphikFileName();
			}
			// ist es ein Schiff
			else if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(i) < 20000)
			{
				m_strAssemblyListEntry.Format("%s-%s",pDoc->m_ShipInfoArray.GetAt(nAssemblyListEntry - 10000).GetShipClass(), CResourceManager::GetString("CLASS"));
				sFile = "Ships//" + pDoc->m_ShipInfoArray.GetAt(nAssemblyListEntry - 10000).GetShipClass() + ".bop";
			}
			// ist es eine Truppe
			else
			{
				m_strAssemblyListEntry = pDoc->m_TroopInfo.GetAt(nAssemblyListEntry - 20000).GetName();
				sFile = "Troops//" + pDoc->m_TroopInfo.GetAt(nAssemblyListEntry - 20000).GetName() + ".bop";
			}
			
			fontFormat.SetAlignment(StringAlignmentNear);
			CRect rect(760,y,1000,y+24);
			g->DrawString(m_strAssemblyListEntry.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(rect.left + 25, rect.top, (rect.right - rect.left) - 25, rect.bottom - rect.top), &fontFormat, &usedBrush);
			
			// kleines Symbol in der Bauliste vom Auftrag zeichnen
			if (sFile != "")
			{
				Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic(sFile);
				if (graphic)
					g->DrawImage(graphic, 750, y, 25, 20);
			}
			g->DrawRectangle(&rectPen, 749, y-1, 27, 22);
									
			// Hier Berechnung der noch verbleibenden Runden, bis das Projekt fertig wird (nicht bei NeverReady-Auftr�gen)
			// divide by zero check
			if (pDoc->GetSystem(p).GetProduction()->GetIndustryProd() > 0)
			{
				if (nAssemblyListEntry > 0 && nAssemblyListEntry < 10000 && pDoc->GetBuildingInfo(nAssemblyListEntry).GetNeverReady())
				{
					RoundToBuild = pDoc->GetSystem(p).GetAssemblyList()->GetNeededIndustryInAssemblyList(i);
					m_strAssemblyListEntry.Format("%i", RoundToBuild);
				}
				// Bei Upgrades
				else if (nAssemblyListEntry < 0)
				{
					RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p).GetAssemblyList()->GetNeededIndustryInAssemblyList(i))
						/((float)pDoc->GetSystem(p).GetProduction()->GetIndustryProd()
							* (100+pDoc->GetSystem(p).GetProduction()->GetUpdateBuildSpeed())/100));
					m_strAssemblyListEntry.Format("%i",RoundToBuild);
				}
				// Bei Geb�uden
				else if (nAssemblyListEntry < 10000)
				{
					RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p).GetAssemblyList()->GetNeededIndustryInAssemblyList(i))
						/((float)pDoc->GetSystem(p).GetProduction()->GetIndustryProd()
							* (100+pDoc->GetSystem(p).GetProduction()->GetBuildingBuildSpeed())/100));
					m_strAssemblyListEntry.Format("%i",RoundToBuild);
				}
				// Bei Schiffen Wertfeffiziens mitbeachten
				else if (nAssemblyListEntry < 20000 && pDoc->GetSystem(p).GetProduction()->GetShipYardEfficiency() > 0)
				{					
					RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p).GetAssemblyList()->GetNeededIndustryInAssemblyList(i))
						/((float)pDoc->GetSystem(p).GetProduction()->GetIndustryProd() * pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipYardEfficiency() / 100
							* (100+pDoc->GetSystem(p).GetProduction()->GetShipBuildSpeed())/100));
					m_strAssemblyListEntry.Format("%i",RoundToBuild);
				}
				// Bei Truppen die Kaserneneffiziens beachten
				else if (pDoc->GetSystem(p).GetProduction()->GetBarrackEfficiency() > 0)
				{
					RoundToBuild = (int)ceil((float)(pDoc->GetSystem(p).GetAssemblyList()->GetNeededIndustryInAssemblyList(i))
						/((float)pDoc->GetSystem(p).GetProduction()->GetIndustryProd() * pDoc->GetSystem(p.x,p.y).GetProduction()->GetBarrackEfficiency() / 100
							* (100+pDoc->GetSystem(p).GetProduction()->GetTroopBuildSpeed())/100));
					m_strAssemblyListEntry.Format("%i",RoundToBuild);
				}
				else
					m_strAssemblyListEntry.Format("?");

				fontFormat.SetAlignment(StringAlignmentFar);
				g->DrawString(m_strAssemblyListEntry.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(765,y,260,25), &fontFormat, &usedBrush);
			}
			y += 25;
			usedBrush.SetColor(normalColor);
		}
	}
	
	// Wenn nix in der Bauliste steht, automatisch Handelsg�ter reinschreiben
	if (pDoc->GetSystem(p).GetAssemblyList()->GetAssemblyListEntry(0) == 0)
	{		
		usedBrush.SetColor(firstColor);
		g->DrawString(CResourceManager::GetString("COMMODITIES").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(760,410,285,25), &fontFormat, &usedBrush);		
	}
}

/////////////////////////////////////////////////////////////////////////////////////////	
// Hier werden die Informationen zur Produktion usw. oben rechts in der Systemansicht ausgegeben
/////////////////////////////////////////////////////////////////////////////////////////
void CSystemMenuView::DrawSystemProduction(Graphics* g)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);
		
	Color penColor;
	penColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	SolidBrush fontBrush(penColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	fontFormat.SetTrimming(StringTrimmingEllipsisCharacter);

	CString s;
	CPoint p;
	p = pDoc->GetKO();	

	// Die Rohstoffe und sonstige Informationen im System oben in der Mitte anzeigen
	// Lager und Rohstoffe rechts zeichnen
	g->DrawString(CResourceManager::GetString("PRODUCTION").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(870,20), &fontFormat, &fontBrush);
	g->DrawString(CResourceManager::GetString("STORAGE").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(980,20), &fontFormat, &fontBrush);
	
	fontBrush.SetColor(normalColor);
	g->DrawString((CResourceManager::GetString("FOOD")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,55), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("INDUSTRY")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,80), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("ENERGY")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,105), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("SECURITY")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,130), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("RESEARCH")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,155), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("TITAN")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,180), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("DEUTERIUM")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,205), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("DURANIUM")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,230), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("CRYSTAL")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,255), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("IRIDIUM")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,280), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("DERITIUM")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,305), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("MORAL")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,330), &fontFormat, &fontBrush);
	g->DrawString((CResourceManager::GetString("CREDITS")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(775,355), &fontFormat, &fontBrush);
	
	fontFormat.SetAlignment(StringAlignmentCenter);
	// Produktion anzeigen
	RectF rect(870,55,60,25);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetFoodProd());
	if (atoi(s) < 0)
	{
		fontBrush.SetColor(Color::Red);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
		fontBrush.SetColor(normalColor);
	}
	else
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	
	rect = RectF(870,80,60,25);
	if (m_iWhichSubMenu == 0)		// Wenn wir im normalen Baumen� sind
	{
		s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd());
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	}
	else if (m_iWhichSubMenu == 1)	// Wenn wir im Werftuntermen� sind
	{
		s.Format("%i",(int)(pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() * 
			pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipYardEfficiency() / 100));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	}
	else							// Wenn wir im Kasernenuntermen� sind
	{
		s.Format("%i",(short)(pDoc->GetSystem(p.x,p.y).GetProduction()->GetIndustryProd() * 
			pDoc->GetSystem(p.x,p.y).GetProduction()->GetBarrackEfficiency() / 100));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	}

	rect = RectF(870,105,60,25);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetEnergyProd());
	if (atoi(s) < 0)
	{
		fontBrush.SetColor(Color::Red);
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
		fontBrush.SetColor(normalColor);
	}
	else
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	
	rect.Y += 25;
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd());
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd());
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	for (int res = TITAN; res <= DERITIUM; res++)
	{
		rect.Y += 25;
		s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetResourceProd(res));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	}
	// Moralproduktion
	rect.Y += 25;
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetMoralProd());
	if(pDoc->GetSystem(p.x,p.y).GetProduction()->GetMoralProd()<0) fontBrush.SetColor(Color(255,0,0));
	else if(pDoc->GetSystem(p.x,p.y).GetProduction()->GetMoralProd()>0) fontBrush.SetColor(Color(0,255,0));
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	fontBrush.SetColor(normalColor);

	rect.Y += 25;
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetProduction()->GetCreditsProd());
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
		
	// ab hier Lager anzeigen
	rect = RectF(950,55,100,25);
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetFoodStore());
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	rect.Y += 25;
	if (m_iWhichSubMenu == 1)		// Wenn wir im Werftuntermen� sind
	{
		s.Format("%i%%",pDoc->GetSystem(p.x,p.y).GetProduction()->GetShipYardEfficiency());
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	}
	else if (m_iWhichSubMenu == 2)	// Wenn wir im Kasernenuntermen� sind
	{
		s.Format("%i%%",pDoc->GetSystem(p.x,p.y).GetProduction()->GetBarrackEfficiency());
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	}
	
	rect = RectF(950,155,100,25);
	// Zus�tzliche Ressourcen aus den Startsystemen von Ressourcenrouten ermitteln
	ULONG resFromRoutes[DERITIUM + 1] = {0};
	ULONG nResInDistSys[DERITIUM + 1]	= {0};
	for (int j = 0; j < pMajor->GetEmpire()->GetSystemList()->GetSize(); j++)
		if (pMajor->GetEmpire()->GetSystemList()->GetAt(j).ko != p)
		{
			CPoint ko = pMajor->GetEmpire()->GetSystemList()->GetAt(j).ko;
			// Wenn unser System blockiert wird so gelten die Ressourcenrouten nicht
			if (pDoc->m_System[ko.x][ko.y].GetBlockade() > NULL)
				continue;
			for (int i = 0; i < pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetSize(); i++)
				if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(i).GetKO() == p)
				{
					// Wenn das Startsystem blockiert wird, so kann die Ressourcenroute ebenfalls nicht benutzt werden
					if (pDoc->m_System[pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(i).GetKO().x][pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(i).GetKO().y].GetBlockade() > NULL)
						continue;
					BYTE res = pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(i).GetResource();
					resFromRoutes[res] += pDoc->m_System[ko.x][ko.y].GetResourceStore(res);
				}
			// gilt nicht bei blockierten Systemen
			if (pDoc->GetSystem(p).GetBlockade() == NULL)
				for (int res = TITAN; res <= DERITIUM; res++)
					if (pDoc->GetSystem(ko).GetProduction()->GetResourceDistributor(res))
						nResInDistSys[res] = pDoc->GetSystem(ko).GetResourceStore(res);
		}
	
	for (int res = TITAN; res <= DERITIUM; res++)
	{
		rect.Y += 25;
		if (nResInDistSys[res] > resFromRoutes[res] + pDoc->GetSystem(p).GetResourceStore(res))
			s.Format("[%i]", nResInDistSys[res]);
		else if (resFromRoutes[res] > 0)
			s.Format("(%i)",pDoc->GetSystem(p.x,p.y).GetResourceStore(res) + resFromRoutes[res]);
		else
			s.Format("%i",pDoc->GetSystem(p.x,p.y).GetResourceStore(res));

		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	}

	rect.Y += 25;
	s.Format("%i",pDoc->GetSystem(p.x,p.y).GetMoral());
	if (pDoc->GetSystem(p.x,p.y).GetMoral() > 174) fontBrush.SetColor(Color(0,250,0)); //Fanatic
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 154) fontBrush.SetColor(Color(20,150,20)); //Loyal
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 130) fontBrush.SetColor(Color(20,150,100)); //Pleased
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 99)  fontBrush.SetColor(Color(150,150,200)); //Satisfied
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 75)  fontBrush.SetColor(Color(160,160,160)); //Apathetic
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 49)  fontBrush.SetColor(Color(200,100,50)); //Angry
	else if (pDoc->GetSystem(p.x,p.y).GetMoral() > 29)  fontBrush.SetColor(Color(210,80,50)); //Furious
	else fontBrush.SetColor(Color(255,0,0));//Rebellious

	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), rect, &fontFormat, &fontBrush);
	
	// kleine Bilder von den Rohstoffen zeichnen
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\foodSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 55, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\industrySmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 80, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\energySmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 105, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\securitySmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 130, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\researchSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 155, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\titanSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 180, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\deuteriumSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 205, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\duraniumSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 230, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\crystalSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 255, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\iridiumSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 280, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\Deritium.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 305, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\moralSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 330, 20, 16);
	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\creditsSmall.bop");
	if (graphic)
		g->DrawImage(graphic, 740, 355, 20, 16);
}

// Funktion berechnet und zeichnet im "rect" die Produktion eins Geb�udes, also alles was es macht
void CSystemMenuView::DrawBuildingProduction(Graphics* g)
{
	if (m_iClickedOn >= m_vBuildlist.GetSize())
		return;

	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	RectF r(BuildingDescription.left, BuildingDescription.top + 5, BuildingDescription.right - BuildingDescription.left, 25);
	CString s;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	// Schriftfarbe w�hlen
	Gdiplus::Color normalColor;
	CFontLoader::GetGDIFontColor(pMajor, 3, normalColor);

	Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
		
	Color penColor;
	penColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkPenColor);
	SolidBrush fontBrush(penColor);

	StringFormat fontFormat;
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
		
	int RunningNumber = abs(m_vBuildlist[m_iClickedOn]);
	
	// Oben im Beschreibungsrechteck den Namen des Projektes hinschreiben
	// Ist es ein Geb�ude oder ein Upgrade eines Geb�udes, welches aber keine Arbeiter ben�tigt
	if (m_vBuildlist[m_iClickedOn] > 0 || pDoc->GetBuildingInfo(RunningNumber).GetWorker() == FALSE)
	{
		CBuildingInfo* b = &pDoc->GetBuildingInfo(RunningNumber);
		
		s.Format("%s",pDoc->GetBuildingName(RunningNumber));
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
		r.Y += 30;
		fontBrush.SetColor(markColor);
		g->DrawString((CResourceManager::GetString("PRODUCTION")+":").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
		r.Y += 22;
		fontBrush.SetColor(normalColor);
	
		if (b->GetFoodProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("FOOD"), b->GetFoodProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetIPProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("INDUSTRY"), b->GetIPProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetEnergyProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("ENERGY"), b->GetEnergyProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetSPProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SECURITY"), b->GetSPProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetFPProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("RESEARCH"), b->GetFPProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetTitanProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("TITAN"), b->GetTitanProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDeuteriumProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("DEUTERIUM"), b->GetDeuteriumProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDuraniumProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("DURANIUM"), b->GetDuraniumProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetCrystalProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("CRYSTAL"), b->GetCrystalProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetIridiumProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("IRIDIUM"), b->GetIridiumProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDeritiumProd() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("DERITIUM"), b->GetDeritiumProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetCredits() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("CREDITS"), b->GetCredits());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetMoralProd() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("MORAL"), b->GetMoralProd());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetMoralProdEmpire() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("MORAL_EMPIREWIDE"), b->GetMoralProdEmpire());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		
		// Ab hier die Boni
		if (b->GetFoodBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("FOOD_BONUS"), b->GetFoodBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetIndustryBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("INDUSTRY_BONUS"), b->GetIndustryBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetEnergyBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("ENERGY_BONUS"), b->GetEnergyBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetSecurityBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SECURITY_BONUS"), b->GetSecurityBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResearchBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("RESEARCH_BONUS"), b->GetResearchBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetTitanBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("TITAN_BONUS"), b->GetTitanBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDeuteriumBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("DEUTERIUM_BONUS"), b->GetDeuteriumBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDuraniumBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("DURANIUM_BONUS"), b->GetDuraniumBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetCrystalBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("CRYSTAL_BONUS"), b->GetCrystalBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetIridiumBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("IRIDIUM_BONUS"), b->GetIridiumBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetDeritiumBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("DERITIUM_BONUS"), b->GetDeritiumBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetAllRessourcesBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("BONUS_TO_ALL_RES"), b->GetAllRessourcesBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetCreditsBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("CREDITS_BONUS"), b->GetCreditsBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		// Wenn alle Forschungsboni belegt sind, sprich Bonus auf alles
		if (b->GetBioTechBoni() != 0 && b->GetBioTechBoni() == b->GetEnergyTechBoni() && b->GetBioTechBoni() == b->GetCompTechBoni()
			&& b->GetBioTechBoni() == b->GetPropulsionTechBoni() && b->GetBioTechBoni() == b->GetConstructionTechBoni()
			&& b->GetBioTechBoni() == b->GetWeaponTechBoni())
		{
			s.Format("%s: %i%%",CResourceManager::GetString("TECHNIC_BONUS"), b->GetBioTechBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		else // bei einzelnen Gebieten
		{
			if (b->GetBioTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("BIOTECH_BONUS"), b->GetBioTechBoni());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			if (b->GetEnergyTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("ENERGYTECH_BONUS"), b->GetEnergyTechBoni());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			if (b->GetCompTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("COMPUTERTECH_BONUS"), b->GetCompTechBoni());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			if (b->GetPropulsionTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("PROPULSIONTECH_BONUS"), b->GetPropulsionTechBoni());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			if (b->GetConstructionTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("CONSTRUCTIONTECH_BONUS"), b->GetConstructionTechBoni());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			if (b->GetWeaponTechBoni() > 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("WEAPONTECH_BONUS"), b->GetWeaponTechBoni());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
		}
		// Wenn alle Geheimdienstdinger belegt sind -> Geheimdienst auf alles
		if (b->GetInnerSecurityBoni() != 0 && b->GetInnerSecurityBoni() == b->GetEconomySpyBoni() 
			&& b->GetInnerSecurityBoni() ==	b->GetEconomySabotageBoni() && b->GetInnerSecurityBoni() == b->GetResearchSpyBoni()
			&& b->GetInnerSecurityBoni() == b->GetResearchSabotageBoni() && b->GetInnerSecurityBoni() == b->GetMilitarySpyBoni()
			&& b->GetInnerSecurityBoni() == b->GetMilitarySabotageBoni())
		{
			s.Format("%s: %i%%",CResourceManager::GetString("COMPLETE_SECURITY_BONUS"), b->GetInnerSecurityBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		else
		{
			if (b->GetEconomySpyBoni() != 0 && b->GetEconomySpyBoni() == b->GetResearchSpyBoni()
				&& b->GetEconomySpyBoni() == b->GetMilitarySpyBoni())
			{
				s.Format("%s: %i%%",CResourceManager::GetString("SPY_BONUS"), b->GetEconomySpyBoni());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			else
			{
				if (b->GetEconomySpyBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("ECONOMY_SPY_BONUS"), b->GetEconomySpyBoni());
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
				if (b->GetResearchSpyBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("RESEARCH_SPY_BONUS"), b->GetResearchSpyBoni());
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
				if (b->GetMilitarySpyBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("MILITARY_SPY_BONUS"), b->GetMilitarySpyBoni());
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
			}
			if (b->GetEconomySabotageBoni() != 0 && b->GetEconomySabotageBoni() == b->GetResearchSabotageBoni()
				&& b->GetEconomySabotageBoni() == b->GetMilitarySabotageBoni())
			{
				s.Format("%s: %i%%",CResourceManager::GetString("SABOTAGE_BONUS"), b->GetEconomySabotageBoni());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			else
			{
				if (b->GetEconomySabotageBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("ECONOMY_SABOTAGE_BONUS"), b->GetEconomySabotageBoni());
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
				if (b->GetResearchSabotageBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("RESEARCH_SABOTAGE_BONUS"), b->GetResearchSabotageBoni());
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
				if (b->GetMilitarySabotageBoni() != 0)
				{
					s.Format("%s: %i%%",CResourceManager::GetString("MILITARY_SABOTAGE_BONUS"), b->GetMilitarySabotageBoni());
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
					r.Y += 22;
				}
			}
			if (b->GetInnerSecurityBoni() != 0)
			{
				s.Format("%s: %i%%",CResourceManager::GetString("INNER_SECURITY_BONUS"), b->GetInnerSecurityBoni());
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
		}
		// Ab hier zus�tzliche Eigenschaften des Geb�udes
		if (b->GetShipYard() == TRUE)
		{
			s = CResourceManager::GetString("SHIPYARD");
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
			switch (b->GetMaxBuildableShipSize())
			{
			case 0: s = CResourceManager::GetString("SMALL"); break;
			case 1: s = CResourceManager::GetString("MIDDLE"); break;
			case 2: s = CResourceManager::GetString("BIG"); break;
			case 3: s = CResourceManager::GetString("HUGE"); break;
			default: s = CResourceManager::GetString("HUGE");
			}
			CString ss;
			ss.Format("%s: %s",CResourceManager::GetString("MAX_SHIPSIZE"), s);
			g->DrawString(ss.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipYardSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SHIPYARD_EFFICIENCY"), b->GetShipYardSpeed());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetBarrack() == TRUE)
		{
			s = CResourceManager::GetString("BARRACK");
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetBarrackSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("BARRACK_EFFICIENCY"), b->GetBarrackSpeed());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShieldPower() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SHIELDPOWER"), b->GetShieldPower());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShieldPowerBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SHIELDPOWER_BONUS"), b->GetShieldPowerBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipDefend() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SHIPDEFEND"), b->GetShipDefend());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipDefendBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SHIPDEFEND_BONUS"), b->GetShipDefendBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetGroundDefend() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("GROUNDDEFEND"), b->GetGroundDefend());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetGroundDefendBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("GROUNDDEFEND_BONUS"), b->GetGroundDefendBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetScanPower() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SCANPOWER"), b->GetScanPower());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetScanPowerBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SCANPOWER_BONUS"), b->GetScanPowerBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetScanRange() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SCANRANGE"), b->GetScanRange());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetScanRangeBoni() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SCANRANGE_BONUS"), b->GetScanRangeBoni());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipTraining() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("SHIPTRAINING"), b->GetShipTraining());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetTroopTraining() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("TROOPTRAINING"), b->GetTroopTraining());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResistance() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("RESISTANCE"), b->GetResistance());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetAddedTradeRoutes() != 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("ADDED_TRADEROUTES"), b->GetAddedTradeRoutes());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetIncomeOnTradeRoutes() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("INCOME_ON_TRADEROUTES"), b->GetIncomeOnTradeRoutes());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipRecycling() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SHIPRECYCLING"), b->GetShipRecycling());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetBuildingBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("BUILDING_BUILDSPEED"), b->GetBuildingBuildSpeed());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetUpdateBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("UPGRADE_BUILDSPEED"), b->GetUpdateBuildSpeed());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetShipBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("SHIP_BUILDSPEED"), b->GetShipBuildSpeed());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetTroopBuildSpeed() != 0)
		{
			s.Format("%s: %i%%",CResourceManager::GetString("TROOP_BUILDSPEED"), b->GetTroopBuildSpeed());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(TITAN))
		{
			s.Format("%s - %s\n", CResourceManager::GetString("RESOURCE_DISTRIBUTOR"), CResourceManager::GetString("TITAN"));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(DEUTERIUM))
		{
			s.Format("%s - %s\n", CResourceManager::GetString("RESOURCE_DISTRIBUTOR"), CResourceManager::GetString("DEUTERIUM"));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(DURANIUM))
		{
			s.Format("%s - %s\n", CResourceManager::GetString("RESOURCE_DISTRIBUTOR"), CResourceManager::GetString("DURANIUM"));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(CRYSTAL))
		{
			s.Format("%s - %s\n", CResourceManager::GetString("RESOURCE_DISTRIBUTOR"), CResourceManager::GetString("CRYSTAL"));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(IRIDIUM))
		{
			s.Format("%s - %s\n", CResourceManager::GetString("RESOURCE_DISTRIBUTOR"), CResourceManager::GetString("IRIDIUM"));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetResourceDistributor(DERITIUM))
		{
			s.Format("%s - %s\n", CResourceManager::GetString("RESOURCE_DISTRIBUTOR"), CResourceManager::GetString("DERITIUM"));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		
		// Ab hier die Vorraussetzungen
		fontBrush.SetColor(markColor);
		s = CResourceManager::GetString("PREREQUISITES")+":";
		g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
		r.Y += 22;
		fontBrush.SetColor(normalColor);
		// ben�tigte Systeme
		if (b->GetNeededSystems() != 0)
		{
			s.Format("%s: %i\n", CResourceManager::GetString("NEEDED_SYSTEMS"), b->GetNeededSystems());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}		
		// max X mal von ID pro Imperium
		if (b->GetMaxInEmpire() > 0)
		{
			if (b->GetMaxInEmpire() == 1)
			{
				s = CResourceManager::GetString("ONCE_PER_EMPIRE");
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			else
			{
				s.Format("%d",b->GetMaxInEmpire());
				g->DrawString(CResourceManager::GetString("MAX_PER_EMPIRE",FALSE,s).AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
		}
		// max X mal von ID pro System
		if (b->GetMaxInSystem().Number > 0 && b->GetMaxInSystem().RunningNumber == b->GetRunningNumber())
		{
			if (b->GetMaxInSystem().Number == 1)
			{
				g->DrawString(CResourceManager::GetString("ONCE_PER_SYSTEM").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
			else
			{
				s.Format("%d",b->GetMaxInSystem().Number);
				g->DrawString(CResourceManager::GetString("MAX_PER_SYSTEM",FALSE,s).AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				r.Y += 22;
			}
		}
		else if (b->GetMaxInSystem().Number > 0 && b->GetMaxInSystem().RunningNumber != b->GetRunningNumber())
		{
			s.Format("%d",b->GetMaxInSystem().Number);
			g->DrawString(CResourceManager::GetString("MAX_ID_PER_SYSTEM",FALSE,s,pDoc->GetBuildingName(b->GetMaxInSystem().RunningNumber)).AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		// min X mal von ID pro System
		if (b->GetMinInSystem().Number > 0)
		{
			s.Format("%d",b->GetMinInSystem().Number);
			g->DrawString(CResourceManager::GetString("MIN_PER_SYSTEM",FALSE,s,pDoc->GetBuildingName(b->GetMinInSystem().RunningNumber)).AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetOnlyHomePlanet() == TRUE)
		{
			g->DrawString(CResourceManager::GetString("ONLY_HOMEPLANET").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetOnlyOwnColony() == TRUE)
		{
			g->DrawString(CResourceManager::GetString("ONLY_OWN_COLONY").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetOnlyTakenSystem() == TRUE)
		{
			g->DrawString(CResourceManager::GetString("ONLY_TAKEN_SYSTEM").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetMinHabitants() > 0)
		{
			s.Format("%d",b->GetMinHabitants());
			g->DrawString(CResourceManager::GetString("NEED_MIN_HABITANTS",FALSE,s).AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetNeededEnergy() > 0)
		{
			s.Format("%s: %i",CResourceManager::GetString("ENERGY"),b->GetNeededEnergy());
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}
		if (b->GetWorker() == TRUE)
		{
			g->DrawString(CResourceManager::GetString("NEED_WORKER").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 22;
		}		
	}
	
	// Wenn es sich um ein Upgrade handelt:
	else if (m_vBuildlist[m_iClickedOn] < 0)	
	{
		CPoint p = pDoc->GetKO();
		// aktuelles Geb�ude, also Geb�ude welches geupdated werden soll
		CBuildingInfo* b1 = &pDoc->GetBuildingInfo(pDoc->BuildingInfo.GetAt(RunningNumber - 1).GetPredecessorID());
		if (b1->GetWorker())
		{
			// Nachfolger des aktuellen Geb�udes
			CBuildingInfo* b2 = &pDoc->GetBuildingInfo(RunningNumber);
			fontBrush.SetColor(penColor);
			fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
			fontFormat.SetAlignment(StringAlignmentNear);
			r.Height = 50;
			s = CResourceManager::GetString("UPGRADE_FROM_TO",FALSE,pDoc->GetBuildingName(pDoc->BuildingInfo.GetAt(RunningNumber - 1).GetPredecessorID()),pDoc->GetBuildingName(RunningNumber));
			g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 75;
			r.Height = 25;
			
			fontBrush.SetColor(markColor);
			fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
			fontFormat.SetAlignment(StringAlignmentCenter);
		
			g->DrawString(CResourceManager::GetString("RELATIVE_PROFIT").AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			r.Y += 25;
			
			fontBrush.SetColor(normalColor);
			if (b1->GetFoodProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(0,0,NULL);
				s.Format("%i %s",b2->GetFoodProd()*number-b1->GetFoodProd()*number, CResourceManager::GetString("FOOD"));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetIPProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(1,0,NULL);
				s.Format("%i %s",b2->GetIPProd()*number-b1->GetIPProd()*number,CResourceManager::GetString("INDUSTRY"));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetEnergyProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(2,0,NULL);
				s.Format("%i %s",b2->GetEnergyProd()*number-b1->GetEnergyProd()*number,CResourceManager::GetString("ENERGY"));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetSPProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(3,0,NULL);
				s.Format("%i %s",b2->GetSPProd()*number-b1->GetSPProd()*number,CResourceManager::GetString("SECURITY"));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetFPProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(4,0,NULL);
				s.Format("%i %s",b2->GetFPProd()*number-b1->GetFPProd()*number,CResourceManager::GetString("RESEARCH"));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetTitanProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(5,0,NULL);
				s.Format("%i %s",b2->GetTitanProd()*number-b1->GetTitanProd()*number,CResourceManager::GetString("TITAN"));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetDeuteriumProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(6,0,NULL);
				s.Format("%i %s",b2->GetDeuteriumProd()*number-b1->GetDeuteriumProd()*number,CResourceManager::GetString("DEUTERIUM"));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetDuraniumProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(7,0,NULL);
				s.Format("%i %s",b2->GetDuraniumProd()*number-b1->GetDuraniumProd()*number,CResourceManager::GetString("DURANIUM"));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetCrystalProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(8,0,NULL);
				s.Format("%i %s",b2->GetCrystalProd()*number-b1->GetCrystalProd()*number,CResourceManager::GetString("CRYSTAL"));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			}
			else if (b1->GetIridiumProd() > 0)
			{
				short number = pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(9,0,NULL);
				s.Format("%i %s",b2->GetIridiumProd()*number-b1->GetIridiumProd()*number,CResourceManager::GetString("IRIDIUM"));
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
				
				fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
				r.Y += 22;
				r.Height = 50;
				s.Format("%s: %i",CResourceManager::GetString("NUMBER_OF_UPGRADEABLE_BUILDINGS"), number);
				g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), r, &fontFormat, &fontBrush);
			}
		}
	}
}

void CSystemMenuView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	CalcLogicalPoint(point);

	// Wenn wir uns in der Systemansicht befinden
	int temp = m_bySubMenu;
	if (ButtonReactOnLeftClick(point, &m_BuildMenueMainButtons, temp, FALSE))
	{
		m_bySubMenu = temp;
		if (m_bySubMenu == 0)
		{
			m_bClickedOnBuyButton = FALSE;
			m_bClickedOnDeleteButton = FALSE;
		}
		Invalidate(FALSE);
	}
	// Ins Baumen� (Bauliste usw.)
	if (m_bySubMenu == 0)
	{
		// haben wir hier auf einen Button geklickt um in ein anderes Untermen� zu gelangen (Geb�ude, Schiffe oder Truppen)
		if (BuildingListButton.PtInRect(point))
		{
			m_iClickedOn = 0;
			m_byStartList = 0;
			m_iWhichSubMenu = 0;
			Invalidate(FALSE);
			return;
		}
		else if (ShipyardListButton.PtInRect(point))
		{
			// Wenn man keine Schiffe zur Auswahl hat dann wird wieder auf das normale Geb�udebaumen� umgeschaltet
			if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetBuildableShips()->GetSize() == 0)
				return;
			m_iClickedOn = 0;
			m_byStartList = 0;
			m_iWhichSubMenu = 1;
			Invalidate(FALSE);
			return;
		}
		else if (TroopListButton.PtInRect(point))
		{
			// Wenn man keine Schiffe zur Auswahl hat dann wird wieder auf das normale Geb�udebaumen� umgeschaltet
			if (pDoc->m_System[pDoc->GetKO().x][pDoc->GetKO().y].GetBuildableTroops()->GetSize() == 0)
				return;
			m_iClickedOn = 0;
			m_byStartList = 0;
			m_iWhichSubMenu = 2;
			Invalidate(FALSE);
			return;
		}

		for (int i = m_byStartList; i < m_vBuildlist.GetSize(); i++)
		{
			if (i > m_byEndList)
				break;

			CRect rect(380, 150 + (i - m_byStartList) * 25, 670, 150 + (i - m_byStartList) * 25 + 25);

			if (rect.PtInRect(point))
			{
				m_iClickedOn = i;
				m_bClickedOnBuyButton = FALSE;
				m_bClickedOnDeleteButton = FALSE;
				Invalidate(FALSE);
				return;
			}
		}

		CPoint p = pDoc->GetKO();
		// �berpr�fen ob wir auf den KaufenButton gedr�ckt haben und wir noch nix gekauft haben
		// dies geht nicht bei NeverReady wie z.B. Kriegsrecht Auftr�gen
		int nFirstAssemblyListEntry = pDoc->GetSystem(p).GetAssemblyList()->GetAssemblyListEntry(0);
		if (BuyButton.PtInRect(point) && pDoc->GetSystem(p).GetAssemblyList()->GetWasBuildingBought() == FALSE
			&& nFirstAssemblyListEntry != 0 && m_bClickedOnDeleteButton == FALSE)
		{
			// Bei Geb�uden nur wenn es nicht ein Auftrag mit NeverReady (z.B. Kriegsrecht) ist)
			if ((nFirstAssemblyListEntry < 0)
				||					
				(nFirstAssemblyListEntry > 0 && nFirstAssemblyListEntry < 10000 && pDoc->GetBuildingInfo(nFirstAssemblyListEntry).GetNeverReady() == FALSE)
				||
				// Bei Schiffen nur, wenn eine Werft noch aktiv ist
				(nFirstAssemblyListEntry >= 10000 && nFirstAssemblyListEntry < 20000 && pDoc->GetSystem(p).GetProduction()->GetShipYard())
				||
				// Bei Truppen nur mit aktiver Kaseren
				(nFirstAssemblyListEntry >= 20000 && pDoc->GetSystem(p).GetProduction()->GetBarrack()))
			{
				pDoc->GetSystem(p).GetAssemblyList()->CalculateBuildCosts(pMajor->GetTrade()->GetRessourcePriceAtRoundStart());
				m_bClickedOnBuyButton = TRUE;				
				Invalidate();
				return;
			}
		}

		// �berpr�fen ob wir auf den Okaybutton gedr�ckt haben um das aktuelle Projekt zu kaufen
		if (OkayButton.PtInRect(point) && m_bClickedOnBuyButton == TRUE)
		{
			int costs = pDoc->GetSystem(p).GetAssemblyList()->BuyBuilding(pMajor->GetEmpire()->GetCredits());
			if (costs != 0)
			{
				OkayButton.SetRect(0,0,0,0);
				CancelButton.SetRect(0,0,0,0);
				pDoc->GetSystem(p).GetAssemblyList()->SetWasBuildingBought(TRUE);
				pMajor->GetEmpire()->SetCredits(-costs);
				// Die Preise an der B�rse anpassen, da wir ja bestimmte Mengen Ressourcen gekauft haben
				// Achtung, hier flag == 1 setzen bei Aufruf der Funktion BuyRessource!!!!
				pMajor->GetTrade()->BuyRessource(TITAN,pDoc->GetSystem(p).GetAssemblyList()->GetNeededTitanInAssemblyList(0),p,pMajor->GetEmpire()->GetCredits(),1);
				pMajor->GetTrade()->BuyRessource(DEUTERIUM,pDoc->GetSystem(p).GetAssemblyList()->GetNeededDeuteriumInAssemblyList(0),p,pMajor->GetEmpire()->GetCredits(),1);
				pMajor->GetTrade()->BuyRessource(DURANIUM,pDoc->GetSystem(p).GetAssemblyList()->GetNeededDuraniumInAssemblyList(0),p,pMajor->GetEmpire()->GetCredits(),1);
				pMajor->GetTrade()->BuyRessource(CRYSTAL,pDoc->GetSystem(p).GetAssemblyList()->GetNeededCrystalInAssemblyList(0),p,pMajor->GetEmpire()->GetCredits(),1);
				pMajor->GetTrade()->BuyRessource(IRIDIUM,pDoc->GetSystem(p).GetAssemblyList()->GetNeededIridiumInAssemblyList(0),p,pMajor->GetEmpire()->GetCredits(),1);
							
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				m_bClickedOnBuyButton = FALSE;
				Invalidate();
				return;
			}
		}
		// �berpr�fen ob wir auf den Cancelbutton gedr�ckt haben um den Kauf abzubrechen
		if (CancelButton.PtInRect(point))
		{
			OkayButton.SetRect(0,0,0,0);
			CancelButton.SetRect(0,0,0,0);			
			m_bClickedOnBuyButton = FALSE;
			m_bClickedOnDeleteButton = FALSE;
			Invalidate();
			return;
		}

		// �berpr�fen ob wir auf den Bau_abbrechen Button gedr�ckt haben
		if (DeleteButton.PtInRect(point) && pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetAssemblyListEntry(0) != 0 && m_bClickedOnBuyButton == FALSE)
		{
			m_bClickedOnDeleteButton = TRUE;			
			Invalidate();
			return;
		}
		// �berpr�fen ob wir auf den Okaybutton gedr�ckt haben um das aktuelle Projekt abzubrechen
		if (OkayButton.PtInRect(point) && m_bClickedOnDeleteButton == TRUE)
		{
			// bekommen bei Abbruch die Ressourcen bzw. Teile der Ressourcen wieder
			for (int j = TITAN; j <= DERITIUM; j++)
			{
				// bestanden Ressourcenrouten, so kann es sein, dass deren Startsysteme einen Anteil oder auch
				// alles zur�ckbekommen
				long getBackRes = pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededResourceInAssemblyList(0, j);
				for (int k = 0; k < pMajor->GetEmpire()->GetSystemList()->GetSize(); k++)
					if (pMajor->GetEmpire()->GetSystemList()->GetAt(k).ko != p)
					{
						CPoint ko = pMajor->GetEmpire()->GetSystemList()->GetAt(k).ko;
						for (int l = 0; l < pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetSize(); l++)
							if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetKO() == p)
								if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetKO() == p)
									if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetResource() == j)
										if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetPercent() > 0)
										{
											// sind wir soweit, dann geht ein prozentualer Anteil zur�ck in das
											// Startsystem der Ressourcenroute
											int back = pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededResourceInAssemblyList(0, j)
												* pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetPercent() / 100;
											ASSERT(back >= 0);
											pDoc->m_System[ko.x][ko.y].SetResourceStore(j, back);
											getBackRes -= back;
										}
					}
				pDoc->m_System[p.x][p.y].SetResourceStore(j, getBackRes);
			}
			// Wenn wir was gekauft hatten, dann bekommen wir die Kaufkosten zur�ck und die Preise an der B�rse
			// regulieren sich wieder auf den Kurs, bevor wir gekauft haben
			if (pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetWasBuildingBought() == TRUE)
			{
				pMajor->GetEmpire()->SetCredits(pDoc->GetSystem(p.x,p.y).GetAssemblyList()->GetBuildCosts());
				// Die Preise an der B�rse anpassen, da wir ja bestimmte Mengen Ressourcen gekauft haben
				// Achtung, hier flag == 1 setzen bei Aufruf der Funktion BuyRessource!!!!
				for (int j = TITAN; j <= IRIDIUM; j++)
					pMajor->GetTrade()->SellRessource(j, pDoc->m_System[p.x][p.y].GetAssemblyList()->GetNeededResourceInAssemblyList(0, j), p, 1);
				pDoc->m_System[p.x][p.y].GetAssemblyList()->SetWasBuildingBought(FALSE);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
			}
			pDoc->m_System[p.x][p.y].GetAssemblyList()->ClearAssemblyList(p, pDoc->m_System);
			// Nach ClearAssemblyList m�ssen wir die Funktion CalculateVariables() aufrufen
			pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),
				pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
			
			int RunningNumber = abs(nFirstAssemblyListEntry);
			// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben. 
			// Den Check nur machen, wenn wir ein Update oder ein Geb�ude welches eine Maxanzahl voraussetzt
			// hinzuf�gen wollen
			if (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire() > 0)
			{
				// Wir m�ssen die GlobalBuilding Variable �ndern, weil sich mittlerweile ja solch ein Geb�ude
				// weniger in der Bauliste befindet. Nicht aber wenn es ein Upgrade ist
				if (nFirstAssemblyListEntry > 0)
					pDoc->m_GlobalBuildings.DeleteGlobalBuilding(pMajor->GetRaceID(), RunningNumber);
				pDoc->GetSystem(p).AssemblyListCheck(&pDoc->BuildingInfo, &pDoc->m_GlobalBuildings);
			}
			m_bClickedOnDeleteButton = FALSE;
			OkayButton.SetRect(0,0,0,0);
			CancelButton.SetRect(0,0,0,0);			
			Invalidate();
			return;
		}
		// �berpr�fen ob wir auf den Cancelbutton gedr�ckt haben um das aktuelle Projekt abbrechen wollen
		// -> gleiche Cancelfunktion wie oben bei Kaufen

		// �berpr�fen ob wir auf den Geb�udeinfo Button gedr�ckt haben um die Info zum Geb�ude anzuzeigen
		if (BuildingInfoButton.PtInRect(point))
		{
			m_bClickedOnBuildingInfoButton = TRUE;
			m_bClickedOnBuildingDescriptionButton = FALSE;
			CRect r = BuildingDescription;
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
			return;
		}
		// �berpr�fen ob wir auf den Geb�udebeschreibungs Button gedr�ckt haben um die Beschreibung zum Geb�ude anzuzeigen
		if (BuildingDescriptionButton.PtInRect(point))
		{
			m_bClickedOnBuildingInfoButton = FALSE;
			m_bClickedOnBuildingDescriptionButton = TRUE;
			CRect r = BuildingDescription;
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
			return;
		}
	}

	//////////////////////////////////////////////////////////	
	// Ins Arbeiterzuweisungsmen� (normale Geb�ude)
	if (m_bySubMenu == 1)
	{
		CPoint p = pDoc->GetKO();
		if (ChangeWorkersButton.PtInRect(point))
		{
			m_bySubMenu = 12;
			//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep2.wav", NULL, SND_FILENAME | SND_ASYNC);
			Invalidate();
			return;
		}
		
		int i = -1;
		if (ButtonReactOnLeftClick(point, &m_WorkerButtons, i, FALSE, TRUE))
		{
			// Wenn wir die Arbeiterzahl erh�hen wollen
			if (i >= 5)
			{
				// bei den Plusbutton m�ssen wir 5 abziehen, um auf die korrelierten Arbeiter zu kommen
				i -= 5;
				// Wenn wir noch freie Arbeiter haben
				if (pDoc->GetSystem(p.x,p.y).GetWorker(11) > 0 && pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i,0,NULL) > pDoc->GetSystem(p.x,p.y).GetWorker(i))
				{
					pDoc->m_System[p.x][p.y].SetWorker(i,0,0);	// FoodWorker inkrementieren
					// FP und SP aus dem System von den Gesamten FP des Imnperiums abziehen
					pMajor->GetEmpire()->AddFP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pMajor->GetEmpire()->AddSP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					// Variablen berechnen
					pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
					// FP�s und SP�s wieder draufrechnen
					pMajor->GetEmpire()->AddFP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pMajor->GetEmpire()->AddSP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
					Invalidate();
					return;
				}
			}
			// Wenn wir die Arbeiterzahl verringern m�chten
			else
			{
				// Wenn wir noch Arbeiter in dem bestimmten Geb�ude haben
				if (pDoc->GetSystem(p.x,p.y).GetWorker(i) > 0)
				{
					pDoc->m_System[p.x][p.y].SetWorker(i,0,1);	// FoodWorker dekrementieren
					// FP und SP aus dem System von den Gesamten FP des Imnperiums abziehen
					pMajor->GetEmpire()->AddFP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pMajor->GetEmpire()->AddSP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					// Variablen berechnen
					pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
					// FP�s und SP�s wieder draufrechnen
					pMajor->GetEmpire()->AddFP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pMajor->GetEmpire()->AddSP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
					Invalidate();
					return;
				}
			}
		}
		// Wenn wir die Arbeiterzahl erh�hen/verringern wollen und auf den Balken gedr�ckt haben
		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 200; j++)
				if (Timber[i][j].PtInRect(point))
				{
					// Wenn unsere Erh�hung gr��er der freien Arbeiter ist
					if (j >= (pDoc->GetSystem(p.x,p.y).GetWorker(11)+pDoc->GetSystem(p.x,p.y).GetWorker(i)))
					{
						// Dann setzen wir die Erh�hung auf den max. mgl. Wert
						j = (pDoc->GetSystem(p.x,p.y).GetWorker(11)+pDoc->GetSystem(p.x,p.y).GetWorker(i));
						j--;	// M�ssen wir machen
					}
					pDoc->m_System[p.x][p.y].SetWorker(i,j+1,2);
					// Wenn wir ziemlich weit ganz links geklickt haben, dann Arbeiter auf null setzen, werden hier nur um eins dekrementiert
					if (j == 0 && point.x < Timber[i][j].left+3)
						pDoc->m_System[p.x][p.y].SetWorker(i,0,1);
					// FP und SP aus dem System von den Gesamten FP des Imnperiums abziehen
					pMajor->GetEmpire()->AddFP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pMajor->GetEmpire()->AddSP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					// Variablen berechnen
					pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
					// FP�s und SP�s wieder draufrechnen
					pMajor->GetEmpire()->AddFP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pMajor->GetEmpire()->AddSP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
					Invalidate();
					return;
				}

	}
	// Ins Arbeiterzuweisungsmen� (Ressourcen Geb�ude)
	else
	{
		if (m_bySubMenu == 12)
		{
			CPoint p = pDoc->GetKO();	
			if (ChangeWorkersButton.PtInRect(point))
			{
				m_bySubMenu = 1;
				//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep2.wav", NULL, SND_FILENAME | SND_ASYNC);
				Invalidate();
			}
			int i = -1;
			if (ButtonReactOnLeftClick(point, &m_WorkerButtons, i, FALSE, TRUE))
			{
				// Wenn wir die Arbeiterzahl erh�hen wollen
				if (i >= 5)
				{
					// bei den Plusbutton m�ssen wir 5 abziehen, um auf die korrelierten Arbeiter zu kommen
					i -= 5;
					// Wenn wir noch freie Arbeiter haben
					if (pDoc->GetSystem(p.x,p.y).GetWorker(11) > 0 && pDoc->GetSystem(p.x,p.y).GetNumberOfWorkbuildings(i+5,0,NULL) > pDoc->GetSystem(p.x,p.y).GetWorker(i+5))
					{
						pDoc->m_System[p.x][p.y].SetWorker(i+5,0,0);	// FoodWorker inkrementieren
						pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
						//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
						Invalidate();
						return;
					}
				}
				// Wenn wir die Arbeiterzahl verringern m�chten
				else
				{
					// Wenn wir noch Arbeiter in dem bestimmten Geb�ude haben
					if (pDoc->GetSystem(p.x,p.y).GetWorker(i+5) > 0)
					{
						pDoc->m_System[p.x][p.y].SetWorker(i+5,0,1);	// FoodWorker dekrementieren
						pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
						//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
						Invalidate();
						return;
					}
				}
			}
			// Wenn wir die Arbeiterzahl erh�hen/verringern wollen und auf den Balken gedr�ckt haben
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 200; j++)
					if (Timber[i][j].PtInRect(point))
					{
						// Wenn unsere Erh�hung gr��er der freien Arbeiter ist
						if (j >= (pDoc->GetSystem(p.x,p.y).GetWorker(11)+pDoc->GetSystem(p.x,p.y).GetWorker(i+5)))
						{
							// Dann setzen wir die Erh�hung auf den max. mgl. Wert
							j = (pDoc->GetSystem(p.x,p.y).GetWorker(11)+pDoc->GetSystem(p.x,p.y).GetWorker(i+5));
							j--;	// M�ssen wir machen
						}
						pDoc->m_System[p.x][p.y].SetWorker(i+5,j+1,2);
						// Wenn wir ziemlich weit ganz links geklickt haben, dann Arbeiter auf null setzen, werden hier nur um eins dekrementiert
						if (j == 0 && point.x < Timber[i][j].left+3)
							pDoc->m_System[p.x][p.y].SetWorker(i+5,0,1);
						pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
						//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
						Invalidate();
						return;
					}
		}
	}

	// Ins Energiezuweisungsmen�
	if (m_bySubMenu == 2)
	{
		CPoint p = pDoc->GetKO();
		for (int i = m_iELPage * NOBIEL; i < m_EnergyList.GetSize(); i++)
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iELPage * NOBIEL + NOBIEL)
				if (m_EnergyList.GetAt(i).rect.PtInRect(point))
				{	
					const CBuildingInfo *buildingInfo = &pDoc->BuildingInfo.GetAt(pDoc->GetSystem(p.x, p.y).GetAllBuildings()->GetAt(m_EnergyList.GetAt(i).index).GetRunningNumber() - 1);
					if (m_EnergyList.GetAt(i).status == 0)
					{
						if (pDoc->GetSystem(p.x,p.y).GetProduction()->GetEnergyProd() >= buildingInfo->GetNeededEnergy())
							pDoc->m_System[p.x][p.y].SetIsBuildingOnline(m_EnergyList.GetAt(i).index, 1);
					}
					else
						pDoc->m_System[p.x][p.y].SetIsBuildingOnline(m_EnergyList.GetAt(i).index, 0);
					// FP und SP aus dem System von den Gesamten FP des Imnperiums abziehen
					pMajor->GetEmpire()->AddFP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pMajor->GetEmpire()->AddSP(-(pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					// Variablen berechnen
					pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
					// FP�s und SP�s wieder draufrechnen
					pMajor->GetEmpire()->AddFP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetResearchProd()));
					pMajor->GetEmpire()->AddSP((pDoc->GetSystem(p.x,p.y).GetProduction()->GetSecurityProd()));
					// Wenn es eine Werft war, die wir an bzw. aus geschaltet haben, dann nochmal schauen ob ich auch
					// noch alle Schiffe bauen kann. Denn wenn die aus ist, dann kann ich keine mehr bauen
					if (buildingInfo->GetShipYard())
						pDoc->m_System[p.x][p.y].CalculateBuildableShips(pDoc, p);
					//PlaySound(*((CBotf2App*)AfxGetApp())->GetPath() + "Sounds\\ComputerBeep1.wav", NULL, SND_FILENAME | SND_ASYNC);
					Invalidate(FALSE);
					break;
				}
		// Wenn ich in dem Men� vor oder zur�ck bl�ttern kann/will
		if (CRect(1011,190,1074,242).PtInRect(point) && m_iELPage * NOBIEL + NOBIEL < m_EnergyList.GetSize())
		{
			m_iELPage++;
			Invalidate(FALSE);
		}
		else if (CRect(1011,490,1074,542).PtInRect(point) && m_iELPage > 0)
		{
			m_iELPage--;
			Invalidate(FALSE);				
		}
	}

	// Ins Geb�ude�bersichtsmen�
	else if (m_bySubMenu == 3)
	{
		// Wenn ich Geb�ude abrei�en will, mit links auf die Schaltfl�che dr�cken
		CPoint p = pDoc->GetKO();
		for (int i = m_iBOPage * NOBIOL; i < m_BuildingOverview.GetSize(); i++)
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iBOPage * NOBIOL + NOBIOL)
				if (m_BuildingOverview.GetAt(i).rect.PtInRect(point))
				{
					USHORT ru = m_BuildingOverview.GetAt(i).runningNumber;
					pDoc->m_System[p.x][p.y].SetBuildingDestroy(ru,TRUE);
					CRect r = m_BuildingOverview.GetAt(i).rect;
					CalcDeviceRect(r);
					InvalidateRect(r, FALSE);
					break;;
				}
		// Wenn ich in dem Men� vor oder zur�ck bl�ttern kann/will
		if (CRect(1011,190,1074,242).PtInRect(point) && m_iBOPage * NOBIOL + NOBIOL < m_BuildingOverview.GetSize())
		{
			m_iBOPage++;
			Invalidate(FALSE);
		}
		else if (CRect(1011,490,1074,542).PtInRect(point) && m_iBOPage > 0)
		{
			m_iBOPage--;
			Invalidate(FALSE);				
		}
	}
	// Ins Systemhandelsmen�
	else if (m_bySubMenu == 4)
	{
		// Button zum Handelsrouten anlegen
		if (CRect(60,600,180,630).PtInRect(point))
		{
			CGalaxyMenuView::IsDrawTradeRoute(TRUE);
			CGalaxyMenuView::GetDrawnTradeRoute()->GenerateTradeRoute(pDoc->GetKO());
			pDoc->GetMainFrame()->SelectMainView(GALAXY_VIEW, pMajor->GetRaceID());
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
		}
		// Button zum Anlegen einer Ressourcenroute
		else if (CRect(360,600,480,630).PtInRect(point))
		{
			CGalaxyMenuView::IsDrawResourceRoute(TRUE);
			CGalaxyMenuView::GetDrawnResourceRoute()->GenerateResourceRoute(pDoc->GetKO(), m_byResourceRouteRes);
			pDoc->GetMainFrame()->SelectMainView(GALAXY_VIEW, pMajor->GetRaceID());
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
		}
		// Button zum �ndern der Ressource einer Ressourcenroute
		else if (CRect(360,640,480,670).PtInRect(point))
		{
			m_byResourceRouteRes++;
			if (m_byResourceRouteRes == IRIDIUM + 1)
				m_byResourceRouteRes = TITAN;
			CRect r(360,640,480,670);
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
		}
		// Button um die Anzahl, die bei einem Klick verschoben wird zu ver�ndern
		else if (CRect(747,600,867,630).PtInRect(point))
		{
			m_iGlobalStoreageQuantity *= 10;
			if (m_iGlobalStoreageQuantity > 10000)
				m_iGlobalStoreageQuantity = 1;
			CRect r(747,600,867,630);
			CalcDeviceRect(r);
			InvalidateRect(r, FALSE);
		}
		else
		{
			CPoint p = pDoc->GetKO();
			// einzelne Buttons um wom�glich eine Ressourcenroute zu k�ndigen
			for (int i = 0; i < pDoc->GetSystem(p).GetResourceRoutes()->GetSize(); i++)
			{
				int j = i + pDoc->GetSystem(p).GetTradeRoutes()->GetSize();
				if (CRect(360,260+j*30,480,290+j*30).PtInRect(point))
				{
					// Eine Ressourcenroute kann nur gek�ndigt werden, wenn sie keine prozentualen Anteile am Bauauftrag
					// besitzt, sprich, wenn sie keine Ressourcen zum Bauauftrag beigetragen hat.
					CResourceRoute* pResRoute = &pDoc->GetSystem(p).GetResourceRoutes()->GetAt(i);
					if (pResRoute->GetPercent() == 0)
					{
						pDoc->GetSystem(p).GetResourceRoutes()->RemoveAt(i--);
						Invalidate(FALSE);
					}
					return;
				}
			}
			// einzelnen Buttons, um Waren zwischen System und globalen Lager hin und her zu schieben
			// Wenn das System blockiert wird kann man nicht mehr Waren ins oder aus dem stellaren Lager
			// nehmen
			if (pDoc->m_System[p.x][p.y].GetBlockade() > NULL)
				return;

			int i = -1;
			if (ButtonReactOnLeftClick(point, &m_SystemTradeButtons, i, FALSE, TRUE))
			{
				// kleine Pfeilbuttons um Waren aus dem globalen Lager ins System zu verschieben
				if (i >= 5)
				{
					i -= 5;

					if (pDoc->m_System[p.x][p.y].GetResourceStore(i) > 0 || pMajor->GetEmpire()->GetGlobalStorage()->GetSubResource(i,p) > 0)
					{
						UINT tempQuantity = m_iGlobalStoreageQuantity;
						if (pDoc->m_System[p.x][p.y].GetResourceStore(i) < m_iGlobalStoreageQuantity && pMajor->GetEmpire()->GetGlobalStorage()->GetSubResource(i,p) == 0)
							m_iGlobalStoreageQuantity = pDoc->m_System[p.x][p.y].GetResourceStore(i);
						UINT getBack = pMajor->GetEmpire()->GetGlobalStorage()->AddRessource(m_iGlobalStoreageQuantity,i,p);
						pDoc->m_System[p.x][p.y].SetResourceStore(i, (getBack - m_iGlobalStoreageQuantity));
						m_iGlobalStoreageQuantity = tempQuantity;
						Invalidate(FALSE);
					}					
				}
				// kleine Pfeilbuttons um Waren aus dem System ins globale Lager zu verschieben
				else 
				{					
					pDoc->m_System[p.x][p.y].SetResourceStore(i, pMajor->GetEmpire()->GetGlobalStorage()->SubRessource(m_iGlobalStoreageQuantity,i,p));
					Invalidate(FALSE);					
				}
			}
		}
	}	

	CMainBaseView::OnLButtonDown(nFlags, point);
}

BOOL CSystemMenuView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	// Wenn wir im Baumen� sind k�nnen wir mit dem Mausrad die Bauliste scrollen
	if (m_bySubMenu == 0)
	{
		if (zDelta < 0)
		{
			if (m_byStartList + NOEIBL / 2 <= m_iClickedOn && m_iClickedOn < m_vBuildlist.GetUpperBound())
				m_byStartList++;
			if (m_iClickedOn < m_vBuildlist.GetUpperBound())
				m_iClickedOn++;
			Invalidate(FALSE);
		}
		else if (zDelta > 0)
		{
			if (m_iClickedOn > 0)
				m_iClickedOn--;
			if (m_byStartList > 0)			
				m_byStartList--;
			Invalidate(FALSE);
		}
	}

	return CMainBaseView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSystemMenuView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// Doppelklick weiterleiten
	CSystemMenuView::OnLButtonDown(nFlags, point);

	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;	

	CalcLogicalPoint(point);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// Wenn wir uns in der Systemansicht befinden
	CPoint p = pDoc->GetKO();
	
	// Baulisteneintrag hinzuf�gen
	if (m_bySubMenu == 0)
	{
		for (int i = m_byStartList; i < m_vBuildlist.GetSize(); i++)
		{
			if (i > m_byEndList)
				break;

			CRect rect(380, 150 + (i - m_byStartList) * 25, 670, 150 + (i - m_byStartList) * 25 + 25);

			if (rect.PtInRect(point))
			{			
				int nID = m_vBuildlist[i];
				int RunningNumber = abs(nID);
				// vor einem Doppelklick m�ssen die ben�tigten Ressourcen sicherheitshalber nochmal neu berechnet.
				// Denn bei ganz flinken Fingern kann es passieren, dass dies in der Draw Methode nicht gemacht werden.
				// also ein Geb�ude oder Geb�udeupdate
				if (nID < 10000)
					pDoc->GetSystem(p).GetAssemblyList()->CalculateNeededRessources(&pDoc->GetBuildingInfo(RunningNumber),0,0, pDoc->GetSystem(p).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
				// also ein Schiff
				else if (nID < 20000 && pDoc->GetSystem(p).GetBuildableShips()->GetSize() > 0)
					pDoc->GetSystem(p).GetAssemblyList()->CalculateNeededRessources(0, &pDoc->m_ShipInfoArray.GetAt(nID - 10000),0, pDoc->GetSystem(p).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
				// also eine Truppe
				else if (pDoc->GetSystem(p).GetBuildableTroops()->GetSize() > 0)
					pDoc->GetSystem(p).GetAssemblyList()->CalculateNeededRessources(0,0,&pDoc->m_TroopInfo.GetAt(nID - 20000), pDoc->GetSystem(p).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
				
				if (pDoc->m_System[p.x][p.y].GetAssemblyList()->MakeEntry(nID, p, pDoc->m_System))
				{
					// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben. 
					// Den Check nur machen, wenn wir ein Update oder ein Geb�ude welches eine Maxanzahl voraussetzt
					// hinzuf�gen wollen
					if (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire() > 0)
					{
						// Wir m�ssen die GlobalBuilding Variable �ndern, weil sich mittlerweile ja solch ein Geb�ude
						// mehr in der Bauliste befindet.
						pDoc->m_GlobalBuildings.AddGlobalBuilding(pMajor->GetRaceID(), RunningNumber);
						// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
						// durchf�hren
						for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
							for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
								if (pDoc->m_System[x][y].GetOwnerOfSystem() == pMajor->GetRaceID())
									pDoc->m_System[x][y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);						
					}
					// sonst den Baulistencheck nur in dem aktuellen System durchf�hren
					else if (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0)
						pDoc->m_System[p.x][p.y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);

					// Wenn wir den Baueintrag setzen konnten, also hier in der if-Bedingung sind, dann CalculateVariables() aufrufen
					pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
					m_iClickedOn = i;

					// Die Struktur Buildlist l�schen
					m_vBuildlist.RemoveAll();
					Invalidate(FALSE);
					return;
				}
				// Baulisteneintrag konnte aufgrund von Ressourcenmangel nicht gesetzt werden
				else
					CSoundManager::GetInstance()->PlaySound(SNDMGR_SOUND_ERROR);
				return;
			}
		}
	}

	// Baulisteneintrag wieder entfernen
	for (int i = 0; i < ALE; i++)
	{
		int nAssemblyListEntry = pDoc->GetSystem(p).GetAssemblyList()->GetAssemblyListEntry(i);
		if (nAssemblyListEntry == 0)
			break;
		
		CRect rect(760, 410 + i * 25, 1000, 410 + i * 25 + 25);
		if (rect.PtInRect(point))
		{
			m_bClickedOnBuyButton = FALSE;
			int RunningNumber = abs(nAssemblyListEntry);
			
			// Nur beim 0-ten Eintrag in der Bauliste, also der, der oben steht
			if (i == 0)
			{
				// bekommen bei Abbruch die Ressourcen bzw. Teile der Ressourcen wieder
				for (int j = TITAN; j <= DERITIUM; j++)
				{
					// bestanden Ressourcenrouten, so kann es sein, dass deren Startsysteme einen Anteil oder auch
					// alles zur�ckbekommen
					long getBackRes = pDoc->GetSystem(p).GetAssemblyList()->GetNeededResourceInAssemblyList(0, j);
					for (int k = 0; k < pMajor->GetEmpire()->GetSystemList()->GetSize(); k++)
						if (pMajor->GetEmpire()->GetSystemList()->GetAt(k).ko != p)
						{
							CPoint ko = pMajor->GetEmpire()->GetSystemList()->GetAt(k).ko;
							for (int l = 0; l < pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetSize(); l++)
								if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetKO() == p)
									if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetResource() == j)
										if (pDoc->m_System[ko.x][ko.y].GetResourceRoutes()->GetAt(l).GetPercent() > 0)
										{
											// sind wir soweit, dann geht ein prozentualer Anteil zur�ck in das
											// Startsystem der Ressourcenroute
											int back = pDoc->GetSystem(p).GetAssemblyList()->GetNeededResourceInAssemblyList(0, j)
												* pDoc->GetSystem(ko).GetResourceRoutes()->GetAt(l).GetPercent() / 100;
											ASSERT(back >= 0);
											pDoc->GetSystem(ko).SetResourceStore(j, back);
											getBackRes -= back;
										}									
						}
					pDoc->GetSystem(p).SetResourceStore(j, getBackRes);
				}
				// Wenn wir was gekauft hatten, dann bekommen wir die Kaufkosten zur�ck
				if (pDoc->GetSystem(p).GetAssemblyList()->GetWasBuildingBought() == TRUE)
				{
					pMajor->GetEmpire()->SetCredits(pDoc->GetSystem(p).GetAssemblyList()->GetBuildCosts());
					// Die Preise an der B�rse anpassen, da wir ja bestimmte Mengen Ressourcen gekauft haben
					// Achtung, hier flag == 1 setzen bei Aufruf der Funktion BuyRessource!!!!
					for (int j = TITAN; j <= IRIDIUM; j++)
						pMajor->GetTrade()->SellRessource(j, pDoc->GetSystem(p).GetAssemblyList()->GetNeededResourceInAssemblyList(0, j), p, 1);
					pDoc->GetSystem(p).GetAssemblyList()->SetWasBuildingBought(FALSE);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CMenuChooseView));
				}
				pDoc->GetSystem(p).GetAssemblyList()->ClearAssemblyList(p, pDoc->m_System);
				// Nach ClearAssemblyList m�ssen wir die Funktion CalculateVariables() aufrufen
				pDoc->GetSystem(p).CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());
				// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben. 
				// Den Check nur machen, wenn wir ein Update oder ein Geb�ude welches eine Maxanzahl voraussetzt
				// hinzuf�gen wollen
				if (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire() > 0)
				{
					// Wir m�ssen die GlobalBuilding Variable �ndern, weil sich mittlerweile ja solch ein Geb�ude
					// weniger in der Bauliste befindet. Nicht aber wenn es ein Upgrade ist.
					pDoc->m_GlobalBuildings.DeleteGlobalBuilding(pMajor->GetRaceID(), RunningNumber);
					// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
					// durchf�hren
					for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
						for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
							if (pDoc->GetSystem(x,y).GetOwnerOfSystem() == pMajor->GetRaceID())
								pDoc->GetSystem(x,y).AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);					
				}
				// sonst den Baulistencheck nur in dem aktuellen System durchf�hren
				else if (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0)
					pDoc->m_System[p.x][p.y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
			}
			// Die restlichen Eintr�ge
			// seperat, weil wir die Bauliste anders l�schen m�ssen und auch keine RES zur�ckbekommen m�ssen
			else
			{
				pDoc->GetSystem(p).GetAssemblyList()->AdjustAssemblyList(i);
				// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben. 
				// Den Check nur machen, wenn wir ein Update oder ein Geb�ude welches eine Maxanzahl voraussetzt
				// hinzuf�gen wollen
				if (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire() > 0)
				{
					// Wir m�ssen die GlobalBuilding Variable �ndern, weil sich mittlerweile ja solch ein Geb�ude
					// weniger in der Bauliste befindet. Nicht aber wenn es ein Upgrade ist.
					pDoc->m_GlobalBuildings.DeleteGlobalBuilding(pMajor->GetRaceID(), RunningNumber);
					// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
					// durchf�hren
					for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
						for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
							if (pDoc->GetSystem(x,y).GetOwnerOfSystem() == pMajor->GetRaceID())
								pDoc->GetSystem(x,y).AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
				}
				// sonst den Baulistencheck nur in dem aktuellen System durchf�hren
				else if (RunningNumber < 10000 && pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0)
					pDoc->m_System[p.x][p.y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
			}
			
			Invalidate(FALSE);
			return;
		}
	}
	
	CMainBaseView::OnLButtonDblClk(nFlags, point);
}

void CSystemMenuView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CalcLogicalPoint(point);

	// Das hier alles nur machen, wenn wir in der System-Ansicht sind
	// Im Geb�ude�bersichtsmen�
	if (m_bySubMenu == 3)
	{
		// Wenn ich Geb�ude nicht mehr abrei�en will, mit rechts auf die Schaltfl�che dr�cken
		CPoint p = pDoc->GetKO();
		for (int i = m_iBOPage * NOBIOL; i < m_BuildingOverview.GetSize(); i++)
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iBOPage * NOBIOL + NOBIOL)
				if (m_BuildingOverview.GetAt(i).rect.PtInRect(point))
				{
					USHORT ru = m_BuildingOverview.GetAt(i).runningNumber;
					pDoc->m_System[p.x][p.y].SetBuildingDestroy(ru,FALSE);
					CRect r = m_BuildingOverview.GetAt(i).rect;
					CalcDeviceRect(r);
					InvalidateRect(r);
					break;
				}
	}

	CMainBaseView::OnRButtonDown(nFlags, point);
}

void CSystemMenuView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;	

	CalcLogicalPoint(point);
	ButtonReactOnMouseOver(point, &m_BuildMenueMainButtons);
	if (m_bySubMenu == 1 || m_bySubMenu == 12)
		ButtonReactOnMouseOver(point, &m_WorkerButtons);
	else if (m_bySubMenu == 4)
		ButtonReactOnMouseOver(point, &m_SystemTradeButtons);
	
	CMainBaseView::OnMouseMove(nFlags, point);
}

void CSystemMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;
	
	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;

	// Wenn wir in irgendeinem System sind, k�nnen wir mit dem links rechts Pfeil zu einem anderen System wechseln
	CPoint p = pDoc->GetKO();
	if (nChar == VK_RIGHT || nChar == VK_LEFT)
	{
		// an welcher Stelle in der Liste befindet sich das aktuelle System?
		short pos = 0;
		for (int i = 0; i < pMajor->GetEmpire()->GetSystemList()->GetSize(); i++)
			if (pMajor->GetEmpire()->GetSystemList()->GetAt(i).ko == p)
			{
				pos = i;
				break;
			}
		if (nChar == VK_RIGHT)
		{
			pos++;
			if (pos == pMajor->GetEmpire()->GetSystemList()->GetSize())
				pos = 0;
		}
		else
		{
			pos--;
			if (pos < 0)
				pos = pMajor->GetEmpire()->GetSystemList()->GetUpperBound();
		}
		if (pMajor->GetEmpire()->GetSystemList()->GetSize() > 1)
		{
			m_iClickedOn = 0;
			m_byStartList = 0;
			pDoc->SetKO(pMajor->GetEmpire()->GetSystemList()->GetAt(pos).ko.x, pMajor->GetEmpire()->GetSystemList()->GetAt(pos).ko.y);
			Invalidate(FALSE);
		}
	}
	// Wenn wir im Baumen� sind k�nnen wir mit den Pfeil auf und Pfeil ab Tasten durch die Bauliste scrollen
	if (m_bySubMenu == 0)
	{
		if (nChar == VK_DOWN)
		{
			if (m_byStartList + NOEIBL / 2 <= m_iClickedOn && m_iClickedOn < m_vBuildlist.GetUpperBound())
				m_byStartList++;
			if (m_iClickedOn < m_vBuildlist.GetUpperBound())
				m_iClickedOn++;
			Invalidate(FALSE);
		}
		else if (nChar == VK_UP)
		{
			if (m_iClickedOn > 0)
				m_iClickedOn--;
			if (m_byStartList > 0)
				m_byStartList--;
			Invalidate(FALSE);
		}
		// Mit der Entertaste k�nnen wir den Auftrag in die Bauliste �bernehmen
		else if (nChar == VK_RETURN && m_iClickedOn < m_vBuildlist.GetSize())
		{
			int nID = m_vBuildlist[m_iClickedOn];			
			int RunningNumber = abs(nID);
			
			// vor einem Doppelklick m�ssen die ben�tigten Ressourcen sicherheitshalber nochmal neu berechnet.
			// Denn bei ganz flinken Fingern kann es passieren, dass dies in der Draw Methode nicht gemacht werden.
			// also ein Geb�ude oder Geb�udeupdate
			if (nID < 10000)
				pDoc->GetSystem(p).GetAssemblyList()->CalculateNeededRessources(&pDoc->GetBuildingInfo(RunningNumber),0,0, pDoc->GetSystem(p).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
			// also ein Schiff
			else if (nID < 20000 && pDoc->GetSystem(p).GetBuildableShips()->GetSize() > 0)
				pDoc->GetSystem(p).GetAssemblyList()->CalculateNeededRessources(0, &pDoc->m_ShipInfoArray.GetAt(nID - 10000),0, pDoc->GetSystem(p).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
			// also eine Truppe
			else if (pDoc->GetSystem(p).GetBuildableTroops()->GetSize() > 0)
				pDoc->GetSystem(p).GetAssemblyList()->CalculateNeededRessources(0,0,&pDoc->m_TroopInfo.GetAt(nID - 20000), pDoc->GetSystem(p).GetAllBuildings(), nID, pMajor->GetEmpire()->GetResearch()->GetResearchInfo());
			
			if (pDoc->m_System[p.x][p.y].GetAssemblyList()->MakeEntry(nID, p, pDoc->m_System))
			{
				// Baulistencheck machen, wenn wir kein Schiff reingesetzt haben. 
				// Den Check nur machen, wenn wir ein Update oder ein Geb�ude welches eine Maxanzahl voraussetzt
				// hinzuf�gen wollen
				if (RunningNumber < 10000 && (nID < 0
					|| pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire() > 0
					||  pDoc->GetBuildingInfo(RunningNumber).GetMaxInSystem().Number > 0))
				{
					// Wenn es nur einmal pro Imperium baubar war, dann Assemblylistcheck in jedem unserer Systeme
					// durchf�hren
					if (pDoc->GetBuildingInfo(RunningNumber).GetMaxInEmpire() > 0)
					{
						for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
							for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
								if (pDoc->m_System[x][y].GetOwnerOfSystem() == m_pPlayersRace->GetRaceID())
									if (m_pPlayersRace->GetRaceBuildingNumber() ==	pDoc->GetBuildingInfo(RunningNumber).GetOwnerOfBuilding())
										pDoc->m_System[x][y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
					}
					// sonst den Baulistencheck nur in dem aktuellen System durchf�hren
					else
						pDoc->m_System[p.x][p.y].AssemblyListCheck(&pDoc->BuildingInfo,&pDoc->m_GlobalBuildings);
				}
				
				// Wenn wir den Baueintrag setzen konnten, also hier in der if-Bedingung sind, dann CalculateVariables() aufrufen
				pDoc->m_System[p.x][p.y].CalculateVariables(&pDoc->BuildingInfo, pMajor->GetEmpire()->GetResearch()->GetResearchInfo(),pDoc->m_Sector[p.x][p.y].GetPlanets(), pMajor, CTrade::GetMonopolOwner());

				// Die Struktur BuildList l�schen, alle Werte auf 0
				m_vBuildlist.RemoveAll();

				Invalidate(FALSE);
			}
		}
	}

	CMainBaseView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSystemMenuView::CreateButtons()
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);

	CString sPrefix = pMajor->GetPrefix();
	// alle Buttons in der View anlegen und Grafiken laden	
	// Buttons in der Systemansicht
	CString fileN = "Other\\" + sPrefix + "button.bop";
	CString fileI = "Other\\" + sPrefix + "buttoni.bop";
	CString fileA = "Other\\" + sPrefix + "buttona.bop";
	m_BuildMenueMainButtons.Add(new CMyButton(CPoint(10,690) , CSize(160,40), CResourceManager::GetString("BTN_BUILDMENUE"), fileN, fileI, fileA));
	m_BuildMenueMainButtons.Add(new CMyButton(CPoint(180,690), CSize(160,40), CResourceManager::GetString("BTN_WORKERSMENUE"), fileN, fileI, fileA));
	m_BuildMenueMainButtons.Add(new CMyButton(CPoint(350,690), CSize(160,40), CResourceManager::GetString("BTN_ENERGYMENUE"), fileN, fileI, fileA));
	m_BuildMenueMainButtons.Add(new CMyButton(CPoint(520,690), CSize(160,40), CResourceManager::GetString("BTN_BUILDING_OVERVIEWMENUE"), fileN, fileI, fileA));
	m_BuildMenueMainButtons.Add(new CMyButton(CPoint(690,690), CSize(160,40), CResourceManager::GetString("BTN_TRADEMENUE"), fileN, fileI, fileA));

	// Zuweisungsbuttons im Arbeitermen�
	fileN = "Other\\" + sPrefix + "buttonminus.bop";
	fileA = "Other\\" + sPrefix + "buttonminusa.bop";
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_WorkerButtons.Add(new CMyButton(CPoint(170,115+i*95) , CSize(40,40), "", fileN, fileN, fileA));
	
	// Zuweisungsbuttons im Arbeitermen�
	fileN = "Other\\" + sPrefix + "buttonplus.bop";
	fileA = "Other\\" + sPrefix + "buttonplusa.bop";
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_WorkerButtons.Add(new CMyButton(CPoint(630,115+i*95) , CSize(40,40), "", fileN, fileN, fileA));
	
	// Zuweisungsbuttons im Systemhandelsmen�
	fileN = "Other\\" + sPrefix + "buttonminus.bop";
	fileA = "Other\\" + sPrefix + "buttonminusa.bop";
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_SystemTradeButtons.Add(new CMyButton(CPoint(608,235+i*60) , CSize(30,30), "", fileN, fileN, fileA));
	fileN = "Other\\" + sPrefix + "buttonplus.bop";
	fileA = "Other\\" + sPrefix + "buttonplusa.bop";
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_SystemTradeButtons.Add(new CMyButton(CPoint(975,235+i*60) , CSize(30,30), "", fileN, fileN, fileA));
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CSystemMenuView::CreateTooltip(void)
{
	// Tooltips erstmal nur im Baumen� und in der Energie- und Geb�ude�bersicht anzeigen
	if (m_bySubMenu != 0 && m_bySubMenu != 2 && m_bySubMenu != 3)
		return "";

	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return "";

	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);	
	
	int nID = -1;
	if (m_bySubMenu == 0)
	{
		// Pr�fen ob die Maus �ber das angezeigte Bild gehalten wurde
		if (m_iClickedOn >= 0 && m_iClickedOn < m_vBuildlist.GetSize())
			if (CRect(70, 60, 70 + 200, 60 + 150).PtInRect(pt))
				nID = abs(m_vBuildlist[m_iClickedOn]);
	}	
	else if (m_bySubMenu == 2)
	{
		// Pr�fen �ber welches Geb�ude die Maus gehalten wurde
		for (int i = m_iELPage * NOBIEL; i < m_EnergyList.GetSize(); i++)
		{
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iELPage * NOBIEL + NOBIEL)
				if (m_EnergyList.GetAt(i).rect.PtInRect(pt))
				{	
					nID = pDoc->GetSystem(pDoc->GetKO()).GetAllBuildings()->GetAt(m_EnergyList.GetAt(i).index).GetRunningNumber();
					break;
				}
		}
	}
	else if (m_bySubMenu == 3)
	{
		// Pr�fen �ber welches Geb�ude die Maus gehalten wurde
		for (int i = m_iBOPage * NOBIOL; i < m_BuildingOverview.GetSize(); i++)
		{
			// Wenn wir auf der richtigen Seite sind
			if (i < m_iBOPage * NOBIOL + NOBIOL)
				if (m_BuildingOverview.GetAt(i).rect.PtInRect(pt))
				{
					nID = m_BuildingOverview.GetAt(i).runningNumber;
					break;
				}
		}
	}	

	if (nID != -1)
	{
		// Geb�ude oder Update
		if (nID < 10000)
		{
			CString sName = pDoc->GetBuildingInfo(nID).GetBuildingName();
			sName = CHTMLStringBuilder::GetHTMLColor(sName);
			sName = CHTMLStringBuilder::GetHTMLHeader(sName, _T("h3"));
			sName = CHTMLStringBuilder::GetHTMLCenter(sName);
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sProd = pDoc->GetBuildingInfo(nID).GetProductionAsString();
			sProd = CHTMLStringBuilder::GetHTMLColor(sProd);
			sProd = CHTMLStringBuilder::GetHTMLHeader(sProd, _T("h5"));
			sProd += CHTMLStringBuilder::GetHTMLStringNewLine();
			sProd += CHTMLStringBuilder::GetHTMLStringHorzLine();
			sProd += CHTMLStringBuilder::GetHTMLStringNewLine();
							
			CString sDesc = pDoc->GetBuildingInfo(nID).GetBuildingDescription();
			sDesc = CHTMLStringBuilder::GetHTMLColor(sDesc, _T("silver"));
			sDesc = CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h5"));
			
			return sName + sProd + sDesc;
		}
		// Schiff
		else if (nID < 20000)
		{
			// Schiff erzeugen und Spezialforschungen einbeziehen
			CShip ship = pDoc->m_ShipInfoArray[nID - 10000];
			pDoc->AddSpecialResearchBoniToShip(&ship, m_pPlayersRace);
			return ship.GetTooltip(false);
		}
	}

	return "";
}