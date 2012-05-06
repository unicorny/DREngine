#include "Engine2Main.h"

DREngineLogger::~DREngineLogger()
{
    
}

DREngineLogger::DREngineLogger()
: mMutex(NULL)
{
    
}

DRReturn DREngineLogger::init(const char* pcFilename, bool printToConsole)
{
	mMutex = SDL_CreateMutex(); 
    DRReturn ret = DRLogger::init(pcFilename, printToConsole);    
	return ret;
}

void DREngineLogger::exit()
{
    DRLogger::exit();
    if(mMutex)
		SDL_DestroyMutex(mMutex); 
	
}

DRReturn DREngineLogger::writeToLogDirect(DRString text)
{
    DRReturn ret = DR_OK;
    SDL_LockMutex(mMutex); 
    DRLogger::writeToLogDirect(text);
    SDL_UnlockMutex(mMutex);
    return ret;
}

DRReturn DREngineLogger::writeToLog(const char* pcText, ...)
{
    //Textbuffer zum schreiben
	char acBuffer[1024];
	
	va_list   Argumente;

	//Buffer fuellen
	va_start(Argumente, pcText);
	vsprintf(acBuffer, pcText, Argumente);
	va_end(Argumente);
	DRString final = EnGetTimeSinceStart();
	final += DRString(acBuffer);
    
	return writeToLog(final);
}
DRReturn DREngineLogger::writeToLog(DRString text)
{
    DRString final = DRString("<tr><td><font size=\"2\" color=\"#000080\">");
    final += text;
    final += DRString("</font></td></tr>");
    
    return writeToLogDirect(final);
}
