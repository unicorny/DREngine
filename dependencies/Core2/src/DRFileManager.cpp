//#include "DRFileManager.h"
#include "Core2Main.h"

using namespace std;

DRFileManager& DRFileManager::Instance()
{
	static DRFileManager TheOneAndOnly;
	return TheOneAndOnly;
}


//********************************************************************
DRReturn DRFileManager::init()
{
	m_bInitialized = true;
	LOG_INFO("File Manager initaliesiert!");
	return DR_OK;
}

//***********************************************************************

DRReturn DRFileManager::addFile(const char* pcFilename)
{
	if(!m_bInitialized) return DR_OK;
	//return CheckFile(pcFilename);
	if(!pcFilename) return DR_ZERO_POINTER;

	// Wird schon innerhalb der DRFile Klasse aufgerufen
/*	const char* pcWPfad = getWholePfad(pcFilename);
	char acWPfad[256];
	if(pcWPfad)
		sprintf(acWPfad, "%s/%s", pcWPfad, pcFilename);
	else
		sprintf(acWPfad, pcFilename);
		*/
	DRFile File(pcFilename, "rb");
	if(!File.isOpen()) LOG_ERROR("Datei konnt nicht geöffnet werden!", DR_ERROR);
	if(!isFileOK(&File))
	{
		//Kein eigenes Format, trozdem übernehmen
		DHASH ID = DRMakeDoubleHash("NULL", pcFilename);
		SIndex* pTIndexx = new SIndex;
		pTIndexx->ID = ID;
		pTIndexx->strFilename = pcFilename;
		pTIndexx->ulCursorInFilePos = 0;

		if(!m_IndexList.addByHash(ID, pTIndexx))
        {
            DR_SAVE_DELETE(pTIndexx);
            LOG_ERROR("Hash-Kollision", DR_ERROR);
        }

		char pcInfo[256];
		sprintf(pcInfo, "Datei %s wurde in FileIndexList vom FileManager eingetragen!\n-Keine DDatei", pcFilename);
		LOG_INFO(pcInfo);
		File.close();
		return DR_OK;

	}

	//Anzahl Bloecke lesen
	u32 ulNumBloecke = 0L;
	//DRSHauptHeader
	File.setFilePointer(8 + sizeof(long) + sizeof(DRReal), SEEK_SET);
	File.read(&ulNumBloecke, sizeof(long), 1);

	for(unsigned long ul = 0; ul < ulNumBloecke; ul++)
	{
		//Einlesen
		DRSBlockHeaderF TBlockHeader;
		File.read(&TBlockHeader.u16HeaderLength, sizeof(short), 1);
		File.read(&TBlockHeader.u32BlockLength, sizeof(long), 1);
		File.read(&TBlockHeader.ID, sizeof(DHASH), 1);
		File.read(&TBlockHeader.iStrLength, sizeof(int), 1);

		if(TBlockHeader.iStrLength > 256)
		{
			DRLog.writeToLog("strLength: %d", TBlockHeader.iStrLength);
			LOG_ERROR("strLength ist zu groß, Fehler in der Datei?", DR_ERROR);
		}
		TBlockHeader.pcDateiname = new char[TBlockHeader.iStrLength+1];
		memset(TBlockHeader.pcDateiname, 0, TBlockHeader.iStrLength+1);

		File.read(TBlockHeader.pcDateiname, sizeof(char), TBlockHeader.iStrLength);

		//Eintragen
		SIndex* pTIndex = new SIndex;
		pTIndex->ID = TBlockHeader.ID;
		pTIndex->strFilename = pcFilename;
		pTIndex->ulCursorInFilePos = File.getFilePointer();

		//Doubletten und HASH Kollisions Check
		SIndex* pCheckIndex = (SIndex*)m_IndexList.findByHash(pTIndex->ID);

		//Eine Kollision liegt vor
		if(pCheckIndex)
		{
#ifdef _DEBUG
                //Block Kollision?
                if(pCheckIndex->strFilename ==  pTIndex->strFilename && pCheckIndex->ulCursorInFilePos == pTIndex->ulCursorInFilePos)
                {
                    LOG_WARNING("Block steht schon in der IndexList!");
                    DRLog.writeToLog("In Datei %s im Block %s", pcFilename, TBlockHeader.pcDateiname);
                }//HASH Kollision?
                else
                {
                    LOG_WARNING("Es liegt eine HASH Kollision vor!");
                    DRLog.writeToLog("Bei HASH: %d in Datei: %s im Block: %s", TBlockHeader.ID, pcFilename, TBlockHeader.pcDateiname);
                }
#endif
		}
		else
                m_IndexList.addByHash(pTIndex->ID, pTIndex);

		File.setFilePointer(TBlockHeader.u32BlockLength, SEEK_CUR);
		DR_SAVE_DELETE_ARRAY(TBlockHeader.pcDateiname);
	}

	//Info
	char pcInfo[256];
	sprintf(pcInfo, "Datei %s wurde in FileIndexList vom FileManager eingetragen!", pcFilename);
	LOG_INFO(pcInfo);
	File.close();

	return DR_OK;

}

//*****************************************************************

DRReturn DRFileManager::newFile(const char* pcFilename)
{
	if(!m_bInitialized) return DR_OK;
	//Check
	if(!pcFilename) return DR_ZERO_POINTER;
	DRFile TempFile(pcFilename, "wb");
	char acError[256];
	char acStart[8] = "DDatei";
	if(!TempFile.isOpen())
	{
            sprintf(acError, "Neue Datei konnte nicht erstellt werden: %s!", pcFilename);
            LOG_ERROR(acError, DR_ERROR);
	}
	//Hauptheader
	DRSHauptHeader HauptHeader;
	//sprintf(HauptHeader.acIdentifierCheck, acStart);
	strcpy(HauptHeader.acIdentifierCheck, acStart);
	HauptHeader.ulDateiGesLegth = sizeof(DRSHauptHeader);
	HauptHeader.fFileVersion = FILEMANAGEREVRSION;
	HauptHeader.ulNumBloecke = 0;
	TempFile.write(&HauptHeader, sizeof(DRSHauptHeader), 1);

	char acEnd[8] = "DEnde";
	TempFile.write(acEnd, sizeof(char), 8);
	TempFile.close();

	return DR_OK;
}

//***************************************************************************************
DRFile* DRFileManager::startWriting(const char* pcPfadName, const char* pcFilename, const char* pcTyp, unsigned long ulBlockLength)
{
	if(!m_bInitialized) return NULL;
	//Zero Pointer Check
	if(!pcPfadName) return NULL;
	if(!pcFilename) return NULL;
	if(!pcTyp) return NULL;

    DRFile* pTempFile = NULL;
    pTempFile = getFileByName(pcTyp, pcFilename);
    if(!pTempFile)
    {
        pTempFile = new DRFile(pcPfadName, "r+b");
        if(!pTempFile->isOpen() || !isFileOK(pTempFile))
        {
            LOG_WARNING("Neue Datei muss erstellt werden!");
            if(newFile(pcPfadName)) return NULL;

            pTempFile->open(pcPfadName, false, "r+b");
            if(!pTempFile->isOpen()) return NULL;
        }
        
        if(!isFileOK(pTempFile)) return NULL;
        pTempFile->setFilePointer(pTempFile->getSize()-8, SEEK_SET);

        unsigned short u16HeaderLength = 2*sizeof(long) + sizeof(int) + sizeof(short) + strlen(pcFilename);
        DHASH ID = DRMakeDoubleHash(pcTyp, pcFilename);

        //BlockHeader schreiben
        pTempFile->write(&u16HeaderLength, sizeof(unsigned short), 1);
        pTempFile->write(&ulBlockLength, sizeof(unsigned long), 1);
        pTempFile->write(&ID, sizeof(DHASH), 1);
        //pstrFilename->Save(pTempFile);
        int istrLength = strlen(pcFilename);
        pTempFile->write(&istrLength, sizeof(int), 1);
        pTempFile->write(pcFilename, sizeof(char), istrLength);

        unsigned long ulCurrentPointerPos = pTempFile->getFilePointer();

        //Hauptheader aktualisieren
        //Dateigröße
        pTempFile->setFilePointer(8, SEEK_SET);
        unsigned long ulDateiGesLength;
        pTempFile->read(&ulDateiGesLength, sizeof(unsigned long), 1);
        ulDateiGesLength += u16HeaderLength + ulBlockLength;
        pTempFile->setFilePointer(8, SEEK_SET);
        pTempFile->write(&ulDateiGesLength, sizeof(unsigned long), 1);

        //NumBlöcke
        pTempFile->setFilePointer(8 + sizeof(long) + sizeof(DRReal), SEEK_SET);
        unsigned long ulNumBloecke;
        pTempFile->read(&ulNumBloecke, sizeof(unsigned long), 1);
        ulNumBloecke++;
        pTempFile->setFilePointer(8 + sizeof(long) + sizeof(DRReal), SEEK_SET);
        pTempFile->write(&ulNumBloecke, sizeof(unsigned long), 1);

        pTempFile->setFilePointer(ulCurrentPointerPos, SEEK_SET);
    }
	//prüfe ob neue dateigröße der alten entspricht
	else
	{
		u32 filePointer = pTempFile->getFilePointer();
		int iStrLen = strlen(pcFilename);
		//filePointer -= iStrLen*sizeof(char) + sizeof(int) + sizeof(DHASH) + sizeof(u32);
		u32 blockLength = 0;
		pTempFile->setFilePointer(-(iStrLen*sizeof(char) + sizeof(int) + sizeof(DHASH) + sizeof(u32)), SEEK_CUR);
		u32 now = pTempFile->getFilePointer();
		pTempFile->read(&blockLength, sizeof(unsigned long), 1);
		pTempFile->setFilePointer(filePointer, SEEK_SET);
		if(ulBlockLength > blockLength)
		{
			closeFile(pTempFile);
			LOG_ERROR("neuer Dateieintrag ist zu gross!", NULL);
		}
	}

	return pTempFile;

}
//************************************************************************************************************
DRReturn DRFileManager::endWriting(DRFile* pOpenFile)
{
	if(!m_bInitialized) return DR_OK;

	//ParamterCheck
	if(!pOpenFile) return DR_ZERO_POINTER;
	if(!pOpenFile->isOpen()) return DR_ERROR;
	
	//Hauptheader Size updaten
	pOpenFile->setFilePointer(8, SEEK_SET);
	unsigned long ulDateiGesLength;
	pOpenFile->read(&ulDateiGesLength, sizeof(unsigned long), 1);
	if(ulDateiGesLength+8 != pOpenFile->getSize())
	{
#ifdef _DEBUG
        LOG_WARNING("Werte für Datei Length stimmen nicht überein!")
        DRLog.writeToLog("Berechnet: %d, über GetSize(): %d", ulDateiGesLength+8, pOpenFile->getSize());
#endif 
        pOpenFile->setFilePointer(8, SEEK_SET);
        ulDateiGesLength = pOpenFile->getSize();
        pOpenFile->write(&ulDateiGesLength, sizeof(unsigned long), 1);

        //End schreiben
        pOpenFile->setFilePointer(0, SEEK_END);
        char acEnde[8] = "DEnde";
        pOpenFile->write(acEnde, sizeof(char), 8);
	}        

	pOpenFile->close();
	DR_SAVE_DELETE(pOpenFile);

	return DR_OK;
}

//********************************************************

bool DRFileManager::isFileOK(DRFile* pOpenFile, bool bPointerOnEnd /* = true */)
{
	if(!m_bInitialized) return false;

	//Pointer Check
	if(!pOpenFile) return false;
	if(!pOpenFile->isOpen()) return false;


	unsigned long ulCurrentFilePointerPos = pOpenFile->getFilePointer();
	//FilePointer setzen und Gr��e pr�fen
	pOpenFile->setFilePointer(0, SEEK_SET);
	if(pOpenFile->getSize() < 19)
	{
		DRLog.writeToLog("DateiGr��e: %d", pOpenFile->getSize());
		LOG_ERROR("zu klein", false);
	}
	char acStart[8];


	//Lesen
	pOpenFile->read(acStart, sizeof(char), 8);
	if(strcmp(acStart, "DDatei")) LOG_ERROR("DDatei fehlt", false);
	DRReal fVersion = 0.0f;
	//pOpenFile->SetFilePointer(120, SEEK_CUR);

	unsigned long ulGesLength;
	pOpenFile->read(&ulGesLength, sizeof(unsigned long), 1);

	pOpenFile->read(&fVersion, sizeof(DRReal), 1);
	if(fVersion != FILEMANAGEREVRSION) LOG_ERROR("Falsche Version", false);

	if(pOpenFile->setFilePointer(pOpenFile->getSize()-8, SEEK_SET)) LOG_WARNING("Fehler beim SetFilePointer!");
	char acEnd[8];
	pOpenFile->read(acEnd, sizeof(char), 8);
	if(strcmp(acEnd, "DEnde")) LOG_ERROR("DEnde fehlt!", false);

	if(!bPointerOnEnd)
            pOpenFile->setFilePointer(ulCurrentFilePointerPos, SEEK_SET);
	else
            pOpenFile->setFilePointer(ulGesLength, SEEK_SET);
	return true;

}
/*
DRReturn DRFileManager::Init()
{
	FMARK;
	//in ..\Data wird auf jeden Fall gesucht
	DRString* pStr = new DRString("Data");
	m_OrdnerList.push_back(pStr);
//	LOG("in OrdnerList getan: %s", pStr->GetString());
	pStr = new DRString(".");
	m_OrdnerList.push_back(pStr);
//	LOG("in OrdnerList getan: %s", pStr->GetString());

	//Data.ini lesen, und Pfade eintragen in OrdnerList
	if(IsFileExist(pStr->GetString()))
	{
		char acTemp[256];
		int iNumPfade = 0;
		//Anzahl Pfade lesen
		GetPrivateProfileString("Pfade", "NumPfade", "[NOT FOUND]", acTemp, 256, "Data\\Data.ini");
		if(strcmp(acTemp, "[NOT FOUND]"))
		{
			iNumPfade = atoi(acTemp);
			for(int i = 0; i < iNumPfade; i++)
			{
				//Anzahl der Pfade durchgehen und eintragen
				char acKey[32];
				sprintf(acKey, "Pfad%d", i);
				GetPrivateProfileString("Pfade", acKey, "[NOT FOUND]", acTemp, 256, "Data\\Data.ini");
				//�berpr�fen
				if(strcmp(acTemp, "[NOT FOUND]"))
				{
					//In Liste eintragen
					pStr = new DRString(acTemp);
					m_OrdnerList.push_back(pStr);
				}
			}
		}
	}
	//m_OrdnerListe durchgehen und alle Dateien untersuchen
	while(m_OrdnerList.size() > 0)
	{
		DRString* pTempStr = m_OrdnerList.front();
		if(FillIndexFromOrdner(pTempStr))
		{
			LOG("FillIndexFromOrdner fehlgeschlagen!");
			LOG_WARNING("FillIndexFromOrdner fehlgeschlagen!");
		}
		m_OrdnerList.pop_front();
		DR_SAVE_DELETE(pTempStr);
	}
	//Liste sortieren
	m_IndexList.SortItems(m_IndexList.GetNItems(), (DRHashListItem*)m_IndexList.GetData());

	//Fertig
	LOG_INFO("FileManager initalisiert!");

	return DR_OK;
}

//*******************************************************************************************

DRReturn DRFileManager::FillIndexFromOrdner(DRString* pstrOrdner)
{
	FMARK;
	//NullPointer check
	if(!pstrOrdner) LOG_ERROR("pstrOrdner ist Null", DR_ZERO_POINTER);
#ifdef __WIN32__
	//Pfad durchsuchen
   WIN32_FINDR_DATA FindFileData;
   HANDLE hFind = INVALIDR_HANDLE_VALUE;
   char DirSpec[MAX_PATH + 1];  // directory specification
   char DirSpec2[MAX_PATH + 1];
   unsigned long dwError;
   DRString* pstrTemp = NULL;

   strncpy (DirSpec, pstrOrdner->GetString(), pstrOrdner->GetStringSize()+1);
    strncpy(DirSpec2, DirSpec, pstrOrdner->GetStringSize()+1);
   strncat (DirSpec, "\\*", 3);

   hFind = FindFirstFile(DirSpec, &FindFileData);

   if (hFind == INVALIDR_HANDLE_VALUE)
   {
		LOG("Invalid file handle. Error is %u\n, Dir is: %s!", GetLastError(), DirSpec);
		LOG_ERROR("Erste Gefundene Datei fehlerhaft!", DR_ERROR);
   }
   else
   {
	   char acTemp[256];
	   sprintf(acTemp, "%s\\%s", DirSpec2, FindFileData.cFileName);
	  // DRLog.WriteToLog("File: %s", acTemp);
	   pstrTemp = new DRString(acTemp);
	   if (CheckFile(pstrTemp))
	   {
		   LOG("Fehler in CheckFile!");
		   //LOG_WARNING("Fehler in CheckFile!");
	   }
	   while (FindNextFile(hFind, &FindFileData) != 0)
	   {
		    sprintf(acTemp, "%s\\%s", DirSpec2, FindFileData.cFileName);
 		  //  DRLog.WriteToLog("File: %s", acTemp);
			pstrTemp = new DRString(acTemp);
			if (CheckFile(pstrTemp))
			{
				LOG("Fehler in CheckFile!");
#ifdef _DEBUG
				LOG_WARNING("Fehler in CheckFile!");
#endif
			}
	   }

		dwError = GetLastError();
		FindClose(hFind);
		if (dwError != ERROR_NO_MORE_FILES)
		{
			LOG("Es gab eine Fehler: %d", dwError);
			LOG_ERROR("Exit wegen unbekanntem Fehler", DR_ERROR);

		}
   }
#endif

   return DR_OK;
}
*/
//***************************************************************************************************++

DRReturn DRFileManager::checkFile(const char* pcFilename) const
{
	if(!m_bInitialized) return DR_OK;
	//NullPointer check
	if(!pcFilename) LOG_ERROR("pcFilename ist Null", DR_ZERO_POINTER);

	//erstaml schauen ob es �berhaupt eine Datei ist
	//char acVergleichText[5];
	//sprintf(acVergleichText, ".");
/*	char* pcTemp = pstrFile->GetString();
	char acTemp[256];
	sprintf(acTemp, pcTemp);
	DRLog.WriteToLog("String: %s, ZeichenLast: %d", pstrFile->GetString(), (int)pstrFile->GetString()[pstrFile->GetStringSize()-1]);
	DRLog.WriteToLog("String: %s, Last Zeichen: %d", pcTemp, (int)pcTemp[pstrFile->GetStringSize()-1]);
	DRLog.WriteToLog("StringSize: %d", pstrFile->GetStringSize());
	DRLog.WriteToLog("acTemp: %s", acTemp);
	for (int j = 0; j < pstrFile->GetStringSize()-1; j++)
	{
		DRLog.WriteToLog("Wert ist %d", (int)pcTemp[j]);
	}
//	*/
	//'.' = 46
	/*if((int)pstrFile->GetString()[pstrFile->GetStringSize()-2] == 46)
//	DRLog.WriteToLog("Zeichen: %d, j: %d", (int)acTemp[7], j);
	//if((int)acTemp[pstrFile->GetStringSize()-2] == 46)
	//if(!strcmp(pstrFile->GetString(), acVergleichText))
	{
		DR_SAVE_DELETE(pstrFile);
//		DRLog.WriteToLog("Punkt!");
//		LOG("Punkt");
		return DR_OK;
	}

/*	sprintf(acVergleichText, "..");
	if(!strcmp(pstrFile->GetString(), acVergleichText))
	{
		DR_SAVE_DELETE(pstrFile);
		return DR_OK;
	}
	*/

	//Ist es vielleicht ein Ordner?
	//Wenn er keine Punkte hat, vielleicht
/*	if(!DRCountCharsOf(pstrFile, '.'))
	{
		m_OrdnerList.push_back(pstrFile);
	//	LOG("in OrdnerList getan: %s", pstrFile->GetString());
		return DR_OK;
	}
	//Oder ein Punkt und der Punkt ist das erste Zeichen
	else if(DRCountCharsOf(pstrFile, '.') == 1)
	{
		if(pstrFile->GetString()[0] == '.')
		{
			m_OrdnerList.push_back(pstrFile);
		//	LOG("in OrdnerList getan: %s", pstrFile->GetString());
			return DR_OK;
		}
	}
*/
	//------------------------------------------------------------------------------
	//Es muss eine Datei sein
	//Ist es eine meiner Dateien? oder eine "Fremd" Datei?
	DRFile* pTempFile = new DRFile(pcFilename);
	if(!pTempFile->isOpen())
	{
#ifdef _DEBUG
            DRLog.writeToLog("%s konnte nicht ge�ffnet werden!", pcFilename);
            LOG_ERROR("Fehler beim �ffnen von pstrFile", DR_ERROR);
#endif
            return DR_ERROR;
	}

	//Hauptheader einlesen
//	int m_iNumChars;
//	pTempFile->read(&m_iNumChars, 4, 1);
//	pTempFile->SetFilePointer(0, SEEK_SET);
//	DRString* pstrTemp = NULL;
	//Nur einlesen wenn es m�glich ist
//	if(m_iNumChars <= 10 && pTempFile->GetSize() > 14)
//		pstrTemp = new DRString(pTempFile);	//4 Bytes + 6 oder 7 Bytes
	char acTemp[8];
	pTempFile->read(acTemp, sizeof(char), 8);
	acTemp[6] = '\0';
	SIndex*  pTempIn = NULL;
	if(strcmp(acTemp, "DDatei"))
	{
            //nicht identisch, "Fremd" Datei
        /*    pTempIn = new SIndex;
            pTempIn->ID = DRMakeDoubleHash("NULL", DRRemoveDir(pcFilename));
            pTempIn->strFilename = pcFilename;
            pTempIn->ulCursorInFilePos = 0;

            //eintragen
            m_IndexList.addByHash(pTempIn->ID, pTempIn); */
            pTempFile->close();
    //	DR_SAVE_DELETE(pstrFile);
            DR_SAVE_DELETE(pTempFile);
            return DR_OK;
	}

	//es ist eine eigene Datei, Header einlesen
	unsigned long dwDateiGesLength;					//4 Bytes
	DRReal fDateiManagerVersion;	//4 Bytes
	unsigned long dwNumBloecke;						//4 Bytes

	//lesen
	pTempFile->read(&dwDateiGesLength, sizeof(unsigned long), 1);
	pTempFile->read(&fDateiManagerVersion, sizeof(DRReal), 1);
	pTempFile->read(&dwNumBloecke, sizeof(unsigned long), 1);
	if(fDateiManagerVersion != FILEMANAGEREVRSION)
	{
            DRLog.writeToLog("Falscher Version in der Datei: %f, Klasse: %f", fDateiManagerVersion, FILEMANAGEREVRSION);
            pTempFile->close();
            DR_SAVE_DELETE(pTempFile);

            LOG_ERROR("Versionskontrolle", DR_ERROR);
	}
	//Ckecken ob Ende in Ordnung
	if(pTempFile->setFilePointer(pTempFile->getSize() - 6, SEEK_SET))
		LOG_ERROR("FilePointer konnte nicht gesetzt werden!", DR_ERROR);

//	if(!pTempFile->GetSize() <= dwDateiGesLength + 10)
//		pstrTemp->Load(pTempFile);
//	else
	//	DR_SAVE_DELETE(pstrTemp);
	pTempFile->read(acTemp, sizeof(char), 6);
	acTemp[6] = '\0';

	//DRString strVergleich("DEnde");
	//if(!pstrTemp || *pstrTemp != strVergleich)
	if(strcmp(acTemp, "DEnde"))
	{
		//Fehler
		DRLog.writeToLog("%s ist Fehlerhaft, DEnde fehlt, Datei wird gel�scht!", pcFilename);
		pTempFile->close();
		DR_SAVE_DELETE(pTempFile);
	//	DR_SAVE_DELETE(pstrFile);

		LOG_ERROR("Fehlerhafte Datei", DR_ERROR);

	}
        pTempFile->close();
	DR_SAVE_DELETE(pTempFile);

	return DR_OK;
        
	//lesen und eintragen aller Bl�cke
	pTempFile->setFilePointer(18, SEEK_SET);
//	pstrTemp->Load(pTempFile);
	//pTempFile->SetFilePointer(12, SEEK_CUR);
	//Pointer am ersten Block (m�sste jedenfalls)

	//Alle Bl�cke durchgehen und eintragen
	for (u32 i = 0; i < dwNumBloecke; i++)
	{
            pTempIn = new SIndex;

            //BlockHeader einlesen
            unsigned short    wHeaderLength;
            unsigned long   dwBlockLength;
            pTempIn->ulCursorInFilePos = pTempFile->getFilePointer();
            pTempFile->read(&wHeaderLength, 2, 1);
            pTempFile->read(&dwBlockLength, 4, 1);
            pTempFile->read(&pTempIn->ID, sizeof(DHASH), 1);

            char tempB[256]; memset(tempB, 0, sizeof(char)*256);
            int size = 0;
            pTempFile->read(&size, sizeof(int), 1);
            if(size < 256)
                pTempFile->read(tempB, sizeof(char), size);
            else
                LOG_WARNING("string ist zu lang, oder ungueltig");

            pTempIn->strFilename = tempB;
            //eintragen
         //   if(!m_IndexList.addByHash(pTempIn->ID, &pTempIn))
           //     LOG_WARNING("Double HASH");
            pTempFile->setFilePointer(dwBlockLength, SEEK_CUR);
	}
//	DR_SAVE_DELETE(pstrFile);
	pTempFile->close();
	DR_SAVE_DELETE(pTempFile);

	return DR_OK;
}


//************************************************************************************************************

void DRFileManager::exit()
{
	if(!m_bInitialized) return;
	//OrdnerList leeren (m�sste eigentlich leer sein)
	m_OrdnerList.clear();
	m_OrdnerPfadList.clear();

	//IndexList leeren
	for (int i = 0; i < (int)m_IndexList.getNItems(); i++)
	{
            SIndex* pTempIn = (SIndex*)m_IndexList.findByIndex((u32)i);
            m_IndexList.removeByHash(pTempIn->ID);
            DRLog.writeToLog("Inhalt von Datiename: %s", pTempIn->strFilename.data());
            DR_SAVE_DELETE(pTempIn);
	}
	m_IndexList.clear(true);
	m_bInitialized = false;
	LOG_INFO("FileManager heruntergefahren!");

}

//********************************************************************************************************************

DRFile* DRFileManager::getFileByHASH(DHASH ID) const
{
	if(!m_bInitialized) return NULL;
	SIndex* pTemp = (SIndex*)m_IndexList.findByHash(ID);

	//Nicht vorhanden
	if(!pTemp)
	{
#ifdef _DEBUG
            LOG_WARNING("ID nicht in Liste vorhanden!");
#endif
            return NULL;
	}
	DRFile* pFile = new DRFile(pTemp->strFilename.data(), "r+b");

	//datei kann nicht geöffnet werden
	if(!pFile->isOpen())
	{
#ifdef _DEBUG
        DRLog.writeToLog("pFile: %s konnte nicht geöffnet werden!", pTemp->strFilename.data());
        DR_SAVE_DELETE(pFile);
        LOG_ERROR("Datei konnte nicht geöffnet werden!", NULL);
#else
		DR_SAVE_DELETE(pFile);
		return NULL;
#endif
	}

	pFile->setFilePointer(pTemp->ulCursorInFilePos, SEEK_SET);
	return pFile;

}

//--------------------------------------------------------------------------------------------------------------------

DRFile* DRFileManager::getFileByName(const char* pcTyp, const char* pcFilename) const
{
    if(!m_bInitialized) return NULL;
    return getFileByHASH(DRMakeDoubleHash(pcTyp, pcFilename));
}

//-----------------------------------------------------------------------------------------------------------------------
/*
DRFile* DRFileManager::GetFileByName(DRString* pstrTyp, DRString* pstrFilename)
{
	return GetFileByName(pstrTyp->GetString(), pstrFilename->GetString());
}
*/

void DRFileManager::closeFile(DRFile* pFile)
{
    if(!m_bInitialized) return;
    if(pFile && pFile->isOpen()) pFile->close();
    DR_SAVE_DELETE(pFile);
}


//********************************************************************************************************************++


DRReturn DRFileManager::addOrdner(const char* pcPfadName)
{
	if(!m_bInitialized) return DR_OK;
	//DRString* pOrd = new DRString(pcPfadName);
	//m_OrdnerPfadList.AddEntry(&pOrd);
	m_OrdnerPfadList.push_back(string(pcPfadName));
	return DR_OK;
}

DRReturn DRFileManager::addFolderToFileSystem(const char* folderName)
{
#ifdef _WIN32
    int state = _mkdir(folderName);
	if(state)
	{
		DRLog.writeToLog("Fehler bei erstellen des Ordners: %s, fehlernr: %d", folderName, errno);
	}
#else
    int state = mkdir(folderName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(!state)
    {
        DRLog.writeToLog("Fehlernummer: %d", errno);
        LOG_ERROR("Fehler beim anlegen eines Ordners", DR_ERROR);
    }
#endif
	return DR_OK;
}

//********************************************************************************************************************++

const char* DRFileManager::getWholePfad(const char* pcFileName)
{
	if(!m_bInitialized) return NULL;

	FILE* pTempFile = NULL;/*fopen(pcFileName, "rb");
        if(pTempFile)
        {
            fclose(pTempFile);
            return pcFileName;
        }*/
        
	for(list<string>::iterator it = m_OrdnerPfadList.begin(); it != m_OrdnerPfadList.end(); it++)
	{
        pTempFile = fopen(string(*it + "/" + string(pcFileName)).data(), "rb");
        if(pTempFile)
		{
			fclose(pTempFile);
			return (*it).data();
		}
	}

	if(pTempFile)
		fclose(pTempFile);
	return NULL;
}
