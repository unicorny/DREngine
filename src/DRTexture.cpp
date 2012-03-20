//#include "Texture.h"
#include "Engine2Main.h"


DRTexture::DRTexture(const char* filename)
: mTexturID(0), mPboID(0), mFilename(filename), mLoadingState(0), 
  mImage(NULL),  mSize(0, 0)
{
	//glGenTextures(1, &mTexturID);
    //bind to the new texture ID
    //glBindTexture(GL_TEXTURE_2D, mTexturID);   

	glGenBuffersARB(1, &mPboID);
}

DRTexture::DRTexture(GLuint textureID, GLubyte* data/* = NULL*/, GLint dataSize/* = 0*/,
    GLint dataType/* = GL_UNSIGNED_BYTE*/)
: mTexturID(textureID), mPboID(0), mFilename(""), mLoadingState(0),
  mImage(NULL), mSize(0, 0)
{
    GLint format, format2 = GL_RGB;
    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &mSize.x);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &mSize.y);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
    
	if(format == 4) format2 = GL_RGBA;
   
    if(data && dataSize)
	{
		glGenBuffersARB(1, &mPboID);
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPboID);	

		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, 
			data, GL_STREAM_DRAW_ARB);	

        glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0, mSize.x, mSize.y, 
                    format2, dataType, NULL);
	}
	
    if(DRGrafikError("[Texture::Texture] 2 glTexSubImage2D"))
        printf("size: %d %d, format: %d, format2: %x\n", mSize.x, mSize.y, format, format2);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	//GlobalRenderer::Instance().addGrafikMemTexture(mMemorySize);
	mLoadingState = 3;	
}

DRTexture::DRTexture(DRVector2i size, GLuint format, GLubyte* data, GLint dataSize)
: mTexturID(0), mPboID(0), mFilename(""), mLoadingState(0),
  mImage(NULL), mSize(size)
{
	//glGenTextures(1, &mTexturID);
    mTexturID = DRTextureManager::Instance()._getTexture(size, format);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint format2 = GL_RGB;
	if(format == 4) format2 = GL_RGBA;

	if(data && dataSize)
	{
		glGenBuffersARB(1, &mPboID);
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPboID);	

		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, 
			data, GL_STREAM_DRAW_ARB);	
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0, mSize.x, mSize.y, 
                    format2, GL_UNSIGNED_BYTE, NULL);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	mLoadingState = 3;	
}

DRTexture::~DRTexture()
{
	if(mTexturID)
	{
        DRTextureManager::Instance().freeTexture(mTexturID);
		mTexturID = 0;
	}	
	if(mPboID)
	{
		glDeleteBuffersARB(1, &mPboID);
		mPboID = 0;
	}
	
	if(mImage)
	{
		DRIImage::deleteImage(mImage);
		mImage = NULL;
	}
	
}

DRReturn DRTexture::loadFromFile()
{
	mImage = DRIImage::newImage();
	const char* path = DRFileManager::Instance().getWholePfad(mFilename.data());
	if(!path)
	{
		if(mImage->loadFromFile(mFilename.data()))
			LOG_ERROR("Fehler beim Textur laden", DR_ERROR);
	}
	else
	{
		if(mImage->loadFromFile(DRString(DRString(path)+"/"+mFilename).data()))
			LOG_ERROR("Fehler2 beim Textur laden", DR_ERROR);
	}
    mSize = mImage->getSize();
	mLoadingState = 1;
	return DR_OK;
}

DRReturn DRTexture::pixelsCopyToRenderer()
{
	if(!mImage) LOG_ERROR("fehler, mImage is zero", DR_NOT_ERROR);
	if(isLoadingError()) LOG_ERROR("error by loading", DR_ERROR);
    
    GLenum format = mImage->getImageFormat();
	unsigned numComponents = 0;
	if(format == GL_RGB) numComponents = 3;
	else if(format == GL_RGBA) numComponents = 4;
	else LOG_ERROR("ungueltiges Format", DR_ERROR);
    
    if(!mTexturID) 
        mTexturID = DRTextureManager::Instance()._getTexture(mImage->getSize(), numComponents);

	glBindTexture(GL_TEXTURE_2D, mTexturID);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPboID);
    
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, mImage->getWidth()*mImage->getHeight()*numComponents, 
					mImage->getPixel(), GL_STREAM_DRAW_ARB);

	glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0, mImage->getWidth(), mImage->getHeight(), 
                    format, GL_UNSIGNED_BYTE, NULL);

	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	if(DRGrafikError("[Texture::pixelsCopyToRenderer] Error by copying pixels to OpenGL")) return DR_ERROR;

	DRIImage::deleteImage(mImage); mImage = NULL;	
	//printf("[Texture::pixelsCopyToRenderer] texture load\n");
	
	mLoadingState = 2;

	return DR_OK;
}

void DRTexture::bind()
{
	if(mLoadingState >= 2)
	{
		glBindTexture(GL_TEXTURE_2D, mTexturID);
        DRGrafikError("[Texture::bind]");
	}
    else
    {
        //DRLog.writeToLog("[Texture::bind] texture not load");
    }
}

void DRTexture::setWrappingMode(GLint mode)
{
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
}

void DRTexture::setFilter(GLint glMinFilter/* = GL_LINEAR*/, GLint glMagFilter/* = GL_LINEAR*/)
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilter);
}

DRVector2i DRTexture::getResolution()
{
	return mSize;
	if(mLoadingState < 2) return DRVector2i(-1, -1);
	
	DRVector2i v;
	glBindTexture(GL_TEXTURE_2D, mTexturID);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &v.x);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &v.y);
	DRGrafikError("[Texture::getResolution]");
	return v;
}