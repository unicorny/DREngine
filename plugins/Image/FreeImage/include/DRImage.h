#ifndef __DR_IMAGE__
#define __DR_IMAGE__

#define FREEIMAGE_LIB
#include "FreeImage.h"
#include "DREngine/DRIImage.h"
#include "DRImage/export.h"

#include <string>

class DRIMAGE_EXPORT DRImage : public DRIImage
{
public:
    DRImage();
    virtual ~DRImage();

    // Funktionen
    virtual DRReturn loadFromFile(const char* filename);
    virtual DRReturn saveIntoFile(const char* filename);
    virtual GLenum getImageFormat() const;
    virtual void setImageFormat(GLenum format);
    
    virtual u32 getWidth() const;
    virtual void setWidth(u32 width);
    virtual u32 getHeight() const;
    virtual void setHeight(u32 height);
    virtual DRVector2i getSize() const;
    virtual void setSize(DRVector2i size);
    virtual u8* getPixel();
    virtual void getPixel(DRColor* buffer);
    virtual void setPixel(u8* pixel);
    virtual void setPixel(DRColor* pixel);

protected:
    //Hilfsfunktionen
    static const char* colorTypeName(FREE_IMAGE_COLOR_TYPE type);
    static const char* imageFormatName(FREE_IMAGE_FORMAT type);

private:
    FIBITMAP* mImage;
    std::string mFilename;
    //daten für selbst angelegte Bilder
    DRVector2i mSize;
    GLuint mImageFormat;

    bool      mLoadedSucessfully;



};
//*/
#endif //__DR_IMAGE__
