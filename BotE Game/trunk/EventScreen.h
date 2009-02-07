/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"
#include "MyButton.h"

using namespace Gdiplus;

class CEventScreen : public CObject
{	
public:
	DECLARE_SERIAL (CEventScreen)

	/// Standardkonstruktor
	CEventScreen(void);

	/// Standardkonstruktor mit Parameterübergabe
	CEventScreen(BYTE playersRace, const CString &imageName, const CString &headline = "", const CString &text = "");

	/// Standarddestruktor
	virtual ~CEventScreen(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	CArray<CMyButton*>* GetButtons() {return &m_Buttons;}

	virtual void Create();

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;

// Attribute
protected:
	BYTE m_byRace;

	Gdiplus::Image* m_pBGImage;

	CString m_strImagePath;

	CString m_strHeadline;

	CString m_strText;

	CArray<CMyButton*> m_Buttons;
};
