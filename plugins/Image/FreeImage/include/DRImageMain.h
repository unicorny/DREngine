#ifndef __DR_IMAGE_MAIN__
#define __DR_IMAGE_MAIN__

#ifdef _WIN32
#include <windows.h>
#endif

#include "DREngine/DRIImage.h"
#include "DRImage/export.h"

#ifdef __cplusplus
extern "C"
{
#endif

DRIMAGE_EXPORT DRIImage* getInstance();
DRIMAGE_EXPORT DRIImage* getInstances(int count);

DRIMAGE_EXPORT void releaseInstance(DRInterface* data);
DRIMAGE_EXPORT void releaseInstances(DRInterface* datas);

#ifdef __cplusplus
}
#endif



#endif //__DR_IMAGE_MAIN__
