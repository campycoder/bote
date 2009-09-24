/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "Message.h"
#include "Research.h"
#include "Intelligence.h"
#include "GlobalStorage.h"
#include "EventColonization.h"
#include "EventBlockade.h"
#include "EventBombardment.h"

/**
 * Struktur, die die wichtigsten Informationen eines Systems aufnehmen kann. Dies wird ben�tigt, wenn wir in einer
 * View eine Liste der Systeme aufnehmen m�chten. Ich m�chte da nur die wichtigsten Infos haben
 */ 
struct SystemViewStruct {
		bool operator< (const SystemViewStruct& elem2) const { return name < elem2.name;}
		bool operator> (const SystemViewStruct& elem2) const { return name > elem2.name;}

		SystemViewStruct operator=(const SystemViewStruct &other) {name = other.name; ko = other.ko; return *this;}
		SystemViewStruct(CString _name, CPoint _ko) : name(_name), ko(_ko) {};
		SystemViewStruct() : name(""), ko(CPoint(-1,-1)) {};

		CString name;
		CPoint ko;
	};

class CSystem;
class CSector;
class CEmpire : public CObject  
{
public:
	DECLARE_SERIAL (CEmpire)
	
	/// Konstruktor
	CEmpire();
	
	/// Destruktor
	~CEmpire();

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);
	
	// Zugriffsfunktionen
	// zum Lesen der Membervariablen
	/// Funktion gibt die Anzahl der Systeme des Imperiums zur�ck.
	BYTE GetNumberOfSystems() const {return m_byNumberOfSystems;}

	/// Funktion gibt einen Zeiger auf die Liste der zum Imperium geh�renden Systeme zur�ck.
	CArray<SystemViewStruct>* GetSystemList() {return &m_SystemList;}
	
	/// Funktion gibt die Nummer des Imperiums zur�ck.
	const CString& GetEmpireID() const {return m_sEmpireID;} 
	
	/// Funktion gibt den aktuellen Bestand an Latinum zur�ck.
	long GetLatinum() const {return m_iLatinum;}
	
	/// Funktion gibt die gesamte Latinum�nderung seit der letzten Runde zur�ck.
	long GetLatinumChange() const {return m_iLatinumChange;}
	
	/// Funktion gibt die Schiffsunterst�tzungskosten zur�ck.
	USHORT GetShipCosts() const {return m_iShipCosts;}
	
	/// Funktion gibt die Schiffunterst�tzungkosten durch die Bev�lkerung zur�ck.
	USHORT GetPopSupportCosts() const {return m_iPopSupportCosts;}
	
	/// Funktion gibt die aktuell produzierten Forschungspunkte zur�ck.
	UINT GetFP() const {return m_lFP;}

	/// Funktion gibt die aktuell produtierten Geheimdienstpunkte zur�ck.
	UINT GetSP() const {return m_Intelligence.GetSecurityPoints();}

	/// Funktion gibt ein Feld mit der Menge aller Ressourcen auf allen Systemen des Imperiums zur�ck.
	const UINT* GetStorage() const {return m_lResourceStorages;}
	
	/// Funktion gibt einen Zeiger auf alle Nachrichten an das Imperium zur�ck.
	MessageArray* GetMessages() {return &m_Messages;}

	/// Funktion gibt einen Zeiger auf das Feld mit den Eventnachrichten f�r das Imperium zur�ck.
	/// @return Pointer auf <code>CObArray</code>
	CObArray* GetEventMessages() {return &m_EventMessages;}

	/// Funktion gibt einen Zeiger auf das Forschungsobjekt des Imperiums zur�ck.
	CResearch* GetResearch(void) {return &m_Research;}
	
	/// Funktion gibt einen Zeiger auf das Geheimdienstobjekt des Imperiums zur�ck.
	CIntelligence* GetIntelligence(void) {return &m_Intelligence;}

	/// Funktion liefert einen Zeiger auf das globale Lager eines Imperiums.
	CGlobalStorage* GetGlobalStorage(void) {return &m_GlobalStorage;}
	
	// zum Schreiben der Membervariablen
	/// Funktion addiert die im Parameter <code>add</code> �bergebene Menge zu der Anzahl der Systeme
	/// des Imperiums.
	/// @param n Anzahl der Systene
	void SetNumberOfSystems(BYTE n) {m_byNumberOfSystems = n;}

	/// Funktion legt die zugeh�rige Rassennummer des Imperiums fest.
	/// @param empireNumber Rassennummer
	void SetEmpireID(const CString& sRaceID) {m_sEmpireID = sRaceID; m_Intelligence.SetRaceID(sRaceID);}

	/// Funktion addiert die �bergebene Menge zum Latinumbestand.
	/// @param add Anzahl des zu addierenden Latinums
	void SetLatinum(int add) {m_iLatinum += add;}

	/// Funktion legt die Latinum�nderung zur vorherigen Runde fest.
	/// @param change �nderung des Latinums gegn�ber der vorherigen Runde.
	void SetLatinumChange(int change) {m_iLatinumChange = change;}
	
	/// Funktion setzt die Schiffsunterst�tzungskosten auf den Wert von <code>costs</code>.
	void SetShipCosts(USHORT costs) {m_iShipCosts = costs;}

	/// Funktion addiert die Schiffsunterst�tzungskosten, �bergeben durch den Parameter <code>add</code>, zu den
	/// aktuellen Schiffsunterst�tzungskosten des Imperiums.
	void AddShipCosts(USHORT add) {m_iShipCosts += add;}

	/// Funktion setzt die Bev�lkerungsunterst�tzungskosten auf den Wert von <code>supportCosts</code>.
	void SetPopSupportCosts(USHORT supportCosts) {m_iPopSupportCosts = supportCosts;}

	/// Funktion addiert die Bev�lkerungsunterst�tzungskosten, �bergeben durch den Parameter <code>add</code>, zu den
	/// aktuellen Bev�lkerungsunterst�tzungskosten des Imperiums.
	void AddPopSupportCosts(USHORT add) {m_iPopSupportCosts += add;}

	/// Funktion addiert die �bergebene Anzahl an Forschungspunkten zu den Forschungspunkten des Imperiums.
	/// @param add Anzahl der zu addierenden Forschungspunkte
	void AddFP(int add);

	/// Funktion addiert die �bergebene Anzahl an Geheimdienstpunkten zu den Geheimdienstpunkten des Imperiums.
	/// @param add Anzahl der zu addierenden Geheimdienstpunkte
	void AddSP(UINT add) {m_Intelligence.AddSecurityPoints(add);}

	/// Funktion addiert die �bergebene Menge an Ressourcen zum zugeh�rigen Ressourcenlager.
	/// @param res Ressource
	/// @param add zu addierende Menge
	void SetStorageAdd(USHORT res, UINT add) {m_lResourceStorages[res] += add;}

	/// Funktion f�gt eine �bergebene Nachricht dem Nachrichtenfeld des Imperiums hinzu.
	/// @param message Nachricht
	void AddMessage(const CMessage &message) {m_Messages.Add(message);}

	// Sonstige Funktionen
	/// Resetfunktion f�r das CEmpire-Objekt.
	void Reset(void);
	
	/// Funktion generiert die Liste der Systeme f�r das Imperium anhand aller Systeme.
	/// @param systems Feld aller Systeme
	void GenerateSystemList(const CSystem systems[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT], const CSector sectors[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT]);

	/// Funktion setzt die Lager und aktuell prodzuierten Forschungs- und Geheimdienstpunkte wieder auf NULL
	/// damit man diese sp�ter wieder f�llen kann. Die wirklichen Lager und Punkte in den einzelnen Systemen
	/// werden dabei nicht anger�hrt
	void ClearAllPoints(void);

	/// Funktion l�scht alle Nachrichten und Antworten an das Imperiums.
	void ClearMessagesAndEvents(void);

private:
	long m_iLatinum;				///< Latinum des Imperiums
	
	long m_iLatinumChange;			///< Gewinn bzw Latinumverlust zur letzten Runde
	
	USHORT m_iShipCosts;			///< die Schiffsunterst�tzungskosten des Imperiums
	
	USHORT m_iPopSupportCosts;		///< Unterst�tzungskosten aufgrund der Bev�lkerung
	
	UINT m_lFP;						///< aktuelle FP des Imperiums

	UINT m_lResourceStorages[DILITHIUM+1];	///< die gesamte Menge auf allen Systemen der jeweiligen Ressource
	
	CString m_sEmpireID;			///< gibt die ID der Rasse des Imperiums zur�ck
	
	BYTE m_byNumberOfSystems;		///< Anzahl Systeme des Imperiums
	
	MessageArray m_Messages;		///< alle Nachrichten an das Imperium

	CObArray m_EventMessages;		///< alle Events f�r das Imperium
	
	CResearch m_Research;			///< die Forschung des Imperiums

	CIntelligence m_Intelligence;	///< der Geheimdienst des Imperiums

	CGlobalStorage m_GlobalStorage;	///< das globale Lager des Imperiums

	CArray<SystemViewStruct> m_SystemList;	///< Zeiger auf die zum Imperium geh�renden Systeme	
};