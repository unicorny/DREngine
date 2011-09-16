/*/*************************************************************************
 *                                                                       *
 * EngineDLL, Engine for my programs, using SDL, OpenGL, OpenAL			 *
 * Copyright (C) 2007, 2008, 2009 Dario Rekowski.						 *
 * Email: dariofrodo@gmx.de   Web: www.mathe-programme.de.tk             *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * any later version.													 *
 *																		 *
 * This program is distributed in the hope that it will be useful,		 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of		 *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the		 *
 * GNU General Public License for more details.							 *
 *																		 *
 * You should have received a copy of the GNU General Public License	 *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *                  
 *                                                                       *
 *************************************************************************/


/*
Programmierer: Dario Rekowski
Nach einem NeHe Tutorial von Paul Frazee (VertexBuffer)
*/
// VBO Extension Definitionen, aus der glext.h
//#define GL_ARRAY_BUFFER_ARB 0x8892
//#define GL_STATIC_DRAW_ARB 0x88E4
//#define GL_SAMPLE_BUFFERS_ARB 0x2041
//#define GL_SAMPLES_ARB 0x2042

#ifndef __DR_GL_EXTENSIONS_H
#define __DR_GL_EXTENSIONS_H

// VBO Extension Funktionszeiger für VertexBuffer
extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;  // VBO Namens Generations-Prozedur
extern PFNGLBINDBUFFERARBPROC glBindBufferARB; // VBO Bind-Prozedur 
extern PFNGLBUFFERDATAARBPROC glBufferDataARB;  // VBO Daten-Lade-Prozedur 
extern PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;  // VBO Sub-Daten-Lade-Prozedur 
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB; // VBO Lösch-Prozedur 

//schauen, ob diese Erweiterung im aktuellem System unterstützt wird
ENGINE_API bool DRIsExtensionSupported(const char* szTargetExtension );

//VertexBuffer Erweiterung aktivieren
ENGINE_API DRReturn DRActivateVBExtension();

ENGINE_API void DRWriteExtensionsToLog(const char* pcExtensions, FILE* pFile = NULL);

#endif