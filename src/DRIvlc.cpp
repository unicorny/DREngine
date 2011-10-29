#include "Engine2Main.h"

#ifdef _WIN32
HMODULE   DRIvlc::mDLL = 0;
#else
void*     DRIvlc::mDLL = NULL;
#endif


DRIvlc& DRIvlc::Instance()
{
	static DRIvlc TheOneAndOnly;
	return TheOneAndOnly;
}


DRIvlc::DRIvlc()
: mInitalized(false)
{
}

DRReturn DRIvlc::init()
{
    if(mInitalized) return DR_OK;

    // load dll
    if(mDLL) LOG_ERROR("dll already loaded", DR_ERROR);
#ifdef _WIN32
    mDLL = LoadLibrary(__vlc_DLL_NAME_);
#else
    mDLL = dlopen(__vlc_DLL_NAME_, RTLD_LAZY);
#endif
    if(!mDLL) LOG_ERROR("dll couldnT loaded", DR_ERROR);


    LOG_INFO("vlc init");
    mInitalized = true;

    return DR_OK;
}

void DRIvlc::exit()
{
    if(!mInitalized) return;

    // freigeben der dll
    if(mDLL)
    {
#ifdef _WIN32
        FreeLibrary(mDLL);
#else
        dlclose(mDLL);
#endif
        mDLL = 0;
    }

    LOG_INFO("vlc exit");

}
