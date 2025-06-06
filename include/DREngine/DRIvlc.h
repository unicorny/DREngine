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

#ifndef __DR_ENGINE2_Ivlc__
#define __DR_ENGINE2_Ivlc__

#define __vlc_DLL_NAME_ "libvlc.dll"
/*
Interface zum einfachen abspielen von Videos durch vlc
TODO: auch sound abspielen
Autor: Dario Rekowski
Datum: 11.11.09
*/

#include "DREngine/export.h"
#include "DRCore2/DRTypes.h"
#ifdef _WIN32 
#include "windows.h"
#endif

class DRENGINE_EXPORT DRIvlc
{
public:
    ~DRIvlc() {if(mInitalized) exit();}

    // Zugrifffunktionen
    static DRIvlc& Instance();

    static inline DRIvlc& getSingleton() {return Instance();}
    static inline DRIvlc* getSingletonPtr() {return &Instance();}


    void exit();
    DRReturn init();

private:
    DRIvlc();
#ifdef _WIN32
    static HMODULE mDLL;
#else
    static void* mDLL;
#endif

    bool mInitalized;
};

#endif //__DR_ENGINE2_Ivlc__
