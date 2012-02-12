#include "ImageMain.h"

DRImage::DRImage()
: mImage(NULL), mFilename(""), mSize(0.0f), mImageFormat(0), mLoadedSucessfully(false)
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

const char* DRImage::imageFormatName(FREE_IMAGE_FORMAT type)
{
    switch(type)
    {
        case FIF_UNKNOWN:       return "unbekanntes Format";
        case FIF_BMP:           return "Bitmap";
        case FIF_JPEG:          return "JPEG";
        case FIF_PNG:           return "PNG";
        case FIF_TARGA:         return "tga (Targa)";
        case FIF_GIF:           return "gif";
        default: "- unknow -";
    }
    return "- nothing -";
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

DRReturn DRImage::saveIntoFile(const char* filename)
{
    if(!mLoadedSucessfully) LOG_ERROR("not image", DR_ERROR);
    
    FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename);
    //if still unkown, return failure
    if(fif == FIF_UNKNOWN)
        LOG_ERROR("Unknown File Format", DR_ERROR);
    if(fif == FIF_JPEG)
    {
        FIBITMAP* t = FreeImage_ConvertTo24Bits(mImage);
        FreeImage_Unload(mImage);
        mImage = t;
    }
    //FIBITMAP* t = FreeImage_Rotate(mImage, 0);
    //FreeImage_Unload(mImage);
    //mImage = t;
    FreeImage_FlipVertical(mImage);
    
    if(!FreeImage_Save(fif, mImage, filename, 0 ))
        LOG_ERROR("image couldn't be saved", DR_ERROR);
    
    return DR_OK;
}

GLenum DRImage::getImageFormat() const
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

u32 DRImage::getWidth() const
{
    if(!mLoadedSucessfully) LOG_ERROR("not loaded", 0);
    return FreeImage_GetWidth(mImage);
}

u32 DRImage::getHeight() const
{
    if(!mLoadedSucessfully) LOG_ERROR("not loaded", 0);
    return FreeImage_GetHeight(mImage);
}

DRVector2 DRImage::getSize() const
{
    if(!mLoadedSucessfully) LOG_ERROR("not loaded", DRVector2(0.0f));
    return DRVector2((DRReal)FreeImage_GetWidth(mImage), (DRReal)FreeImage_GetHeight(mImage));
}

void DRImage::setSize(DRVector2 size)
{
    mSize = size;
}

u8* DRImage::getPixel()
{
    if(!mLoadedSucessfully) LOG_ERROR("not loaded", NULL);
    return FreeImage_GetBits(mImage);
}

void DRImage::getPixel(DRColor* buffer)
{
    if(!mLoadedSucessfully) LOG_ERROR_VOID("not loaded");
    //FIBITMAP* b = FreeImage_ConvertTo32Bits(mImage);
    FIBITMAP *red = NULL, *green = NULL, *blue = NULL;
     if(!(red  = FreeImage_GetChannel(mImage, FICC_RED )))
        LOG_WARNING("Fehler bei get red channel!");
    if(!(blue = FreeImage_GetChannel(mImage, FICC_BLUE)))
        LOG_WARNING("Fehler bei get blue channel!");
    if(!(green = FreeImage_GetChannel(mImage, FICC_GREEN)))
        LOG_WARNING("Fehler bei get green channel!");
    
    u8* rp = (u8*)FreeImage_GetBits(red);
    u8* gp = (u8*)FreeImage_GetBits(green);
    u8* bp = (u8*)FreeImage_GetBits(blue);
    u32 size = getWidth()*getHeight();
    for(u32 i = 0; i < size; i++)
    {
        buffer[i].b = (float)(rp[i])/255.0f;
        buffer[i].g = (float)(gp[i])/255.0f;
        buffer[i].r = (float)(bp[i])/255.0f;
    }
    
   // FreeImage_Unload(b);  
    FreeImage_Unload(red);
    FreeImage_Unload(green);
    FreeImage_Unload(blue);
}

void DRImage::setWidth(u32 width)
{
    //if(!mLoadedSucessfully) LOG_ERROR("not loaded", NULL);
    mSize.x = (DRReal)width;
}

void DRImage::setHeight(u32 height)
{
    mSize.y = (DRReal)height;
}

void DRImage::setImageFormat(GLuint format)
{
    mImageFormat = format;
}

void DRImage::setPixel(u8* pixel)
{
    if(mLoadedSucessfully)
    {
        mLoadedSucessfully = false;
        if(mImage)
        {
            FreeImage_Unload(mImage);
            mImage = NULL;
        }
    }
    FIBITMAP* t = FreeImage_ConvertFromRawBits(pixel, (u32)mSize.x, (u32)mSize.y, (u32)mSize.x*4, 32, FI_RGBA_BLUE_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_RED_MASK, TRUE);
    mImage = FreeImage_ConvertTo32Bits(t);
    
    FreeImage_Unload(t);

    if(!mImage)
        LOG_ERROR_VOID("Fehler beim erstellen des Images");
    
    mLoadedSucessfully = true; 
}

void DRImage::setPixel(DRColor* pixel)
{
    if(!pixel) LOG_ERROR_VOID("Zeropointer");
    
    if((u32)mSize.x <= 0 || (u32)mSize.y <= 0) LOG_ERROR_VOID("Ungueltige Groesse");
    //printf("mSize.x <= 0.0f: %d, mSize.x >= 0.0f: %d, (int)mSize.x == 0: %d, (int)mSize.x: %d\n", mSize.x <= 0.0f, mSize.x >= 0.0f, (int)mSize.x == 0, (int)mSize.x);
    
  //  printf("try to allocate %d Byte, x: %d, y: %d, sizeof(u32):%d\n", (u32)mSize.x*(u32)mSize.y*sizeof(u32), (u32)mSize.x, (u32)mSize.y, sizeof(u32));
    u8* temp = (u8*)malloc(sizeof(u32)*(u32)mSize.x*(u32)mSize.y);
    u32* t2 = (u32*)temp;
    if(!temp) LOG_ERROR_VOID("Fehler bei malloc");
    int i = 0;
    for(i = 0; i < mSize.x*mSize.y; i++)
    {
        t2[i] = (u32)pixel[i];
    }
    setPixel(temp);
    if(temp)
        free(temp);
    else
        LOG_ERROR_VOID("critical!");
}