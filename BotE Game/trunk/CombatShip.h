/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afx.h"
#include "Ship.h"
#include "Torpedo.h"

/**
* Eine Struktur f�r die Zeit wann wieder Beam- und Torpedowaffen abgefeuert werden k�nnen.
* Die Zeiten in dieser Struktur entsprechen nicht den wahren Sekunden. Ich habe zur Umrechnung
* 1sek == 10 Zeiteinheiten angesetzt. Somit bedeutet z.B. 5sek ein Wert von 50.
*/
struct ShootTime
{
	CArray<BYTE,BYTE> phaser;		///< Gibt an wann wir wieder die Phaserwaffe abfeuern k�nnen
	CArray<BYTE,BYTE> torpedo;		///< Gibt an wann wir wieder die Torpedowaffe abfeuern k�nnen
	BOOLEAN phaserIsShooting;		///< Feuert ein Phaser gerade?
};

class CCombatShip :	public CObject
{
	friend class CCombat;
public:
	/// Konstruktor
	CCombatShip(void);

	/// Destruktor
	~CCombatShip(void);

	/// Kopierkonstruktor
	CCombatShip(const CCombatShip & rhs);
	
	/// Zuweisungsoperatur
	CCombatShip & operator=(const CCombatShip &);

	/**
	 * Diese Funktion setzt die Man�vriebarkeit des Schiffes. Sie muss direkt nach anlegen des CombatSchiffes aufgerufen
	 * werden.
	 */
	void SetManeuverability(BYTE man) {m_byManeuverability = man;}

	/**
	 * Diese Funktion gibt den m�glichen Bonus durch die Man�vriebarkeit bei einem Angriff zur�ck. �bergeben m�ssen
	 * daf�r die Man�vrierbarkeit des Angreifers und die Man�vrierbarkeit des Verteidigers werden.
	 */
	static BYTE GetToHitBoni(BYTE Att, BYTE Def);

	/**
	 * Diese Funktion gibt den m�glichen Malus durch die Man�vriebarkeit bei einem Angriff zur�ck. �bergeben m�ssen
	 * daf�r die Man�vrierbarkeit des Angreifers und die Man�vrierbarkeit des Verteidigers werden.
	 */
	static BYTE GetToHitMali(BYTE Att, BYTE Def);

	/**
	 * Diese Funktion stellt die vorhandenen regenerativen Schilde auf die feindliche schilddurchschlagende Waffe ein.
	 */
	void ActRegShield();

	/**
	* Diese Funktion berechnet den n�chsten Punkt auf dem Weg zum Ziel, den das Schiff erreichen wird.
	*/
	void CalculateNextPosition();

	/**
	* Diese Funktion setzt das Schiff auf den n�chsten Punkt seiner vorher berechneten Flugroute.
	*/
	void GotoNextPosition();

	/**
	* Diese Funktion f�hrt einen Beamangriff gegen das Ziel durch, welches in der Variablen <code>m_pTarget<code>
	* gespeichert ist. Als Parameter wird dabei die Beamnummer �bergeben werden, ab welcher die Berechung durchgef�hrt
	* werden soll. Wenn w�hrend des Angriff das Ziel vernichtet wird, dann gibt die Funktion die aktuelle Beamnummer
	* zur�ck. Ansonsten gibt sie immer (-1/-1) zur�ck. Der R�ckgabewert ist eine Struktur mit 2 Variablen. Die erste
	* Variable gibt die Nummer des Beams im Feld an, die zweite die Nummer der Anzahl des Beams.
	*/
	CPoint AttackEnemyWithBeam(CPoint beamStart);

	/**
	* Diese Funktion f�hrt einen Torpedoangriff gegen das Ziel durch, welches in der Variablen <code>m_pTarget<code>
	* gespeichert ist. Als Parameter wird dabei ein Zeiger auf das Feld aller Torpedos im Kampf <code>CT<code>
	* �bergeben. Diese Funktion generiert dann automatisch die entsprechenden Torpedoobkekte und f�gt diese
	* in <code>CT<code> ein. Wenn w�hrend des Angriff das Ziel vernichtet wird, dann gibt die Funktion die 
	* aktuelle Torpedonummer zur�ck. Ansonsten gibt sie immer (-1/-1) zur�ck. Der R�ckgabewert ist eine Struktur
	* mit 2 Variablen. Die erste Variable gibt die Nummer der Art des Launchers im Feld an, die zweite
	* die Nummer der Anzahl dieses Launchers.
	*/
	CPoint AttackEnemyWithTorpedo(CombatTorpedos* CT, CPoint torpedoStart);

	/**
	* Diese Funktion gibt den Modifikator, den wir durch die Crewerfahrung erhalten zur�ck
	*/
	BYTE GetCrewExperienceModi();

	/**
	 * Diese Funktion gibt einen Wahrheitswert zur�ck, ob sich die Schilde schon auf eine schilddurchschlagende
	 * Waffe eingestellt haben.
	 */
	BOOLEAN GetActRegShields() const {return m_bRegShieldStatus;}
	
public:
	/// Zeiger auf das Schiff, welches hier im Kampf ist
	CShip*	m_pShip;
	
	/// Aktuelle Position (Koordinate im Raum) des Schiffes
	Position m_KO;					
	
	/// Flugroute des Schiffes, welche die folgenden Koordinaten beinhaltet
	CArray<Position,Position> m_Route;
	
	/// Zeit bis das Schiff wieder seine Waffen abfeuern kann. Wenn dieser Wert NULL erreicht hat, dann kann es
	/// die Waffen wieder feuern.
	ShootTime m_Fire;
	
	/// werden Pulsebeams geschossen? Nur f�r den CombatSimulator
	BOOLEAN m_bPulseFire;
	
	/// Die Man�vrierbarkeit des Schiffes im Kampf.
	BYTE m_byManeuverability;

	/// Die aktuelle Taktik des Schiffes
	BYTE m_Tactic;

	/// Der Schadens- und Verteidigungsbonus/malus der Schiffe der Rasse
	USHORT m_iModifier;

	/// Zeiger auf das Schiff, welches es als Ziel erfasst hat
	CCombatShip* m_pTarget;

	/// Status der regenerativen Schilde, angepa�t oder nicht 
	BOOLEAN m_bRegShieldStatus;

	/// Ist das Schiff noch getarnt oder nicht. Nach dem Feuern hat das Schiff noch 10 bis 20 Ticks Zeit,
	/// bis es wirklich angegriffen werden kann. Solange m_byCloak gr��er als NULL ist, ist das Schiff getarnt.
	BYTE m_byCloak;

	/// Hat das Schiff schonmal getarnt geschossen?
	BOOLEAN m_bShootCloaked;


};

typedef CArray<CCombatShip,CCombatShip> CombatShips;