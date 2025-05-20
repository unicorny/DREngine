#ifndef __DR_IMAGE_MAIN__
#define __DR_IMAGE_MAIN__

#ifdef _WIN32
#include <windows.h>
#endif

#include "DREngine/DRIImage.h"
#include "export.h"

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
