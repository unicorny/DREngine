#include "ImageMain.h"

DRImage::DRImage()
: mImage(NULL), mFilename(""), mLoadedSucessfully(false)
{
  //  LOG_INFO("constructer called");
}

DRImage::~DRImage()
{
    if(mImage)
    {
        FreeImage_Unload(mImage);
        mImage = NULL;
    }
   // LOG_INFO("deconstructor called");
}

//**********************************************************************************************************************
const char* DRImage::colorTypeName(FREE_IMAGE_COLOR_TYPE type)
{
    switch(type)
    {
    case FIC_MINISBLACK: return "Monochrome bitmap (1-bit): first palette entry is black.\n \
                                 Palletised bitmap (4 or 8-bit) and single channel non standard bitmap:\n\
                                 the bitmap has a greyscale palette";
    case FIC_MINISWHITE: return "Monochrome bitmap (1-bit): first palette entry is white.\n\
                                 Palletised bitmap (4 or 8-bit):\n\
                                 the bitmap has an inverted greyscale palette";
    case FIC_PALETTE:     return "Palettized bitmap (1, 4 or 8 bit)";
    case FIC_RGB:         return "High-color bitmap (16, 24 or 32 bit), RGB16 or RGBF";
    case FIC_RGBALPHA:    return "High-color bitmap with an alpha channel (32 bit bitmap, RGB16 or RGBAF";
    case FIC_CMYK:        return "CMYK bitmap (32 bit only)";
    default: return "-- unknown color type --";
     }

     return "--error--";
}

//*********************************************************************************************************************

DRReturn DRImage::loadFromFile(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    mFilename = filename;
    if(!file)
    {
        DRLog.writeToLog("[DRImage::loadFromFile] Fehler, %s konnte nicht geoffnet werden", filename);
        return DR_ERROR;
    }
    fclose(file);


    if(mLoadedSucessfully)
    {
        mLoadedSucessfully = false;
        if(mImage)
        {
            FreeImage_Unload(mImage);
            mImage = NULL;
        }
    }
     //image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	//color type
	//FREE_IMAGE_COLOR_TYPE colorType; mColorType
	//pointer to the image, once loaded
	//FIBITMAP *dib(0); mImage
	//pointer to the image data
	//BYTE* bits(0); mImageData
	//image width and height
	//unsigned int width(0), height(0);
	//mWidth, mHeight

	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename, 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(filename);
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		LOG_ERROR("Unknown File Format", DR_ERROR);

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		mImage = FreeImage_Load(fif, filename);
	//if the image failed to load, return failure
	if(!mImage)
		LOG_ERROR("Image failed to Load, FIF Support failed?", DR_ERROR);

	//retrieve the image data
//	mImageData = FreeImage_GetBits(mImage);
	//get the image width and height
	//mWidth = FreeImage_GetWidth(mImage);
//	mHeight = FreeImage_GetHeight(mImage);

    //get the color type
//	mColorType = FreeImage_GetColorType(mImage);
	//if this somehow one of these failed (they shouldn't), return failure
/*	if((mImageData == 0) || (mWidth == 0) || (mHeight == 0))
	{
		//Free FreeImage's copy of the data
		FreeImage_Unload(mImage);
		mImageData = NULL;
		LOG_ERROR("bits, width or height is zero", -1);
	}
//*/
    //Swap Colors to get RGB
    FIBITMAP *red = NULL, *blue = NULL;
    if(!(red  = FreeImage_GetChannel(mImage, FICC_RED )))
        LOG_WARNING("Fehler bei get red channel!");
    if(!(blue = FreeImage_GetChannel(mImage, FICC_BLUE)))
        LOG_WARNING("Fehler bei get blue channel!");

    if(!FreeImage_SetChannel(mImage, blue, FICC_RED ))
        LOG_WARNING("Fehler bei set Blue channel to red!");
    if(!FreeImage_SetChannel(mImage, red , FICC_BLUE))
        LOG_WARNING("Fehler bei set Red Channel to blue!");

    FreeImage_Unload(red);
    FreeImage_Unload(blue);

	mLoadedSucessfully = true;
	//LOG_INFO("Image loaded sucessfully");

    return DR_OK;
}

GLenum DRImage::getImageFormat()
{
    if(!mLoadedSucessfully) LOG_ERROR("not loaded", 0);
    FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(mImage);
    GLenum    imageFormat = 0;//GL_BGR;
	if(colorType == FIC_RGBALPHA)
	{
		imageFormat = GL_RGBA;
	}
	else if(colorType == FIC_PALETTE)
	{
        imageFormat = GL_COLOR_INDEX;
	}
	else if(colorType == FIC_RGB)
	{
		imageFormat = GL_RGB;
	}
	return imageFormat;
}

unsigned int DRImage::getWidth()
{
    if(!mLoadedSucessfully) LOG_ERROR("not loaded", 0);
    return FreeImage_GetWidth(mImage);
}

unsigned int DRImage::getHeight()
{
    if(!mLoadedSucessfully) LOG_ERROR("not loaded", 0);
    return FreeImage_GetHeight(mImage);
}

unsigned char* DRImage::getPixel()
{
    if(!mLoadedSucessfully) LOG_ERROR("not loaded", NULL);
    return FreeImage_GetBits(mImage);
}
