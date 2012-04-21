#ifndef __NETWORK_MAIN_H__
#define __NETWORK_MAIN_H__

#ifdef _WIN32
#include <windows.h>
#else
#endif
#include <Engine2Main.h>
#include <SDLNet/SDL_net.h>


#ifdef _WIN32
    #ifdef BUILD_DLL_NETWORK
        #define NETWORK_API __declspec(dllexport)
    #else
        #define NETWORK_API __declspec(dllimport)
    #endif
#else
    #define NETWORK_API
#endif


#include "Network.h"

#ifdef __cplusplus
extern "C"
{
#endif

NETWORK_API DRINetwork* getInstance();

#ifdef __cplusplus
}
#endif

#endif // __NETWORK_MAIN_H__
