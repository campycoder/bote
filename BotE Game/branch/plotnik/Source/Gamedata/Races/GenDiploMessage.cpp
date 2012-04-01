// GenDiploMessage.cpp: Implementierung der Klasse CGenDiploMessage.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "GenDiploMessage.h"
#include "Botf2Doc.h"
#include "RaceController.h"
#include <vector>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CGenDiploMessage::CGenDiploMessage(void)
{
}

CGenDiploMessage::~CGenDiploMessage(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Statische Funktion zum Erzeugen eines Angebotstextes f�r ein diplomatisches Angebot
/// einer Majorrace.
/// @param Diplomatieobjekt, in das die Textinformationen geschrieben werden
/// @return <code>true</code> wenn alles geklappt hat, sonst <code>false</code>
bool CGenDiploMessage::GenerateMajorOffer(CDiplomacyInfo& info)
{	
	CString read[13] = {""};	// Anzahl der ganzen verschiedenen Nachrichten + 1
	
	CString search;
	search.Format("%s:", info.m_sFromRace);
	search.MakeUpper();
	
	CString csInput;											// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorsDiploOffers.data";	// Name des zu �ffnenden Files 
	CStdioFile file;											// Varibale vom Typ CStdioFile
	
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))	// Datei wird ge�ffnet
	{
		int i = 0;
		while (file.ReadString(csInput))
		{
			// Wenn wir in dem File die Rasse gefunden haben ("RASSENID:" gefunden!)
			if (csInput == search || i > 0)
			{
				read[i] = csInput;
				i++;
			}
			if (i == 13)
				break;
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"MajorsDiploOffers.data\" kann nicht ge�ffnet werden...");
		return false;
	}
	if (read[0] == "")
	{
		CString s;
		s.Format("Fehler in Datei! Konnte %s nicht in MajorDiploOffers.data finden", search);
		AfxMessageBox(s);
		return false;
	}
	// Datei wird geschlossen
	file.Close();

	// Jetzt die einzelnen Werte aus dem read-Feld in die Membervariablen schreiben
	CString sOffer = "";
	CString sHeadline = "";
	switch (info.m_nType)
	{
		case TRADE_AGREEMENT:
		{
			sHeadline = CResourceManager::GetString("TRADE_AGREEMENT_OFFER");
			sOffer = read[1];
			break;
		}
		case FRIENDSHIP_AGREEMENT:
		{
			sHeadline = CResourceManager::GetString("FRIENDSHIP_OFFER");
			sOffer = read[2];
			break;
		}
		case COOPERATION:
		{
			sHeadline = CResourceManager::GetString("COOPERATION_OFFER");
			sOffer = read[3];
			break;
		}
		case AFFILIATION:
		{
			sHeadline = CResourceManager::GetString("AFFILIATION_OFFER");
			sOffer = read[4];
			break;
		}
		case MEMBERSHIP:
		{
			sHeadline = CResourceManager::GetString("MEMBERSHIP_OFFER");
			sOffer = read[5];
			break;
		}
		case NON_AGGRESSION_PACT:
		{
			sHeadline = CResourceManager::GetString("NON_AGGRESSION_OFFER");
			sOffer = read[6];
			break;
		}
		case DEFENCE_PACT:
		{
			sHeadline = CResourceManager::GetString("DEFENCE_PACT_OFFER");
			sOffer = read[7];
			break;
		}
		case WAR_PACT:
		{
			sHeadline = CResourceManager::GetString("WAR_PACT_OFFER");
			sOffer = read[8];

			CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
			ASSERT(pDoc);

			CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(info.m_sWarpactEnemy));
			if (pMajor)
				sOffer.Replace("$enemy$", pMajor->GetEmpireNameWithArticle());
			break;
		}
		case WAR:
		{
			sHeadline = CResourceManager::GetString("WAR_OFFER");
			sOffer = read[9];
			break;
		}
		case CORRUPTION:
		{
			sHeadline = CResourceManager::GetString("CORRUPTION");
			CString temp;
			temp.Format("%d", info.m_nCredits);

			// bei Bestechung einer Minorrace
			if (info.m_sCorruptedRace != "")
				sOffer = read[10];

			sOffer.Replace("$credits$",temp);
			break;
		}
		case DIP_REQUEST:
		{
			sHeadline = CResourceManager::GetString("REQUEST");
			CString temp;
			temp.Format("%d", info.m_nCredits);
			
			sOffer = read[11];   // bei Forderung an eine andere Majorrace
			CString res = "";
			if (info.m_nResources[TITAN] > 0)
			{
				CString s; s.Format("%d",info.m_nResources[TITAN]);
				res = CResourceManager::GetString("TITAN_REQUEST", FALSE, s);
			}
			else if (info.m_nResources[DEUTERIUM] > 0)
			{
				CString s; s.Format("%d",info.m_nResources[DEUTERIUM]);
				res = CResourceManager::GetString("DEUTERIUM_REQUEST", FALSE, s);
			}
			else if (info.m_nResources[DURANIUM] > 0)
			{
				CString s; s.Format("%d",info.m_nResources[DURANIUM]);
				res = CResourceManager::GetString("DURANIUM_REQUEST", FALSE, s);
			}
			else if (info.m_nResources[CRYSTAL] > 0)
			{
				CString s; s.Format("%d",info.m_nResources[CRYSTAL]);
				res = CResourceManager::GetString("CRYSTAL_REQUEST", FALSE, s);
			}
			else if (info.m_nResources[IRIDIUM] > 0)
			{
				CString s; s.Format("%d",info.m_nResources[IRIDIUM]);
				res = CResourceManager::GetString("IRIDIUM_REQUEST", FALSE, s);
			}
			else if (info.m_nResources[DERITIUM] > 0)
			{
				CString s; s.Format("%d",info.m_nResources[DERITIUM]);
				res = CResourceManager::GetString("DERITIUM_REQUEST", FALSE, s);
			}
			sOffer.Replace("$ressource$",res);
			
			sOffer.Replace("$credits$",temp);
			break;
		}
		case PRESENT:
		{
			sHeadline = CResourceManager::GetString("PRESENT");
			sOffer = read[12];
			CString temp;
			temp.Format("%d", info.m_nCredits);
			sOffer.Replace("$credits$",temp);
			break;
		}
		default:
			return false;
	}

	// Wenn Credits mit dazugegeben wurden
	if (info.m_nCredits > 0 && info.m_nType != PRESENT && info.m_nType != DIP_REQUEST && info.m_nType != CORRUPTION)
	{
		CString temp = sOffer;
		CString s;
		s.Format("%d", info.m_nCredits);
		sOffer = CResourceManager::GetString("EXTRA_CREDITS", FALSE, temp, s);
	}
	
	// Wenn wir eine Ressource mit dazugeben und es sich nicht um eine Forderung handelt
	if (info.m_nType != DIP_REQUEST)
	{
		if (info.m_nResources[TITAN] > 0)
		{
			CString s; s.Format("%d",info.m_nResources[TITAN]);
			sOffer = CResourceManager::GetString("EXTRA_TITAN", FALSE, sOffer, s);
		}
		else if (info.m_nResources[DEUTERIUM] > 0)
		{
			CString s; s.Format("%d",info.m_nResources[DEUTERIUM]);
			sOffer = CResourceManager::GetString("EXTRA_DEUTERIUM", FALSE, sOffer, s);
		}
		else if (info.m_nResources[DURANIUM] > 0)
		{
			CString s; s.Format("%d",info.m_nResources[DURANIUM]);
			sOffer = CResourceManager::GetString("EXTRA_DURANIUM", FALSE, sOffer, s);
		}
		else if (info.m_nResources[CRYSTAL] > 0)
		{
			CString s; s.Format("%d",info.m_nResources[CRYSTAL]);
			sOffer = CResourceManager::GetString("EXTRA_CRYSTAL", FALSE, sOffer, s);
		}
		else if (info.m_nResources[IRIDIUM] > 0)
		{
			CString s; s.Format("%d",info.m_nResources[IRIDIUM]);
			sOffer = CResourceManager::GetString("EXTRA_IRIDIUM", FALSE, sOffer, s);
		}
		else if (info.m_nResources[DERITIUM] > 0)
		{
			CString s; s.Format("%d",info.m_nResources[DERITIUM]);
			sOffer = CResourceManager::GetString("EXTRA_DERITIUM", FALSE, sOffer, s);
		}
	}
	if (info.m_nType != CORRUPTION && info.m_nType != PRESENT && info.m_nType != DIP_REQUEST && info.m_nType != WAR && info.m_nType != WAR_PACT)
	{
		CString temp;
		if (info.m_nDuration == 0)
			temp = CResourceManager::GetString("UNLIMITED_CONTRACT_DURATION", FALSE, sOffer);
		else
		{
			CString s; s.Format("%d", info.m_nDuration);	
			temp = CResourceManager::GetString("LIMITED_CONTRACT_DURATION", FALSE, sOffer, s);
		}
		sOffer = temp;		
	}

	info.m_sHeadline = sHeadline;
	info.m_sText = sOffer;		
	return true;
}

/// Statische Funktion zum Erzeugen eines Antworttextes auf ein diplomatisches Angebot.
/// @param Diplomatieobjekt, in das die Textinformationen geschrieben werden
/// @return <code>true</code> wenn alles geklappt hat, sonst <code>false</code>
bool CGenDiploMessage::GenerateMajorAnswer(CDiplomacyInfo& info)
{
	CString read[18] = {""};	// Anzahl der ganzen verschiedenen Nachrichten + 1
	
	CString search;
	search.Format("%s:", info.m_sToRace);
	search.MakeUpper();
	
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MajorsDiploAnswers.data";	// Name des zu �ffnenden Files 
	CStdioFile file;													// Varibale vom Typ CStdioFile
	
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird ge�ffnet
	{
		int i = 0;
		CString csInput;
		while (file.ReadString(csInput))
		{
			if (csInput == search || i > 0)	// Wenn wir in dem File die Rasse gefunden haben ("RASSENNAME:" gefunden!)
			{
				read[i] = csInput;
				i++;
			}
			if (i == 18)
				break;
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"MajorsDiploAnswers.data\" kann nicht ge�ffnet werden...");
		exit(1);
	}
	
	if (read[0] == "")
	{
		CString s;
		s.Format("Fehler in Datei! Konnte %s nicht in MajorDiploAnswers.data finden", search);
		AfxMessageBox(s);
	}
	
	// Datei wird geschlossen
	file.Close();

	// Jetzt die einzelnen Werte aus dem read-Feld in die Membervariablen schreiben
	CString sAnswer = 0;
	switch (info.m_nType)
	{
	case TRADE_AGREEMENT:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[1];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_TRADE_AGREEMENT");
			}
			else
			{
				info.m_sHeadline = CResourceManager::GetString("DECLINE_TRADE_AGREEMENT");
				sAnswer = read[9];
			}
			break;
		}
	case FRIENDSHIP_AGREEMENT:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[2];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_FRIENDSHIP");
			}
			else
			{
				sAnswer = read[10];
				info.m_sHeadline = CResourceManager::GetString("DECLINE_FRIENDSHIP");
			}
			break;
		}
	case COOPERATION:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[3];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_COOPERATION");
			}
			else
			{
				sAnswer = read[11];
				info.m_sHeadline = CResourceManager::GetString("DECLINE_COOPERATION");
			}
			break;
		}
	case AFFILIATION:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[4];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_AFFILIATION");
			}
			else
			{
				sAnswer = read[12];
				info.m_sHeadline = CResourceManager::GetString("DECLINE_AFFILIATION");
			}
			break;
		}
	case NON_AGGRESSION_PACT:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[5];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_NON_AGGRESSION");
			}
			else
			{
				sAnswer = read[13];
				info.m_sHeadline = CResourceManager::GetString("DECLINE_NON_AGGRESSION");
			}
			break;
		}
	case DEFENCE_PACT:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[6];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_DEFENCE_PACT");
			}
			else
			{
				sAnswer = read[14];
				info.m_sHeadline = CResourceManager::GetString("DECLINE_DEFENCE_PACT");
			}
			break;
		}
	case WAR_PACT:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[7];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_WARPACT");
			}
			else
			{
				sAnswer = read[15];
				info.m_sHeadline = CResourceManager::GetString("DECLINE_WARPACT");
			}
			break;
		}
	case DIP_REQUEST:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[8];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_REQUEST");
			}
			else
			{
				sAnswer = read[16];
				info.m_sHeadline = CResourceManager::GetString("DECLINE_REQUEST");
			}
			break;
		}
	case WAR:
		{
			sAnswer = read[17];
			info.m_sHeadline = CResourceManager::GetString("WAR_OFFER");
			break;
		}
	}

	info.m_nFlag = DIPLOMACY_ANSWER;
	info.m_sText = sAnswer;
	// Sender - Empf�nger vertauschen, da es nun eine Antwort ist
	CString sOldToRace = info.m_sToRace;
	info.m_sToRace = info.m_sFromRace;
	info.m_sFromRace = sOldToRace;

	return true;
}

/// Statische Funktion zum Erzeugen eines Angebotstextes f�r ein diplomatisches Angebot
/// einer Minorrace.
/// @param Diplomatieobjekt, in das die Textinformationen geschrieben werden
/// @return <code>true</code> wenn alles geklappt hat, sonst <code>false</code>
bool CGenDiploMessage::GenerateMinorOffer(CDiplomacyInfo& info)
{
	// Dokument holen
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	CRace* pRace = pDoc->GetRaceCtrl()->GetRace(info.m_sFromRace);
	if (!pRace)
		return false;

	std::vector<CString> vSearchText;
	if (pRace->IsRaceProperty(FINANCIAL))
		vSearchText.push_back("FINANCIAL:");
	if (pRace->IsRaceProperty(WARLIKE))
		vSearchText.push_back("WARLIKE:");
	if (pRace->IsRaceProperty(AGRARIAN))
		vSearchText.push_back("FARMER:");
	if (pRace->IsRaceProperty(INDUSTRIAL))
		vSearchText.push_back("INDUSTRIAL:");
	if (pRace->IsRaceProperty(SECRET))
		vSearchText.push_back("SECRET:");
	if (pRace->IsRaceProperty(SCIENTIFIC))
		vSearchText.push_back("RESEARCHER:");
	if (pRace->IsRaceProperty(PRODUCER))
		vSearchText.push_back("PRODUCER:");
	if (pRace->IsRaceProperty(PACIFIST))
		vSearchText.push_back("PACIFIST:");
	if (pRace->IsRaceProperty(SNEAKY))
		vSearchText.push_back("SNEAKY:");
	if (pRace->IsRaceProperty(SOLOING))
		vSearchText.push_back("SOLOING:");
	if (pRace->IsRaceProperty(HOSTILE))
		vSearchText.push_back("HOSTILE:");

	if (vSearchText.size() == 0)
		vSearchText.push_back("NOTHING_SPECIAL:");
	
	CString read[7];	// Anzahl der ganzen verschiedenen Nachrichten + 1
	CString search;
	search.Format("%s:", info.m_sFromRace);
	search.MakeUpper();
	// aus den ganzen Rasseneigenschaften eine zuf�llige aussuchen (falls mehrere vorhanden sind)
	CString search2 = vSearchText[rand()%vSearchText.size()];
	
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MinorsDiploOffers.data";	// Name des zu �ffnenden Files 
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird ge�ffnet
	{		
		int i = 0;
		CString sInput;	// auf csInput wird die jeweilige Zeile gespeichert
		while (file.ReadString(sInput))
		{
			// Wenn wir in dem File die Rasse gefunden haben ("RASSENNAME:" gefunden
			// oder ("RASSENTYP:" gefunden!)
			if (sInput == search || sInput == search2 || i > 0)
			{
				read[i] = sInput;
				i++;
			}
			if (i == 7)
				break;
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"MinorsDiploOffers.data\" kann nicht ge�ffnet werden...");
		return false;
	}
	// Datei wird geschlossen
	file.Close();
	
	// Jetzt die einzelnen Werte aus dem read-Feld in die Membervariablen schreiben
	CString sOfferText = "";
	CString sHeadline = "";
	switch (info.m_nType)
	{
	case TRADE_AGREEMENT:
		{
			sHeadline = CResourceManager::GetString("TRADE_AGREEMENT_OFFER");
			sOfferText = read[1];
			break;
		}
	case FRIENDSHIP_AGREEMENT:
		{
			sHeadline = CResourceManager::GetString("FRIENDSHIP_OFFER");
			sOfferText = read[2];
			break;
		}
	case COOPERATION:
		{
			sHeadline = CResourceManager::GetString("COOPERATION_OFFER");
			sOfferText = read[3];
			break;
		}
	case AFFILIATION:
		{
			sHeadline = CResourceManager::GetString("AFFILIATION_OFFER");
			sOfferText = read[4];
			break;
		}
	case MEMBERSHIP:
		{
			sHeadline = CResourceManager::GetString("MEMBERSHIP_OFFER");
			sOfferText = read[5];
			break;
		}
	case WAR:
		{
			sHeadline = CResourceManager::GetString("WAR_OFFER");
			sOfferText = read[6];
			break;
		}
	}

	info.m_sHeadline = sHeadline;
	info.m_sText = sOfferText;
	return true;
}

/// Statische Funktion zum Erzeugen eines Antworttextes auf ein diplomatisches Angebot.
/// @param Diplomatieobjekt, in das die Textinformationen geschrieben werden
/// @return <code>true</code> wenn alles geklappt hat, sonst <code>false</code>
bool CGenDiploMessage::GenerateMinorAnswer(CDiplomacyInfo& info)
{
	// Dokument holen
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	CRace* pRace = pDoc->GetRaceCtrl()->GetRace(info.m_sToRace);
	if (!pRace)
		return false;
	if (pRace->GetType() != MINOR)
		return false;

	std::vector<CString> vSearchText;
	if (pRace->IsRaceProperty(FINANCIAL))
		vSearchText.push_back("FINANCIAL:");
	if (pRace->IsRaceProperty(WARLIKE))
		vSearchText.push_back("WARLIKE:");
	if (pRace->IsRaceProperty(AGRARIAN))
		vSearchText.push_back("FARMER:");
	if (pRace->IsRaceProperty(INDUSTRIAL))
		vSearchText.push_back("INDUSTRIAL:");
	if (pRace->IsRaceProperty(SECRET))
		vSearchText.push_back("SECRET:");
	if (pRace->IsRaceProperty(SCIENTIFIC))
		vSearchText.push_back("RESEARCHER:");
	if (pRace->IsRaceProperty(PRODUCER))
		vSearchText.push_back("PRODUCER:");
	if (pRace->IsRaceProperty(PACIFIST))
		vSearchText.push_back("PACIFIST:");
	if (pRace->IsRaceProperty(SNEAKY))
		vSearchText.push_back("SNEAKY:");
	if (pRace->IsRaceProperty(SOLOING))
		vSearchText.push_back("SOLOING:");
	if (pRace->IsRaceProperty(HOSTILE))
		vSearchText.push_back("HOSTILE:");

	if (vSearchText.size() == 0)
		vSearchText.push_back("NOTHING_SPECIAL:");

	CString read[14];	// Anzahl der ganzen verschiedenen Nachrichten + 1
	CString search;
	search.Format("%s:", info.m_sToRace);
	search.MakeUpper();
	// aus den ganzen Rasseneigenschaften eine zuf�llige aussuchen (falls mehrere vorhanden sind)
	CString search2 = vSearchText[rand()%vSearchText.size()];
	
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Races\\MinorsDiploAnswers.data";	// Name des zu �ffnenden Files 
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird ge�ffnet
	{
		int i = 0;
		CString csInput;	// auf csInput wird die jeweilige Zeile gespeichert
		while (file.ReadString(csInput))
		{
			// Wenn wir in dem File die Rasse gefunden haben ("RASSENNAME:" gefunden!)
			if (csInput == search || csInput == search2 || i > 0)
			{
				read[i] = csInput;
				i++;
			}
			if (i == 14)
				break;
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"MinorsDiploAnswers.data\" kann nicht ge�ffnet werden...");
		return false;
	}
	// Datei wird geschlossen
	file.Close();
	
	// Jetzt die einzelnen Werte aus dem read-Feld in die Membervariablen schreiben
	CString sAnswer = "";
	switch (info.m_nType)
	{
	case TRADE_AGREEMENT:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[1];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_TRADE_AGREEMENT");
			}
			else
			{
				info.m_sHeadline = CResourceManager::GetString("DECLINE_TRADE_AGREEMENT");
				sAnswer = read[7];
			}
			break;
		}
	case FRIENDSHIP_AGREEMENT:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[2];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_FRIENDSHIP");
			}
			else
			{
				info.m_sHeadline = CResourceManager::GetString("DECLINE_FRIENDSHIP");
				sAnswer = read[8];
			}
			break;
		}
	case COOPERATION:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[3];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_COOPERATION");
			}
			else
			{
				sAnswer = read[9];
				info.m_sHeadline = CResourceManager::GetString("DECLINE_COOPERATION");
			}
			break;
		}
	case AFFILIATION:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[4];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_AFFILIATION");
			}
			else
			{
				info.m_sHeadline = CResourceManager::GetString("DECLINE_AFFILIATION");
				sAnswer = read[10];
			}
			break;
		}
	case MEMBERSHIP:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[5];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_MEMBERSHIP");
			}
			else
			{
				info.m_sHeadline = CResourceManager::GetString("DECLINE_MEMBERSHIP");
				sAnswer = read[11];
			}
			break;
		}
	case CORRUPTION:
		{
			if (info.m_nAnswerStatus == ACCEPTED)
			{
				sAnswer = read[6];
				info.m_sHeadline = CResourceManager::GetString("ACCEPT_CORRUPTION");
			}
			else
			{
				info.m_sHeadline = CResourceManager::GetString("DECLINE_CORRUPTION");
				sAnswer = read[12];
			}
			break;
		}
	case WAR:
		{
			sAnswer = read[13];
			info.m_sHeadline = CResourceManager::GetString("WAR_OFFER");
			break;
		}
	}
	info.m_nFlag = DIPLOMACY_ANSWER;
	info.m_sText = sAnswer;
	// Sender - Empf�nger vertauschen, da es nun eine Antwort ist
	info.m_sToRace = info.m_sFromRace;
	info.m_sFromRace = pRace->GetRaceID();

	return true;
}