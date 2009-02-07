#include "stdafx.h"
#include "GlobalStorage.h"

IMPLEMENT_SERIAL (CGlobalStorage, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CGlobalStorage::CGlobalStorage(void)
{
	this->Reset();
}

CGlobalStorage::~CGlobalStorage(void)
{
	m_ResOut.RemoveAll();
	m_ResIn.RemoveAll();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CGlobalStorage::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);

	m_ResIn.Serialize(ar);
	m_ResOut.Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_byPercentLosing;
		ar << m_iMaxTakeFromStorage;
		ar << m_iTakeFromStorage;
		for (int i = TITAN; i <= IRIDIUM; i++)
			ar << m_iRessourceStorages[i];
/*		ar << m_ResIn.GetSize();
		for (int i = 0; i < m_ResIn.GetSize(); i++)
			m_ResIn.GetAt(i).Serialize(ar);
		ar << m_ResOut.GetSize();
		for (int i = 0; i < m_ResOut.GetSize(); i++)
			m_ResOut.GetAt(i).Serialize(ar);*/
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int number = 0;
		StorageStruct ss;

		ar >> m_byPercentLosing;
		ar >> m_iMaxTakeFromStorage;
		ar >> m_iTakeFromStorage;
		for (int i = TITAN; i <= IRIDIUM; i++)
			ar >> m_iRessourceStorages[i];
/*		ar >> number;
		m_ResIn.RemoveAll();
		for (int i = 0; i < number; i++)
		{
			ss.Serialize(ar);
			m_ResIn.Add(ss);
		}
		ar >> number;
		m_ResOut.RemoveAll();
		for (int i = 0; i < number; i++)
		{
			ss.Serialize(ar);
			m_ResOut.Add(ss);
		}*/
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion gibt die Menge der Ressource <code>res</code> zur�ck, die von dem System mit der Koordinate
/// <code>ko</code> in das globale Lager kommen sollen.
USHORT CGlobalStorage::GetAddedResource(BYTE res, CPoint ko) const
{
	USHORT resIn = 0;
	for (int i = 0; i < m_ResIn.GetSize(); i++)
		if (m_ResIn.GetAt(i).ko == ko && m_ResIn.GetAt(i).res == res)
		{
			resIn += m_ResIn.GetAt(i).resTransfer;
			break;
		}
	return resIn;
}

/// Diese Funktion gibt die Menge der Ressource <code>res</code> zur�ck, die in das System mit der Koordinate
/// <code>ko</code> aus dem globale Lager kommen sollen.
USHORT CGlobalStorage::GetSubResource(BYTE res, CPoint ko) const
{
	USHORT resOut = 0;
	for (int i = 0; i < m_ResOut.GetSize(); i++)
		if (m_ResOut.GetAt(i).ko == ko && m_ResOut.GetAt(i).res == res)
		{
			resOut += m_ResOut.GetAt(i).resTransfer;
			break;
		}
	return resOut;
}

/// Diese Funktion gibt die gesamte Menge der Ressource <code>res</code> zur�ck, die in das globale Lager kommen soll.
USHORT CGlobalStorage::GetAllAddedResource(BYTE res) const
{
	USHORT resIn = 0;
	for (int i = 0; i < m_ResIn.GetSize(); i++)
		if (m_ResIn.GetAt(i).res == res)
			resIn += m_ResIn.GetAt(i).resTransfer;
	return resIn;
}

/// Diese Funktion gibt die gesamte Menge der Ressource <code>res</code> zur�ck, die aus dem globalen Lager entfernt
/// werden soll.
USHORT CGlobalStorage::GetAllSubResource(BYTE res) const
{
	USHORT resOut = 0;
	for (int i = 0; i < m_ResOut.GetSize(); i++)
		if (m_ResOut.GetAt(i).res == res)
			resOut += m_ResOut.GetAt(i).resTransfer;
	return resOut;
}

/// Diese Funktion gibt <code>TRUE</code> zur�ck, wenn sich irgendeine Ressource im globalen Lager befindet.
BOOLEAN CGlobalStorage::IsFilled() const
{
	for (int i = TITAN; i <= IRIDIUM; i++)
		if (m_iRessourceStorages[i] > NULL)
			return TRUE;
	return FALSE;
}

/// Diese Funktion addiert die mit dem Parameter <code>add</code> �bergebende Anzahl der Ressource
/// <code>res</code> zum Lagerinhalt. Im Parameter <code>ko</code> wird die Systemkoordinate �bergeben,
/// von wo aus man die Transaktion f�hrt. Der R�ckgabewert der Funktion ist eine Menge, einer
/// Ressource, die in der selben Runde schon aus dem globalen Lager in das System kommen soll.
USHORT CGlobalStorage::AddRessource(USHORT add, BYTE res, CPoint ko)
{
	USHORT getBack = 0;
	// Zu Beginn in dem Feld suchen, ob man Ressourcen aus dem globalen Lager in der neuen Runde ins System
	// verschieben will. Diese werden zuerst entfernt.
	for (int i = 0; i < m_ResOut.GetSize(); i++)
		if (m_ResOut.GetAt(i).ko == ko && m_ResOut.GetAt(i).res == res)
		{
			// Wenn die Addition kleiner als die Menge der aus dem Lager gehenden Ressourcen ist
			if (add < m_ResOut.GetAt(i).resTransfer)
			{
				m_ResOut.GetAt(i).resTransfer -= add;
				m_iTakeFromStorage -= add;
				return add;
			}
			// Wenn die Addition gleich der Menge der aus dem Lager gehenden Ressourcen ist
			else if (add == m_ResOut.GetAt(i).resTransfer)
			{
				m_ResOut.RemoveAt(i--);
				m_iTakeFromStorage -= add;
				return add;
			}
			// Wenn die Addition gr��er der Menge der aus dem Lager gehenden Ressourcen ist
			else
			{
			/*	add -= m_ResOut.GetAt(i).resTransfer;
				getBack = m_ResOut.GetAt(i).resTransfer;
				m_iTakeFromStorage -= getBack;
				m_ResOut.RemoveAt(i--);
				break;*/
				m_iTakeFromStorage -= m_ResOut.GetAt(i).resTransfer;
				m_ResOut.RemoveAt(i--);
				return add;
			}
		}

	// Zuerst mal schauen, ob dieses System mit der Koordinate ko schon im Feld vorhanden ist. Wenn
	// dies der Fall ist, dann den Ressourceneintrag in diesem Feld erh�hen
	for (int i = 0; i < m_ResIn.GetSize(); i++)
		if (m_ResIn.GetAt(i).ko == ko && m_ResIn.GetAt(i).res == res)
		{
			m_ResIn.GetAt(i).resTransfer += add;
			return getBack;
		}
	// Hat man das System noch nicht in dem Feld, dann neuen Eintrag erstellen
	StorageStruct ss;
	ss.ko = ko; ss.res = res; ss.resTransfer = add;
	m_ResIn.Add(ss);
	return getBack;
}

/// Diese Funktion subtrahiert die mit dem Parameter <code>sub</code> �bergebende Anzahl der Ressource
/// <code>res</code> vom Lagerinhalt. Im Parameter <code>ko</code> wird die Systemkoordinate �bergeben, von wo aus
/// mam die Transaktion f�hrt. Dabei beachtet die Funktion, dass nicht schon zuviel aus dem Lager genommen
/// wurde und das der Lagerinhalt nicht negativ werden kann. Der R�ckgabewert der Funktion ist eine Menge, einer
/// Ressource, die in der selben Runde schon aus dem gleichen System addiert wurde. Somit kann man die Waren
/// auch sofort wieder herausnehmen.
USHORT CGlobalStorage::SubRessource(USHORT sub, BYTE res, CPoint ko)
{
	USHORT getBack = 0;
	// Wenn man schon aus diesem System Ressourcen ins globale Lager verschieben m�chte, so werden erstmal diese
	// Ressourcen abgezogen. Der Rest wird dann wirklich aus dem globalen Lager genommen.
	for (int i = 0; i < m_ResIn.GetSize(); i++)
		if (m_ResIn.GetAt(i).ko == ko && m_ResIn.GetAt(i).res == res)
		{
			// Wenn die Subtraktion kleiner als die Menge der in das Lager gehenden Ressourcen ist
			if (sub < m_ResIn.GetAt(i).resTransfer)
			{
				m_ResIn.GetAt(i).resTransfer -= sub;
				return sub;
			}
			// Wenn die Subtraktion gleich der Menge in das Lager gehenden Ressourcen ist
			else if (sub == m_ResIn.GetAt(i).resTransfer)
			{
				m_ResIn.RemoveAt(i--);
				return sub;
			}
			// Wenn die Subtraktion gr��er der Menge in das Lager gehenden Ressourcen ist
			else
			{
				sub -= m_ResIn.GetAt(i).resTransfer;
				getBack = m_ResIn.GetAt(i).resTransfer;
				m_ResIn.RemoveAt(i--);
				break;
			}
		}
	// �berpr�fen, dass nicht zuviel Ressourcen aus dem Lager in dieser Runde genommen werden k�nnen
	if ((m_iTakeFromStorage + sub) > m_iMaxTakeFromStorage)
		sub = m_iMaxTakeFromStorage - m_iTakeFromStorage;
	// �berpr�fen, dass man nicht mehr Ressourcen aus dem Lager nehmen kann, als darin vorhanden sind
	if ((UINT)(sub + this->GetAllSubResource(res)) > m_iRessourceStorages[res])
		sub = m_iRessourceStorages[res] - this->GetAllSubResource(res);
	if (sub == 0)
		return getBack;
	m_iTakeFromStorage += sub;
	// Zuerst mal schauen, ob dieses System mit der Koordinate ko schon im Feld vorhanden ist. Wenn
	// dies der Fall ist, dann den Ressourceneintrag in diesem Feld erh�hen
	for (int i = 0; i < m_ResOut.GetSize(); i++)
		if (m_ResOut.GetAt(i).ko == ko && m_ResOut.GetAt(i).res == res)
		{
			m_ResOut.GetAt(i).resTransfer += sub;
			return getBack;
		}
	// Hat man das System noch nicht in dem Feld, dann neuen Eintrag erstellen
	StorageStruct ss;
	ss.ko = ko; ss.res = res; ss.resTransfer = sub;
	m_ResOut.Add(ss);
	return getBack;
}

/// Diese Funktion f�hrt am Lagerinhalt alle m�glichen �nderungen durch, die bei jeder neuen Runde eintreten
/// k�nnen. Dabei f�llt sie auch die Lager der entsprechenden Systeme.
void CGlobalStorage::Calculate(CSystem systems[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT])
{
	m_iTakeFromStorage = 0;
	// zuerst werden die Ressourcen aus dem globalen Lager in die Systeme �bertragen
	for (int i = 0; i < m_ResOut.GetSize(); )
	{
		// Sicherheitsabfrage, falls man doch durch irgendeinen Zufall mal mehr aus dem Lager nehmen w�rde als
		// darin vorhanden ist
		if (m_iRessourceStorages[m_ResOut.GetAt(i).res] >= m_ResOut.GetAt(i).resTransfer)
		{
			systems[m_ResOut.GetAt(i).ko.x][m_ResOut.GetAt(i).ko.y].SetRessourceStore(m_ResOut.GetAt(i).res, m_ResOut.GetAt(i).resTransfer);
			m_iRessourceStorages[m_ResOut.GetAt(i).res] -= m_ResOut.GetAt(i).resTransfer;
		}
		else
		{
			systems[m_ResOut.GetAt(i).ko.x][m_ResOut.GetAt(i).ko.y].SetRessourceStore(m_ResOut.GetAt(i).res, m_iRessourceStorages[m_ResOut.GetAt(i).res]);
			m_iRessourceStorages[m_ResOut.GetAt(i).res] = 0;
		}
		m_ResOut.RemoveAt(i);
	}
	// dann wird der Inhalt des globalen Lagers mit den Ressourcen gef�llt, die aus den Systemen kommen
	for (int i = 0; i < m_ResIn.GetSize(); )
	{
		m_iRessourceStorages[m_ResIn.GetAt(i).res] += m_ResIn.GetAt(i).resTransfer;
		m_ResIn.RemoveAt(i);
	}
	// zuletzt verschwindet ein gewisser Anteil aus dem globalen Lager. Von jeder Ressource gehen "m_iPercentLosing"
	// Prozent verloren
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_iRessourceStorages[i] -= m_iRessourceStorages[i] * m_byPercentLosing / 100;
}

/// Resetfunktion f�r die Klasse CGlobalStorage, welche alle Werte wieder auf Ausgangswerte setzt.
void CGlobalStorage::Reset()
{
	for (int i = TITAN; i <= IRIDIUM; i++)
		m_iRessourceStorages[i] = 0;
	m_byPercentLosing = 15;
	m_iTakeFromStorage = 0;
	m_iMaxTakeFromStorage = 1000;
	m_ResOut.RemoveAll();
	m_ResIn.RemoveAll();
}