#include "Engine2Main.h"

DRTextur::DRTextur()
: mParent(DRIImage::newImage()), mTexturID(0)
{
}

DRTextur::DRTextur(const char* filename, bool keepImage /* = false */)
: mParent(DRIImage::newImage()), mTexturID(0)
{
    load(filename, keepImage);
}

DRTextur::~DRTextur()
{
    unload();
}

DRReturn DRTextur::load(const char* filename, bool keepImage /* = false*/)
{
    if(mTexturID != 0)
        unload();
    
    if(!mParent)
        mParent = DRIImage::newImage();
    if(mParent->loadFromFile(filename))
        LOG_ERROR("Fehler beim Textur laden", DR_ERROR);

    //generate an OpenGL texture ID for this texture
	glGenTextures(1, &mTexturID);
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, mTexturID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//store the texture data for OpenGL use
/*	glTexImage2D(GL_TEXTURE_2D, 0, uiNumComponents, width, height,
		0, imageFormat, GL_UNSIGNED_BYTE, bits);
		*/
    GLenum format = mParent->getImageFormat();
    unsigned numComponents = 0;
    if(format == GL_RGB) numComponents = 3;
    else if(format == GL_RGBA) numComponents = 4;
    else LOG_ERROR("ungueltiges Format", DR_ERROR);

	glTexImage2D(GL_TEXTURE_2D, 0, numComponents, mParent->getWidth(), mParent->getHeight(),
		0, format, GL_UNSIGNED_BYTE, mParent->getPixel());

    if(DRGrafikError("Fehler beim erstellen einer Textur!")) return DR_ERROR;
    DRLog.writeToLog("Textur, format: %d, components: %d, rgb: %d, rgba: %d, width: %d, height: %d",
                      format, numComponents, GL_RGB, GL_RGBA, mParent->getWidth(), mParent->getHeight());

    if(!keepImage)
    {
        DRIImage::deleteImage(mParent);
        mParent = NULL;
    }

    return DR_OK;
}

void DRTextur::unload()
{
    if(mParent)
    {
        DRIImage::deleteImage(mParent);
        mParent = NULL;
    }
    glDeleteTextures(1, &mTexturID);
    mTexturID = 0;
}

DRReturn DRTextur::bind()
{
    glEnable(GL_TEXTURE_2D);
    //bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, mTexturID);

	if(DRGrafikError("bind texture")) return DR_ERROR;

    return DR_OK;
}
