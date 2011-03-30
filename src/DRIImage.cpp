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

GLenum DRIImage::getImageFormat()
{
    LOG_WARNING("Not implemented!");
    return 0;
}

unsigned int DRIImage::getWidth()
{
    LOG_WARNING("Not implemented!");
    return 0;
}
unsigned int DRIImage::getHeight()
{
    LOG_WARNING("Not implemented!");
    return 0;
}
unsigned char* DRIImage::getPixel()
{
    LOG_WARNING("Not implemented!");
    return NULL;
}
