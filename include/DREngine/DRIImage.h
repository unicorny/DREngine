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
    Eine Klasse welche ein Bild repr�sentiert,
    welches zum Beispiel als Textur verwendet werden kann
    @author Dario Rekowski
    @date 05.11.10
*/

#ifndef __DR_ENGINE2_IIMAGE__
#define __DR_ENGINE2_IIMAGE__
#ifdef _WIN32
	#ifdef _MSC_VER
		#define __IMAGE_DLL_NAME__ "DRImage.dll"
	#else
		#define __IMAGE_DLL_NAME__ "libDRImage.dll"
	#endif
#else
	#define __IMAGE_DLL_NAME__ "./libDRImage.so"
#endif

#include "OpenGL.h"
#include "DRInterface.h"
#include "DRCore2/Foundation/DRVector2i.h"
#include "DRCore2/Foundation/DRColor.h"

class DRENGINE_EXPORT DRIImage : public DRInterface
{
public:
    DRIImage();
    virtual ~DRIImage();

    static DRIImage* newImage();
    static void deleteImage(DRIImage *pData);

    virtual GLenum getImageFormat() const;
    virtual void setImageFormat(GLenum format);
    virtual u32 getWidth() const;
    virtual void setWidth(u32 width);
    virtual u32 getHeight() const;
    virtual void setHeight(u32 height);
    virtual void setSize(DRVector2i size);
    virtual DRVector2i getSize() const;
    virtual u8* getPixel();
    virtual void getPixel(DRColor* buffer);
    //! \brief setzt neue Pixel f&uuml;r das Bild
    //!
    //! Dabei wird ein Pixelarray erwartet der gr&ouml;&szlig;e width*height*u32
    //! width und height müssen vorher gesetzt werden!
    //! \param pixel zeigt auf einen Puffer gefüllt mit Pixeln, Daten werden kopiert,
    //! Puffer kann danach freigegeben werden
    virtual void setPixel(u8* pixel);
    virtual void setPixel(DRColor* pixel);


    //! Functionen
    virtual DRReturn loadFromFile(const char* filename);
    virtual DRReturn saveIntoFile(const char* filename);
protected:


private:

};

#endif //__DR_ENGINE2_IIMAGE__
