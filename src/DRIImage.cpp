#include "Engine2Main.h"

DRIImage::DRIImage()
{

}

DRIImage::~DRIImage()
{
}


DRIImage* DRIImage::newImage()
{
   // printf("operator new!\n");
    DRIImage* out = (DRIImage*)DRInterface::Instance(__IMAGE_DLL_NAME__);
    if(!out) return new DRIImage;
   // printf("out size: %d, size: %uld\n", sizeof(out));
    return out;
}

void DRIImage::deleteImage(DRIImage* pData)
{
    //printf("operator delete!\n");
    DRInterface::releaseMem((DRInterface*)pData, __IMAGE_DLL_NAME__);
}

DRReturn DRIImage::loadFromFile(const char* filename)
{
    LOG_WARNING("Not implemented!");
    return DR_OK;
}

DRReturn DRIImage::saveIntoFile(const char* filename)
{
    LOG_WARNING("Not implemented!");
    return DR_OK;    
}

GLenum DRIImage::getImageFormat()
{
    LOG_WARNING("Not implemented!");
    return 0;
}

void DRIImage::setImageFormat(GLenum format)
{
    LOG_WARNING("Not implemented!");
}

u32 DRIImage::getWidth()
{
    LOG_WARNING("Not implemented!");
    return 0;
}

void DRIImage::setWidth(u32 width)
{
    LOG_WARNING("Not implemented!");
}

u32 DRIImage::getHeight()
{
    LOG_WARNING("Not implemented!");
    return 0;
}

void DRIImage::setHeight(u32 height)
{
    LOG_WARNING("Not implemented!");
}

u8* DRIImage::getPixel()
{
    LOG_WARNING("Not implemented!");
    return NULL;
}

void DRIImage::getPixel(DRColor* buffer)
{
    LOG_WARNING("Not implemented!");
}

void DRIImage::setPixel(u8* pixel)
{
    LOG_WARNING("Not implemented!");
}

void DRIImage::setSize(DRVector2 size)
{
    LOG_WARNING("Not implemented!");
}

DRVector2 DRIImage::getSize()
{
    LOG_WARNING("Not implemented!");
    return DRVector2(0.0f);
}

void DRIImage::setPixel(DRColor* pixel)
{
    LOG_WARNING("Not implemented!");    
}