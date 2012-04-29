// PlanetBottomView.cpp : implementation file
//

#include "stdafx.h"
#include "botf2.h"
#include "MainFrm.h"
#include "GalaxyMenuView.h"
#include "PlanetBottomView.h"
#include "ShipBottomView.h"
#include "SmallInfoView.h"
#include "Races\RaceController.h"
#include "HTMLStringBuilder.h"
#include "Galaxy\Anomaly.h"
#include "Graphic\memdc.h"

// CPlanetBottomView

IMPLEMENT_DYNCREATE(CPlanetBottomView, CBottomBaseView)

CPlanetBottomView::CPlanetBottomView()
{	
}

CPlanetBottomView::~CPlanetBottomView()
{	
}

BEGIN_MESSAGE_MAP(CPlanetBottomView, CBottomBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CPlanetBottomView drawing

void CPlanetBottomView::OnDraw(CDC* dc)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	CMajor* pMajor = m_pPlayersRace;
	ASSERT(pMajor);
	if (!pMajor)
		return;
	// TODO: add draw code here

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
				
	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(Color::White);
	Bitmap* graphic = NULL;

	// alte Planetenrechtecke l�schen
	m_vPlanetRects.clear();

	// Galaxie im Hintergrund zeichnen
	CString sPrefix = pMajor->GetPrefix();
	
	Bitmap* background = pDoc->GetGraphicPool()->GetGDIGraphic("Backgrounds\\" + sPrefix + "galaxyV3.bop");
	if (background)
		g.DrawImage(background, 0, 0, 1075, 249);
	
	CPoint KO = pDoc->GetKO();
	short resizeItX = 0;
	float maxHabitants = 0.0f;
	int nPosX = m_TotalSize.cx - 175;
	int nVertCenter = m_TotalSize.cy / 2;

	if (pDoc->m_Sector[KO.x][KO.y].GetFullKnown(pMajor->GetRaceID()) == TRUE)
	{
		for (int i = 0; i < pDoc->m_Sector[KO.x][KO.y].GetNumberOfPlanets(); i++)
		{
			CPlanet* planet = pDoc->m_Sector[KO.x][KO.y].GetPlanet(i);
			maxHabitants += planet->GetMaxHabitant();

			CRect rect;
			switch (planet->GetSize())
			{
			case PLANT_SIZE::SMALL:
				nPosX -= 75;
				rect.SetRect(nPosX, nVertCenter - 23, nPosX + 45, nVertCenter + 22);
				break;
			case PLANT_SIZE::NORMAL:
				nPosX -= 90;
				rect.SetRect(nPosX, nVertCenter - 30, nPosX + 60, nVertCenter + 30);
				break;
			case PLANT_SIZE::BIG:
				nPosX -= 105;
				rect.SetRect(nPosX, nVertCenter - 40, nPosX + 80, nVertCenter + 40);
				break;
			case PLANT_SIZE::GIANT:
				if (planet->GetPlanetName() == "Saturn")
				{	
					nPosX -= 155;
					rect.SetRect(nPosX, nVertCenter - 35, nPosX + 145, nVertCenter + 49);					
				}
				else
				{
					nPosX -= 145;
					rect.SetRect(nPosX, nVertCenter - 63, nPosX + 125, nVertCenter + 62);					
				}
				break;
			}
			
			m_vPlanetRects.push_back(rect);
			planet->DrawPlanet(g, rect, pDoc->GetGraphicPool());
		}			
	}
	if (pDoc->GetSector(KO.x,KO.y).GetScanned(pMajor->GetRaceID()))
	{
		if (pDoc->GetSector(KO.x,KO.y).GetSunSystem())
		{
			graphic = NULL;
			switch(pDoc->GetSector(KO.x,KO.y).GetSunColor())
			{
			case 0:
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_blue.bop"); break;
			case 1:
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_green.bop"); break;
			case 2:
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_orange.bop"); break;
			case 3:
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_red.bop"); break;
			case 4:
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_violet.bop"); break;			
			case 5:
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_white.bop"); break;
			case 6:
				graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Suns\\sun_yellow.bop"); break;					
			}				
			if (graphic)
				g.DrawImage(graphic, 950, -10, 250, 261);
		}
		else if (pDoc->GetSector(KO).GetAnomaly())
		{
			graphic = NULL;;
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("MapStars\\" + pDoc->GetSector(KO).GetAnomaly()->GetImageFileName());

			if (graphic)
			{
				if (pDoc->GetSector(KO).GetAnomaly()->GetImageFlipHorz())
				{
					Bitmap* copy = graphic->Clone(0, 0, graphic->GetWidth(), graphic->GetHeight(), PixelFormat32bppPARGB);
					copy->RotateFlip(Gdiplus::RotateNoneFlipX);
					g.DrawImage(copy, 500, -11, 284, 284);
					delete copy;
					copy = NULL;
				}
				else
					g.DrawImage(graphic, 500, -11, 284, 284);
			}
		}
	}
	
	// Rassenspezifische Schriftart ausw�hlen
	CFontLoader::CreateGDIFont(pMajor, 2, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentNear);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontBrush.SetColor(Color(170,170,170));
				
	// Informationen zu dem System angeben
	CString s;
	float currentHabitants = pDoc->m_Sector[KO.x][KO.y].GetCurrentHabitants();
	if (pDoc->GetSector(KO).GetAnomaly() && pDoc->GetSector(KO).GetScanned(pMajor->GetRaceID()))
		s.Format("%s", pDoc->GetSector(KO).GetAnomaly()->GetMapName(KO));
	else
		s.Format("%s %c%i",CResourceManager::GetString("SECTOR"),(char)(KO.y+97),KO.x+1);
	g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,25), &fontFormat, &fontBrush);
	
	if (pDoc->m_Sector[KO.x][KO.y].GetScanned(pMajor->GetRaceID()) == FALSE)
	{
		s = CResourceManager::GetString("UNKNOWN");
		g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,47), &fontFormat, &fontBrush);
	}
	else if (pDoc->m_Sector[KO.x][KO.y].GetSunSystem() == TRUE && pDoc->m_Sector[KO.x][KO.y].GetKnown(pMajor->GetRaceID()) == TRUE)
	{
		// vorhandene Rohstoffe auf allen Planeten zeichnen
		s = CResourceManager::GetString("EXISTING_RES") + ":";
		g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(735,228), &fontFormat, &fontBrush);
		RectF boundingBox;
		g.MeasureString(s.AllocSysString(), s.GetLength(), &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(735, 228), &fontFormat, &boundingBox); 
		// Symbole der vorhanden Ressourcen im System ermitteln
		BOOLEAN res[DERITIUM + 1] = {0},rescol[DERITIUM + 1] = {0};
		pDoc->GetSector(KO).GetAvailableResources(res, false);	//alle Ressourcen
		pDoc->GetSector(KO).GetAvailableResources(rescol, true);//erschlossene Ressourcen
		int nExist = 0;
		for (int i = TITAN; i <= DERITIUM; i++)
		{
			if (res[i])
			{
				graphic = NULL;
				switch(i)
				{
					case TITAN:		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\titanSmall.bop");		break;
					case DEUTERIUM: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\deuteriumSmall.bop");	break;
					case DURANIUM:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\duraniumSmall.bop");	break;
					case CRYSTAL:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\crystalSmall.bop");		break;
					case IRIDIUM:	graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\iridiumSmall.bop");		break;
					case DERITIUM: graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\Deritium.bop");		break;
				}
				if (graphic)
					g.DrawImage(graphic, (int)boundingBox.GetRight() + 5 + nExist * 20, 228, 20, 16);
				if(!rescol[i])
				{
					fontBrush.SetColor(Color(100,0,0,0));//graut nicht erschlossene Ressourcen aus
					g.FillRectangle(&fontBrush,(int)boundingBox.GetRight() + 5 + nExist * 20, 228,20,16);
					fontBrush.SetColor(Color(170,170,170));
				}
				nExist++;
			}
		}
		
		s.Format("%s: %s",CResourceManager::GetString("SYSTEM"), pDoc->m_Sector[KO.x][KO.y].GetName());
		g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,47), &fontFormat, &fontBrush);
		if (pDoc->m_Sector[KO.x][KO.y].GetFullKnown(pMajor->GetRaceID()))
		{
			s.Format("%s: %.3lf %s",CResourceManager::GetString("MAX_HABITANTS"), maxHabitants, CResourceManager::GetString("MRD"));
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,180), &fontFormat, &fontBrush);
			s.Format("%s: %.3lf %s",CResourceManager::GetString("CURRENT_HABITANTS"), currentHabitants, CResourceManager::GetString("MRD"));
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,202), &fontFormat, &fontBrush);
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\popmaxSmall.bop");
			if (graphic)
				g.DrawImage(graphic, 23, 180, 20, 16);
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\populationSmall.bop");			
			if (graphic)
				g.DrawImage(graphic, 23, 202, 20, 16);
		}
	}
	// Symbole zu Truppen zeichnen
	if (pDoc->GetSector(KO).GetSunSystem() && pDoc->GetSector(KO).GetOwnerOfSector() != "" &&
		(pDoc->GetSector(KO).GetScanPower(pMajor->GetRaceID()) > 50 || pDoc->GetSector(KO).GetOwnerOfSector() == pMajor->GetRaceID()))
	{
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Other\\troopSmall.bop");
		int nTroopNumber = pDoc->GetSystem(KO).GetTroops()->GetSize();
		if (graphic && nTroopNumber)
		{
			// soviel wie Truppen stationiert sind, so viele Symbole werden gezeichnet
			s.Format("%d x", nTroopNumber);
			int nPosX = 0;
			if (nTroopNumber >= 10)
				nPosX = 10;
			if (nTroopNumber >= 100)
				nPosX = 20;
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(40,69), &fontFormat, &fontBrush);
			// Bild zeichnen
			g.DrawImage(graphic, 62 + nPosX, 64, 24, 24);
		}
	}
	// Scannerst�rke zeichnen
	if (pDoc->m_Sector[KO.x][KO.y].GetScanned(pMajor->GetRaceID()) == TRUE)
	{
		// Rassenspezifische Schrift ausw�hlen
		CFontLoader::CreateGDIFont(pMajor, 1, fontName, fontSize);
		s.Format("%s: %i%%",CResourceManager::GetString("SCANPOWER"), pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()));
		if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) >= 75)
			fontBrush.SetColor(Color(0,245,0));
		else if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) >= 50)
			fontBrush.SetColor(Color(50,180,50));
		else if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) >= 25)
			fontBrush.SetColor(Color(230,230,20));
		else if (pDoc->m_Sector[KO.x][KO.y].GetScanPower(pMajor->GetRaceID()) > 0)
			fontBrush.SetColor(Color(230,100,0));
		else
			fontBrush.SetColor(Color(245,0,0));		
		
		g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(711,0), &fontFormat, &fontBrush);
	}
	// Namen des Besitzers des Sector unten rechts zeichnen
	if (pDoc->GetSector(KO).GetScanned(pMajor->GetRaceID()) && pMajor->IsRaceContacted(pDoc->GetSector(KO).GetOwnerOfSector())
		|| pDoc->GetSector(KO).GetOwnerOfSector() == pMajor->GetRaceID())
	{
		CRace* pOwner = pDoc->GetRaceCtrl()->GetRace(pDoc->GetSector(KO).GetOwnerOfSector());
		if (pOwner)
		{
			s = pOwner->GetRaceName();
			if (pOwner->GetType() == MAJOR)
			{
				Color color;
				color.SetFromCOLORREF(((CMajor*)pOwner)->GetDesign()->m_clrSector);
				fontBrush.SetColor(color);
			}
			else
				fontBrush.SetColor(Color(255,255,255));			

			CFontLoader::CreateGDIFont(pMajor, 4, fontName, fontSize);
			g.DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), PointF(735,190), &fontFormat, &fontBrush);
		}		

		// Wir selbst und alle uns bekannten Rassen sehen, wenn das System blockiert wird.
		// Daf�r wird ein OverlayBanner �ber die Ansicht gelegt.
		if (pDoc->m_System[KO.x][KO.y].GetBlockade() > NULL)
		{
			CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
			CSize viewSize(m_TotalSize.cx - 160, m_TotalSize.cy - 120);
			s.Format("%d", pDoc->m_System[KO.x][KO.y].GetBlockade());
			COverlayBanner* banner = new COverlayBanner(CPoint(80,60), viewSize, CResourceManager::GetString("SYSTEM_IS_BLOCKED", FALSE, s), RGB(200,0,0));
			banner->SetBorderWidth(1);
			Gdiplus::Font font(fontName.AllocSysString(), fontSize);
			banner->Draw(&g, &font);
			delete banner;
		}
	}

	g.ReleaseHDC(pDC->GetSafeHdc());
}


// CPlanetBottomView diagnostics

#ifdef _DEBUG
void CPlanetBottomView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CPlanetBottomView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPlanetBottomView message handlers

BOOL CPlanetBottomView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return FALSE;
}

void CPlanetBottomView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);

	if (!pDoc->m_bDataReceived)
		return;

	if (m_vPlanetRects.size() == 0)
		return;

	CalcLogicalPoint(point);
	
	CPoint KO = pDoc->GetKO();
	for (UINT i = 0; i < m_vPlanetRects.size(); i++)
		if (m_vPlanetRects[i].PtInRect(point))
		{
			CSmallInfoView::SetPlanetInfo(true);
			CSmallInfoView::SetPlanet(pDoc->m_Sector[KO.x][KO.y].GetPlanet(i));
			pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
			break;
		}
	// Wenn wir die Planeten sehen und haben ein Schiff gew�hlt welches einen Planeten kolonisieren bzw. terraformen
	// soll m�ssen wir hier den Planeten anklicken k�nnen
	if (CGalaxyMenuView::IsMoveShip() == TRUE)
	{
		short nOldTerraformingPlanet = pDoc->m_ShipArray.ElementAt(pDoc->GetCurrentShipIndex()).GetTerraformingPlanet();
		if (nOldTerraformingPlanet != -1)
			pDoc->GetSector(pDoc->GetKO()).GetPlanet(nOldTerraformingPlanet)->SetIsTerraforming(FALSE);

		pDoc->m_ShipArray.ElementAt(pDoc->GetCurrentShipIndex()).SetTerraformingPlanet(-1);
		// Haben wir auf einen Planeten geklickt
		for (UINT i = 0; i < m_vPlanetRects.size(); i++)
			if (m_vPlanetRects[i].PtInRect(point))
			{
				// Lange Abfrage hie notwendig, weil bei Kolonisierung brauchen wir nen geterraformten Planeten und
				// beim Terraforming nen bewohnbaren noch nicht geterraformten Planeten
				if (pDoc->GetSector(pDoc->GetKO()).GetPlanet(i)->GetTerraformed() == FALSE
					&& pDoc->GetSector(pDoc->GetKO()).GetPlanet(i)->GetHabitable() == TRUE
					&& pDoc->m_ShipArray.GetAt(pDoc->GetCurrentShipIndex()).GetCurrentOrder() == SHIP_ORDER::TERRAFORM)
				{
					CGalaxyMenuView::SetMoveShip(FALSE);
					CShipBottomView::SetShowStation(false);
					CSmallInfoView::SetShipInfo(true);
					pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));
					pDoc->m_ShipArray.ElementAt(pDoc->GetCurrentShipIndex()).SetTerraformingPlanet(i);
					pDoc->m_Sector[pDoc->GetKO().x][pDoc->GetKO().y].GetPlanet(i)->SetIsTerraforming(TRUE);
					// den Terraformingbefehl zur�cknehmen, wenn kein anderes Schiff diesen Planeten mehr terraform
					if (i != nOldTerraformingPlanet && nOldTerraformingPlanet != -1)
					{
						for (int y = 0; y < pDoc->m_ShipArray.GetSize(); y++)
							if (pDoc->m_ShipArray[y].GetKO() == pDoc->GetKO() && pDoc->m_ShipArray[y].GetTerraformingPlanet() == nOldTerraformingPlanet)
							{
								pDoc->GetSector(pDoc->GetKO()).GetPlanet(nOldTerraformingPlanet)->SetIsTerraforming(TRUE);
								break;
							}
					}					

					Invalidate();
				}

				break;
			}
	}

	CBottomBaseView::OnLButtonDown(nFlags, point);
}

void CPlanetBottomView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_vPlanetRects.size() == 0)
		return;

	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	ASSERT(pDoc);
	
	CalcLogicalPoint(point);

	CPoint KO = pDoc->GetKO();
	for (UINT i = 0; i < m_vPlanetRects.size(); i++)
		if (m_vPlanetRects[i].PtInRect(point))
		{
			CPlanet* pPlanet = pDoc->m_Sector[KO.x][KO.y].GetPlanet(i);
			if (pPlanet != CSmallInfoView::GetPlanet())
			{
				CSmallInfoView::SetPlanet(pPlanet);
				CSmallInfoView::SetPlanetStats(true);
				pDoc->GetMainFrame()->InvalidateView(RUNTIME_CLASS(CSmallInfoView));				
			}
			break;
		}

	CBottomBaseView::OnMouseMove(nFlags, point);
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CPlanetBottomView::CreateTooltip(void)
{
	CBotf2Doc* pDoc = (CBotf2Doc*)GetDocument();
	CPoint KO = pDoc->GetKO();

	if (!pDoc->m_bDataReceived)
		return "";

	// Wo sind wir
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CalcLogicalPoint(pt);

	// wurde die Maus �ber den Namen einer Rasse gehalten
	if (pDoc->GetSector(KO).GetOwnerOfSector() != "")
	{
		if (CRect(735,190,885,220).PtInRect(pt))
		{
			CMajor* pMajor = m_pPlayersRace;
			ASSERT(pMajor);
			if (!pMajor)
				return "";

			if (pDoc->GetSector(KO).GetScanned(pMajor->GetRaceID()) && pMajor->IsRaceContacted(pDoc->GetSector(KO).GetOwnerOfSector()) || pDoc->GetSector(KO).GetOwnerOfSector() == pMajor->GetRaceID())
			{
				CRace* pOwner = pDoc->GetRaceCtrl()->GetRace(pDoc->GetSector(KO).GetOwnerOfSector());
				if (!pOwner)
					return "";
				
				return pOwner->GetTooltip();
			}
		}
	}

	if (pDoc->GetSector(KO).GetAnomaly())
	{
		if (CRect(500, 0, 784, m_TotalSize.cy).PtInRect(pt))
		{
			CString sName = pDoc->GetSector(KO).GetAnomaly()->GetMapName(KO);
			sName = CHTMLStringBuilder::GetHTMLColor(sName);
			sName = CHTMLStringBuilder::GetHTMLHeader(sName);
			sName = CHTMLStringBuilder::GetHTMLCenter(sName);
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sDesc = pDoc->GetSector(KO).GetAnomaly()->GetPhysicalDescription();
			sDesc = CHTMLStringBuilder::GetHTMLColor(sDesc);
			sDesc = CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h5"));
			sDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
			sDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
			sDesc += CHTMLStringBuilder::GetHTMLStringHorzLine();
			sDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
			sDesc += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sGame = pDoc->GetSector(KO).GetAnomaly()->GetGameplayDescription();
			sGame = CHTMLStringBuilder::GetHTMLColor(sGame, _T("silver"));
			sGame = CHTMLStringBuilder::GetHTMLHeader(sGame, _T("h5"));

			return sName + sDesc + sGame;
		}
	}

	// wurde die Maus �ber die Sonne gehalten
	else if (pDoc->GetSector(KO).GetSunSystem())
	{
		if (CRect(m_TotalSize.cx - 125, 0, m_TotalSize.cx, m_TotalSize.cy).PtInRect(pt))
		{
			CString sSunColor;
			switch (pDoc->GetSector(KO).GetSunColor())
			{
			case 0: sSunColor = CResourceManager::GetString("BLUE_STAR");	break;
			case 1: sSunColor = CResourceManager::GetString("GREEN_STAR");	break;
			case 2: sSunColor = CResourceManager::GetString("ORANGE_STAR");	break;
			case 3: sSunColor = CResourceManager::GetString("RED_STAR");	break;
			case 4: sSunColor = CResourceManager::GetString("VIOLET_STAR");	break;
			case 5: sSunColor = CResourceManager::GetString("WHITE_STAR");	break;
			case 6: sSunColor = CResourceManager::GetString("YELLOW_STAR");	break;
			}
			sSunColor = CHTMLStringBuilder::GetHTMLColor(sSunColor);
			sSunColor = CHTMLStringBuilder::GetHTMLHeader(sSunColor);
			sSunColor = CHTMLStringBuilder::GetHTMLCenter(sSunColor);
			sSunColor += CHTMLStringBuilder::GetHTMLStringNewLine();
			sSunColor += CHTMLStringBuilder::GetHTMLStringNewLine();
			
			CString sSunDesc;
			switch (pDoc->GetSector(KO).GetSunColor())
			{
			case 0: sSunDesc = CResourceManager::GetString("BLUE_STAR_DESC");	break;
			case 1: sSunDesc = CResourceManager::GetString("GREEN_STAR_DESC");	break;
			case 2: sSunDesc = CResourceManager::GetString("ORANGE_STAR_DESC");	break;
			case 3: sSunDesc = CResourceManager::GetString("RED_STAR_DESC");	break;
			case 4: sSunDesc = CResourceManager::GetString("VIOLET_STAR_DESC");	break;
			case 5: sSunDesc = CResourceManager::GetString("WHITE_STAR_DESC");	break;
			case 6: sSunDesc = CResourceManager::GetString("YELLOW_STAR_DESC");	break;
			}
			sSunDesc = CHTMLStringBuilder::GetHTMLColor(sSunDesc);
			sSunDesc = CHTMLStringBuilder::GetHTMLHeader(sSunDesc, _T("h5"));
			sSunDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
			
			// wurden keine Planeten angezeigt, das System ist also nicht bekannt, dann hier aufh�ren
			if (m_vPlanetRects.size() == 0)
				return sSunColor + sSunDesc;
			
			sSunDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
			sSunDesc += CHTMLStringBuilder::GetHTMLStringHorzLine();
			sSunDesc += CHTMLStringBuilder::GetHTMLStringNewLine();
			
			CString sSystemBoni = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString("BONI_IN_SYSTEM"), _T("silver"));
			sSystemBoni = CHTMLStringBuilder::GetHTMLHeader(sSystemBoni, _T("h4"));
			sSystemBoni = CHTMLStringBuilder::GetHTMLCenter(sSystemBoni);
			sSystemBoni += CHTMLStringBuilder::GetHTMLStringNewLine();
			
			for (int j = 0; j < 8; j++)
			{
				short nBonus = 0;
				for (int i = 0; i < pDoc->GetSector(KO).GetNumberOfPlanets(); i++)
				{
					CPlanet* pPlanet = pDoc->m_Sector[KO.x][KO.y].GetPlanet(i);
					ASSERT(pPlanet);
					if (pPlanet->GetBoni()[j])
						if (j != DERITIUM)
							nBonus += (pPlanet->GetSize() + 1) * 25;
				}
				if (nBonus)
				{
					sSystemBoni += CHTMLStringBuilder::GetHTMLStringNewLine();
					CString sBoni;
					switch(j)
					{
						case TITAN:		sBoni = CResourceManager::GetString("TITAN_BONUS");		break;
						case DEUTERIUM: sBoni = CResourceManager::GetString("DEUTERIUM_BONUS"); break;
						case DURANIUM:	sBoni = CResourceManager::GetString("DURANIUM_BONUS");	break;
						case CRYSTAL:	sBoni = CResourceManager::GetString("CRYSTAL_BONUS");	break;
						case IRIDIUM:	sBoni = CResourceManager::GetString("IRIDIUM_BONUS");	break;
						case 6:			sBoni = CResourceManager::GetString("FOOD_BONUS");		break;
						case 7:			sBoni = CResourceManager::GetString("ENERGY_BONUS");	break;
					}
					
					CString sBonus;
					sBonus.Format("%d%% %s",nBonus, sBoni);
					sBonus = CHTMLStringBuilder::GetHTMLColor(sBonus);
					sBonus = CHTMLStringBuilder::GetHTMLHeader(sBonus, _T("h5"));
					sSystemBoni += sBonus;				
				}
			}
			return sSunColor + sSunDesc + sSystemBoni;
		}	

		// wurden keine Planeten angezeigt, das System ist also nicht bekannt, dann hier aufh�ren
		if (m_vPlanetRects.size() == 0)
			return "";

		// wurde die Maus �ber einen der Planeten gehalten oder �ber die Planetenboni?
		for (UINT i = 0; i < m_vPlanetRects.size(); i++)
		{
			CPlanet* pPlanet = pDoc->m_Sector[KO.x][KO.y].GetPlanet(i);
			ASSERT(pPlanet);

			// wurde auf den Planeten gezeigt
			if (m_vPlanetRects[i].PtInRect(pt))
			{
				CString sTip;				
				
				CString sName = CHTMLStringBuilder::GetHTMLColor(pPlanet->GetPlanetName());
				sName = CHTMLStringBuilder::GetHTMLHeader(sName);
				sName = CHTMLStringBuilder::GetHTMLCenter(sName);
				sName += CHTMLStringBuilder::GetHTMLStringNewLine();
				sTip += sName;
				
				CString s;
				s.Format("CLASS_%c_TYPE", pPlanet->GetClass());
				s = CHTMLStringBuilder::GetHTMLColor(_T("(") + CResourceManager::GetString(s) + _T(")"), _T("silver"));
				s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h4"));
				s = CHTMLStringBuilder::GetHTMLCenter(s);
				s += CHTMLStringBuilder::GetHTMLStringNewLine();
				s += CHTMLStringBuilder::GetHTMLStringNewLine();
				sTip += s;
				
				s.Format("CLASS_%c_INFO", pPlanet->GetClass());
				s = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString(s));
				s = CHTMLStringBuilder::GetHTMLHeader(s, _T("h5"));			
				sTip += s;				
				return sTip;			
			}
			else
			{
				// wurde knapp �ber das Planetenrechteck gezeigt
				CRect boniRect = m_vPlanetRects[i];
				boniRect.OffsetRect(0, -23);
				if (boniRect.PtInRect(pt))
				{
					// pr�fen ob beim Planeten auf eine der m�glichen Boni gezeigt wurde
					const BOOLEAN* boni = pPlanet->GetBoni();
					// erstmal schauen, wieviele Boni �berhaupt vorhanden sind
					BYTE n = 0;
					for (int j = 0; j < 8; j++)
						n += boni[j];
					if (n == 0)
						continue;
					
					int x = boniRect.CenterPoint().x - 5;
					x -= (n*9-4);
					int y = boniRect.top;

					for (int j = 0; j < 8; j++)
						if (boni[j])
						{
							CRect rect(x, y, x + 20, y + 16);
							if (rect.PtInRect(pt))
							{
								CString sBoni;
								switch(j)
								{
									case TITAN:		sBoni = CResourceManager::GetString("TITAN_BONUS"); break;
									case DEUTERIUM: sBoni = CResourceManager::GetString("DEUTERIUM_BONUS"); break;
									case DURANIUM:	sBoni = CResourceManager::GetString("DURANIUM_BONUS"); break;
									case CRYSTAL:	sBoni = CResourceManager::GetString("CRYSTAL_BONUS"); break;
									case IRIDIUM:	sBoni = CResourceManager::GetString("IRIDIUM_BONUS"); break;
									case DERITIUM:
										sBoni = CHTMLStringBuilder::GetHTMLColor(CResourceManager::GetString("DERITIUM") + " " + CResourceManager::GetString("EXISTING"));
										sBoni = CHTMLStringBuilder::GetHTMLHeader(sBoni, _T("h5"));
										return CHTMLStringBuilder::GetHTMLCenter(sBoni);									
									case 6:			sBoni = CResourceManager::GetString("FOOD_BONUS"); break;
									case 7:			sBoni = CResourceManager::GetString("ENERGY_BONUS"); break;
								}
								CString sTip;
								sTip.Format("%d%% %s",(pPlanet->GetSize() + 1) * 25, sBoni);
								sTip = CHTMLStringBuilder::GetHTMLColor(sTip);
								sTip = CHTMLStringBuilder::GetHTMLHeader(sTip, _T("h5"));
								return CHTMLStringBuilder::GetHTMLCenter(sTip);
							}					
							x += 18;					
						}
				}
			}	
		}		
	}
	
	return "";
}
