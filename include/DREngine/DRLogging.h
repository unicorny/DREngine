#ifndef __DR_ENGINE_LOGGING_H
#define __DR_ENGINE_LOGGING_H

#include "DRCore2/DRCore2Main.h"
#include "DREngine/export.h"

#define LOG_SDL_INTERN(text, f, l, fu) DREngineLog.writeToLog("<tr><td><font size=\"2\"><b><font color=\"#BF8000\">SDL Fehler:</font></b> %s</font></td><td><font size=\"2\"> (<i>%s</i>, Zeile <i>%d</i>, Funktion <i>%s</i>)</font></td></tr>", text, f, l, fu)
#define LOG_ERROR_SDL(r) {const char* pcErrorSDL = SDL_GetError(); if(strlen(pcErrorSDL) > 2){ LOG_SDL_INTERN(pcErrorSDL, DRRemoveDir(__FILE__), __LINE__, __FUNCTION__); return r;}}
#define LOG_ERROR_SDL_VOID() {const char* pcErrorSDL = SDL_GetError(); if(strlen(pcErrorSDL) > 2){ LOG_SDL_INTERN(pcErrorSDL, DRRemoveDir(__FILE__), __LINE__, __FUNCTION__); return;}}
#define LOG_WARNING_SDL() {const char* pcErrorSDL = SDL_GetError(); if(strlen(pcErrorSDL) > 2) LOG_SDL_INTERN(pcErrorSDL, DRRemoveDir(__FILE__), __LINE__, __FUNCTION__);}

//! Fragt nach einem Fehler in OpenGL
DRENGINE_EXPORT DRReturn DRGrafikError(const char* pcErrorMessage);

#endif //__DR_ENGINE_LOGGING_H
