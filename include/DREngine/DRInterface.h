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

/*!
    Standard Interface Klasse, parent von allen Interface Klassen,
    welche eine externe dll dynamisch einbinden
*/

#ifndef __DR_ENGINE2_INTERFACE__
#define __DR_ENGINE2_INTERFACE__

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif //_WIN32

#include "DREngine/export.h"
#include "DRCore2/DRTypes.h"

class DRENGINE_EXPORT DRInterface
{
public:
    virtual ~DRInterface();

    // Zugrifffunktionen
    static DRInterface* Instance(const char* dllName);

    static void close();

    static void releaseMem(DRInterface* data, const char* dllname);

protected:
    DRInterface();
    static DRReturn loadDll(const char* dllname);

#ifdef _WIN32
    static HMODULE mDLL;
#else
    static void* mDLL;
#endif


private:
};
#endif //__DR_ENGINE2_INTERFACE__
