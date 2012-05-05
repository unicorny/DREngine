#include "Core2Main.h"

//Konstuktor und Deskonstruktor
DRLogger::DRLogger()
: m_bPrintToConsole(false)
{
	//m_pFile = NULL;
	strcpy(m_acFilename, "NotInitLogger.html");
}

DRLogger::DRLogger(const DRLogger& log)
{
    strcpy(m_acFilename, log.m_acFilename);
}

//------------------------------------------------------
DRLogger::~DRLogger()
{
}

//-------------------------------------------------------
//--------------------------------------------------------
//init und Exit
DRReturn DRLogger::init(const char* pcFilename, bool printToConsole)
{
    m_bPrintToConsole = printToConsole;
    
	//Filenamen kopieren
	//sprintf(m_acFilename, pcFilename);
	strcpy(m_acFilename, pcFilename);

	//�ffnen zum �berschreiben
	//m_pFile = fopen(m_acFilename, "w");
	m_File.open(m_acFilename, true, "wt");

	//pr�fen
	if(!m_File.isOpen()) return DR_ERROR;

	//Was reinschreiben
//	fprintf(m_pFile, "Init des Loggers erfolgreich!\n");

	m_File.setFilePointer(0, SEEK_SET);
	char acTemp[] = "<HTML>\n<head>\n<title>Log Datei</title></head><body>\n<font face=\"Arial\" size=\"2\">\n<table>";
	m_File.write(acTemp, sizeof(char), strlen(acTemp));
	writeToLog("Init des Loggers erfolgreich!");
//	char acTemp2[] = "<tr><a href= http://www.mathe-programme.de.tt> Homepage </a><br>Ein Programm von Dario Rekowski.</tr>";
	writeToLog("Ein Programm von Dario Rekowski.\nVerwendet die Core.dll von Dario Rekowski\n<a href= http://www.einhornimmond.de> Homepage </a>");
//	m_File.write(acTemp2, sizeof(char), strlen(acTemp2));


	//schlie�en
//	fclose(m_pFile);
//	m_pFile = NULL;
	m_File.close();
	return DR_OK;
}

//----------------------------------------------------------

void DRLogger::exit()
{
	//if(!m_pFile) m_pFile = fopen(m_acFilename, "a");
	if(!m_File.isOpen()) m_File.open(m_acFilename, false, "at");
	m_File.setFilePointer(0, SEEK_END);
	//fprintf(m_pFile, "-----------------------------------------------------\nEnde gut alles gut.");
	//fclose(m_pFile);
	writeToLog("-----------------------------------------------------\nEnde gut alles gut.");
//	char acTemp[] = "</body>\n</table></HTML>";
//	m_File.write(acTemp, sizeof(char)*strlen(acTemp));
	writeToLogDirect("\n</table></body></HTML>");


	m_File.close();
}


// *****************************************************************
// Einen 2D-Vektor in die Logbuchdatei schreiben
DRReturn DRLogger::writeVector2ToLog(const DRVector2& v, const char* name)
{
	// 2D-Vektor in die Logbuchdatei schreiben
    if(name)
        writeToLogDirect("<tr><td><b>%s</b></td></tr>", name);
    return writeToLogDirect("<tr><td><font size=\"2\"><b><font color=\"#000080\">2D-Vektor:</font></b> x = <i>%.5f</i>, y = <i>%.5f</i>, L�nge = <i>%.5f</i></td></tr>",
                            v.x, v.y, v.length());
}

// *****************************************************************
// Einen 3D-Vektor in die Logbuchdatei schreiben
DRReturn DRLogger::writeVector3ToLog(const DRVector3& v, const char* pcName /* = NULL */)
{
	// 3D-Vektor in die Logbuchdatei schreiben
	char acName[64];
	if(pcName)
		sprintf(acName, "3D-Vektor %s:", pcName);
	else
		sprintf(acName, "3D-Vektor:");
	return writeToLogDirect("<tr><td><font size=\"2\"><b><font color=\"#000080\">%s</font></b> x = <i>%.3f</i>, y = <i>%.3f</i>, z = <i>%.3f</i>, L�nge = <i>%.3f</i></td></tr>",
		                     acName, v.x, v.y, v.z, v.length());
}

// ******************************************************************
// Eine Matrix in die Logbuchdatei schreiben
DRReturn DRLogger::writeMatrixToLog(const DRMatrix& m, const char* name)
{
	// Matrix in die Logbuchdatei schreiben
    if(name)
        writeToLogDirect("<tr><td><b>%s</b></td></tr>", name);
    writeToLogDirect("<tr><td><font size=\"2\"><b><font color=\"#000080\">Matrix:</font></b><table>");
    for(int iRow = 0; iRow < 4; iRow++)
    {
            writeToLogDirect("<tr>");
            for(int iColumn = 0; iColumn < 4; iColumn++)
            {
                    writeToLogDirect("<td><font size=\"1\">");
                    writeToLogDirect("<i>%.3f</i>", m.m[iRow][iColumn]);
                    writeToLogDirect("</td>");
            }

            writeToLogDirect("</tr>");
    }

    return writeToLogDirect("</table>");
}

// ******************************************************************
// Eine Ebene in die Logbuchdatei schreiben
/*DRReturn DRLogger::WritePlaneToLog(tbPlane& p)
{
	// Ebene in die Logbuchdatei schreiben
	return tbWriteToLogDirect("<tr><td><font size=\"2\"><b><font color=\"#000080\">Ebene:</font></b> a = <i>%.3f</i>, b = <i>%.3f</i>, c = <i>%.3f</i>, d = <i>%.3f</i>, Normalenvektorl�nge = <i>%.3f</i></td></tr>",
                              p.a, p.b, p.c, p.d, tbVector3Length(p.n));
}
*/

// ******************************************************************
// Eine Farbe in die Logbuchdatei schreiben

DRReturn DRLogger::writeColorToLog(const DRColor& c)
{
	char acHexColor[9];

	sprintf(acHexColor, "%x", (int)(c) << 8);
	acHexColor[6] = 0;

	// Farbe in die Logbuchdatei schreiben
	return writeToLogDirect("<tr><td><font size=\"2\"><b><font color=\"#000080\">Farbe:</font></b> a = <i>%.3f</i>, r = <i>%.3f</i>, g = <i>%.3f</i>, b = <i>%.3f</i>, Hexadezimal: <i>0x%x</i>, <font color=\"#%s\"><i>Probetext</i></font></td></tr>",
                              c.a, c.r, c.g, c.b, (int)(c), acHexColor);
}

//**************************************************************************************************************************
//*/
DRReturn DRLogger::writeToLog(const char* pcText, ...)
{
    //Textbuffer zum schreiben
	char acBuffer[1024];
	
	va_list   Argumente;

	//Buffer fuellen
	va_start(Argumente, pcText);
	vsprintf(acBuffer, pcText, Argumente);
	va_end(Argumente);
    
    return writeToLog(DRString(acBuffer));
}

DRReturn DRLogger::writeToLog(DRString text)
{
    DRString final = DRString("<tr><td><font size=\"2\" color=\"#000080\">");
    final += text;
    final += DRString("</font></td></tr>");
    
    return writeToLogDirect(final);
}

//***************************************************************************

DRReturn DRLogger::writeToLogDirect(const char* pcText, ...)
{
	//Textbuffer zum schreiben
	char acBuffer[1024];
	
	va_list   Argumente;

	//Buffer fuellen
	va_start(Argumente, pcText);
	vsprintf(acBuffer, pcText, Argumente);
	va_end(Argumente);
    
    return writeToLogDirect(DRString(acBuffer));
}

DRReturn DRLogger::writeToLogDirect(DRString text)
{    
	//Datei zum anh�ngen �ffnen (wenn sie es nicht schon ist)
	if(!m_File.isOpen()) m_File.open(m_acFilename, false, "at");

	//wenns nicht geht, Fehler
	if(!m_File.isOpen()) return DR_ERROR;

	m_File.setFilePointer(0, SEEK_END);

    if(m_bPrintToConsole)
        printf("%s\n", text.data());
        
	//ersetzen der \n durch <br>
	text.replace(text.find('\n'),1,"<br>");

	//einf�gen eines Zeilenumbruchs und Formationen
	if(m_File.getFile())
            fprintf(m_File.getFile(), text.data());

	//und Datei schlie�en (nur im Debug Modus)
#ifdef _DEBUG
//	fclose(m_pFile);
	fflush(m_File.getFile());
	//m_File.close();
//	m_pFile = NULL;
	// Zus�tzlich wird noch eine Debug-Ausgabe erzeugt.
	OutputDebugString(text.data());
#endif
    
	return DR_OK;
}


//********************************************************************************************************************++

