/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "Botf2Doc.h"
#include "MainBaseView.h"


// CShipDesignMenuView view

class CShipDesignMenuView : public CMainBaseView
{
	DECLARE_DYNCREATE(CShipDesignMenuView)

protected:
	CShipDesignMenuView();           // protected constructor used by dynamic creation
	virtual ~CShipDesignMenuView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	/// Funktion f�hrt Aufgaben aus, welche zu jeder neuen Runde von den Views ausgef�hrt werden m�ssen.
	void OnNewRound(void);
	
private:
	// Funktionen
	/// Funkion legt alle Buttons f�r die Geheimdienstansichten an.
	void CreateButtons();

	/// Funktion zum Zeichnen des Schiffsdesignmen�s
	/// @param g Zeiger auf GDI+ Grafikobjekt
	void DrawShipDesignMenue(Graphics* g);

	/// Funktion �berpr�ft ob das in der Designansicht angeklickte Schiff in einem unserer Systeme gerade gebaut wird
	/// Man ben�tigt diesen Check da man keine Schiffe �ndern kann, welche gerade gebaut werden.
	/// @param n Index des zu pr�fenden Schiffes aus der Schiffsliste
	/// @return CString mit dem Namen des Systems, wird das Schiff nirgends gebaut ist der String leer
	CString CheckIfShipIsBuilding(int n) const;

	// Attribute

	// Grafiken
	Bitmap* bg_designmenu;			///< Schiffsdesignmenu

	// Buttons

	// Hier die Variablen, die wir ben�tigen, wenn wir in der Schiffsdesignansicht sind
	short  m_iClickedOnShip;			// auf welches Schiff wurde in der Designansicht geklickt
	short  m_iOldClickedOnShip;			// auf welches Schiffe wurde vorher geklickt, braucht man als Modifikator
	short  m_nSizeOfShipDesignList;		///< Anzahl der Eintr�ge in der Schiffsdesignliste
	USHORT m_iBeamWeaponNumber;			// Nummer der Beamwaffe
	USHORT m_iTorpedoWeaponNumber;		// Nummer der Torpedowaffe
	BOOLEAN m_bFoundBetterBeam;			// Gibt es einen besseren Beamtyp
	BOOLEAN m_bFoundWorseBeam;			// Gibt es einen schlechteren Beamtyp

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

