#include "NetworkMain.h"

Network* g_Instance = NULL;
int g_ProzessCount = 0;
int g_ThreadCount = 0;

#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to proces
            // return FALSE to fail DLL load
            g_ProzessCount++;
            DRLog.writeToLog("Prozess Attach, current: %d", g_ProzessCount);
            break;

        case DLL_PROCESS_DETACH:
            g_ProzessCount--;
            DRLog.writeToLog("Prozess Attach, current: %d", g_ProzessCount);
            // detach from process
            if(g_Instance && !g_ProzessCount && !g_ThreadCount)
            {
                g_Instance->exit();
                delete g_Instance;
                g_Instance = NULL;

                LOG_INFO("process, instance deleted");
            }
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            g_ThreadCount++;
            DRLog.writeToLog("Thread Attach, current: %d", g_ThreadCount);
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            g_ThreadCount--;
            DRLog.writeToLog("Thread Detach, current: %d", g_ThreadCount);
            if(g_Instance && !g_ProzessCount && !g_ThreadCount)
            {
                g_Instance->exit();
                delete g_Instance;
                g_Instance = NULL;
                LOG_INFO("instance deleted");            }
            break;
    }
    return TRUE; // succesful
}
#endif //_WIN32



DRINetwork* getInstance()
{
    if(!g_Instance)
    {
        g_Instance = new Network;
        g_Instance->init();
        LOG_INFO("new called");
    }
    return g_Instance;
}

