#ifndef __DR_ENGINE_PLUGINS_IMAGE_FREEIMAGE_EXPORT_H__
#define __DR_ENGINE_PLUGINS_IMAGE_FREEIMAGE_EXPORT_H__

#ifdef _WIN32
    #ifdef BUILD_DLL_IMAGE
        #define IMAGE_API __declspec(dllexport)
    #else
        #define IMAGE_API __declspec(dllimport)
    #endif
#else
    #define IMAGE_API
#endif

#endif //__DR_ENGINE_PLUGINS_IMAGE_FREEIMAGE_EXPORT_H__