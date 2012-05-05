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
    DRLogger::init(pcFilename, printToConsole);
    mMutex = SDL_CreateMutex();
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
    SDL_LockMutex(mMutex); LOG_WARNING_SDL();
    DRLogger::writeToLogDirect(text);
    SDL_UnlockMutex(mMutex); LOG_WARNING_SDL();
    return ret;
}