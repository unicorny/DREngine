#include "ImageMain.h"

int g_ProzessCount = 0;
int g_ThreadCount = 0;
DRHashList mImageList;

void clearImageList()
{
    for(uint i = 0; i < mImageList.getNItems(); i++)
    {
        DRImage* img = (DRImage*)mImageList.findHashByIndex(i);
        DRImage* last = (DRImage*)mImageList.findByIndex(i);
        if(!last)
        {
            DR_SAVE_DELETE(img);
        }
        else
        {
            DR_SAVE_DELETE_ARRAY(img);
        }
        //*/
    }
    mImageList.clear(true);
}

#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to proces
            // return FALSE to fail DLL load
            g_ProzessCount++;
            //DRLog.writeToLog("Prozess Attach, current: %d", g_ProzessCount);
            break;

        case DLL_PROCESS_DETACH:
            g_ProzessCount--;
            //DRLog.writeToLog("Prozess Attach, current: %d", g_ProzessCount);
            // detach from process
            if(!g_ProzessCount && !g_ThreadCount)
            {
                clearImageList();
                LOG_INFO("process, images deleted");
            }
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            g_ThreadCount++;
           // DRLog.writeToLog("Thread Attach, current: %d", g_ThreadCount);
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            g_ThreadCount--;
            //DRLog.writeToLog("Thread Detach, current: %d", g_ThreadCount);
            if(!g_ProzessCount && !g_ThreadCount)
            {
                clearImageList();
                LOG_INFO("images deleted");
            }
            break;
    }
    return TRUE; // succesful
}
#endif //_WIN32



DRIImage* getInstance()
{
    DRImage* temp = NULL;
    temp = new DRImage;
    mImageList.addByHash((DHASH)temp, NULL);
    return temp;
}

DRIImage* getInstances(int count)
{
    DRImage* temp = NULL;
    temp = new DRImage[count];
    mImageList.addByHash((DHASH)temp, temp+count*sizeof(DRImage));
    return temp;
}

void releaseInstance(DRInterface* data)
{
	DHASH hash = (DHASH)(DRImage*)data;
    DRInterface* temp = (DRInterface*)mImageList.findByHash(hash);

    if(temp)
    {
        DR_SAVE_DELETE_ARRAY(data);
    }
    else
    {
        DR_SAVE_DELETE(data);
    }

    mImageList.removeByHash(hash);
}

void releaseInstances(DRInterface* datas)
{
    //LOG_INFO("Warnung, not implemented yet!");
    releaseInstance(datas);
}
//*/
