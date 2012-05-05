#include "Core2Main.h"

//Globale Variablen
CORE2_API DRLogger DRLog;

// Counter for dll use
int gCurrentNumUser = 0;
int gCurrentRun = 0;
#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            gCurrentNumUser++;
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            gCurrentNumUser--;
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
#endif



CORE2_API DRReturn Core2_init(const char* logFileName)
{
    //Logger init
    gCurrentRun++;
    DRLog.init(logFileName, true);
    LOG_INFO("Core2 init");
    DRFileManager::getSingleton().init();
    return DR_OK;
}

CORE2_API DRReturn Core2_init(DRLogger& log)
{
    //Logger init
    gCurrentRun++;
    DRLog = log;
    LOG_INFO("Core2 init");
    DRFileManager::getSingleton().init();
    return DR_OK;
}

CORE2_API void Core2_exit()
{
    gCurrentRun--;
    DRFileManager::getSingleton().exit();
    LOG_INFO("Core2 exit");
    DRLog.exit();
}
