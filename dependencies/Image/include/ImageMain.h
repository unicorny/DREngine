#ifndef __DR_IMAGE_MAIN__
#define __DR_IMAGE_MAIN__

#ifdef _WIN32
#include <windows.h>
#else
#endif
#include <Engine2Main.h>

#ifdef _WIN32
    #ifdef BUILD_DLL_IMAGE
        #define IMAGE_API __declspec(dllexport)
    #else
        #define IMAGE_API __declspec(dllimport)
    #endif
#else
    #define IMAGE_API
#endif

#include "FreeImage.h"
#include "Image.h"

#ifdef __cplusplus
extern "C"
{
#endif

IMAGE_API DRIImage* getInstance();
IMAGE_API DRIImage* getInstances(int count);

IMAGE_API void releaseInstance(DRInterface* data);
IMAGE_API void releaseInstances(DRInterface* datas);

#ifdef __cplusplus
}
#endif



#endif //__DR_IMAGE_MAIN__
