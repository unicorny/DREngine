/*/*************************************************************************
 *                                                                         *
 * DREngine, Engine for my programs, using SDL and OpenGL                 *
 * Copyright (C) 2012, 2013, 2014 github.com/unicorny                      *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * any later version.                                                      *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 *                                                                         *
 ***************************************************************************/


#ifndef __DR_ENGINE2_MAIN__
#define __DR_ENGINE2_MAIN__

 // intern imports
#include "DRCore2/DRTypes.h" 
#include "DRCore2/Foundation/DRUtils.h"
#include "DRCore2/Foundation/DRVector2.h"
#include "DRCore2/Manager/DRGameStateManager.h"

#include "DREngine/export.h"
#include "DRVideoConfig.h"
#include "OpenGL.h"

// globals
#define XWIDTH g_v2WindowLength.x
#define YHEIGHT g_v2WindowLength.y

#include <string>

//----------------------------------------------------------------------------------------------------------------------
DRENGINE_EXPORT extern SDL_Window* g_pSDLWindow;
DRENGINE_EXPORT extern SDL_GLContext g_glContext;
DRENGINE_EXPORT extern DRVector2  g_v2WindowLength;
DRENGINE_EXPORT extern Uint8*    g_piPressed;
DRENGINE_EXPORT extern u16       g_CPU_Count;
DRENGINE_EXPORT extern DRGameStateManager* g_pGameStateManager;


/**	Engine Funktionen
	Verschieden Init-Funktionen und anderes
	Init alleine für SDL
	für SDL und OpenGL
	für SDL und Ogre3D
*/
DRENGINE_EXPORT DRReturn EnInit_Simple(DRReal fVersion = 1.0f, bool initSound = false);
DRENGINE_EXPORT DRReturn EnInit(DRReal fVersion = 1.0f, bool initSound = false);
DRENGINE_EXPORT DRReturn EnInit_INI(const char* iniFileName);
DRENGINE_EXPORT DRReturn EnInit_OpenGL(DRReal fVersion = 1.0f, DRVideoConfig video = DRVideoConfig(), const char* pcTitel = "OpenGL Render Fenster", const char* pcBMPIcon = NULL, bool bInitSound = true);
DRENGINE_EXPORT DRReturn EnGameLoop(DRGameStatePtr firstGameState, bool bExitByEsc = true);
DRENGINE_EXPORT DRReturn EnGameLoop(DRReturn (*pMoveProc)(DRReal), DRReturn (*pRenderProc)(DRReal), bool bExitByEsc = true);
DRENGINE_EXPORT void     EnExit();

DRENGINE_EXPORT void	 EnPostExitMessageToSDL();
DRENGINE_EXPORT DRReal   EnSDL_Loop();
DRENGINE_EXPORT bool	 EnIsButtonPressed(SDL_Scancode button);
DRENGINE_EXPORT std::string EnGetTimeSinceStart();


#endif //__DR_ENGINE2_MAIN__
