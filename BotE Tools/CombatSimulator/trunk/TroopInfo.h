/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "troop.h"

class CTroopInfo :	public CTroop
{
public:
	/// Konstruktor
	CTroopInfo(void);

	/// Konstruktor mit kompletter Parameter�bergabe
	CTroopInfo(CString name, CString desc, BYTE power, BYTE costs, BYTE techs[6], USHORT res[5], USHORT ip,
		BYTE ID, BYTE owner, USHORT size, BYTE moralValue);
	
	/// Destruktor
	~CTroopInfo(void);

	// Zugriffsfunktionen zum Lesen der Membervariablen
	/**
	 * Diese Funktion gibt den Namen der Truppe zur�ck.
	 */
	const CString& GetName() const {return m_strName;}

	/**
	 * Dieser Funktion gibt die Beschreibung der Truppe zur�ck.
	 */
	const CString& GetDescription() const {return m_strDescription;}

	/**
	 * Diese Funktion gibt die Unterhaltskosten der Truppe zur�ck.
	 */
	BYTE GetMaintenanceCosts() const {return m_byMaintenanceCosts;}

	/**
	 * Diese Funktion gibt einen Zeiger auf die ben�tigten Techlevel dieser Einheit zur�ck.
	 */
	BYTE* GetNeededTechlevels() {return m_byNeededTechs;}

	/**
	 * Diese Funktion gibt ein ben�tigtes Techlevel zur�ck, welches man mit dem Paramter <code>tech</code>
	 * erfragt hat.
	 */
	BYTE GetNeededTechlevel(BYTE tech) const {return m_byNeededTechs[tech];}

	/**
	 * Diese Funktion gibt einen Zeiger auf die ben�tigten Ressourcen dieser Truppe zur�ck
	 */
	USHORT* GetNeededResources() {return m_iNeededResources;}

	/**
	 * Diese Funktion gibt die ben�tigte Industrie zum Bau dieser Einheit zur�ck.
	 */
	USHORT GetNeededIndustry() const {return m_iNeededIndustry;}

	/**
	 * Diese Funktion gibt die Truppengr��e, somit den ben�tigten Platz bei einem Transport zur�ck.
	 */
	USHORT GetSize() const {return m_iSize;}

	/**
	 * Diese Funktion gibt den Moralwert dieser Truppe zur�ck.
	 */
	BYTE GetMoralValue() const {return m_byMoralValue;}
	
	// Zugriffsfunktionen zum Schreiben der Membervariablen
	/**
	 * Diese Funktion setzt den Namen der Einheit.
	 */
	void SetName(const CString& name) {m_strName = name;}

	/**
	 * Diese Funktion setzt die Beschreibung der Truppe.
	 */
	void SetDescription(const CString& desc) {m_strDescription = desc;}

	/**
	 * Diese Funktion setzt die zum Bau ben�tigte Industrie der Truppe.
	 */
	void SetNeededIndustry(USHORT ip) {m_iNeededIndustry = ip;}

private:
	/// Der Name der Einheit
	CString m_strName;

	/// Die Beschreibung der Einheit
	CString m_strDescription;

	/// Diese Variable beinhaltet die Unterhaltskosten der Einheit
	BYTE m_byMaintenanceCosts;

	/// Dieses Feld beinhaltet die ben�tigten Forschungslevel zum Bau dieser Einheit
	BYTE m_byNeededTechs[6];

	/// Dieses Feld beinhaltet die ben�tigten Ressourcen zum Bau dieser Einheit
	USHORT m_iNeededResources[5];

	/// Die ben�tigte Industrie um eine Einheit fertigzustellen
	USHORT m_iNeededIndustry;

	/// Die Anzahl der Soldaten bzw. der ben�tigte Platz bei einem Transport f�r diese Einheit
	USHORT m_iSize;

	/// Der Moralwert der Truppe. Um dieses Wert wird die Moral bei unter 100% im System erh�ht bzw.
	/// bei �ber 100% in dem System verringert
	BYTE m_byMoralValue;
};
