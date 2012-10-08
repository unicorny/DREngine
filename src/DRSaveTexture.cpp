#include "Engine2Main.h"

 DRSaveTexture::DRSaveTexture(const char* savingPath, DRTextureBufferPtr textureBuffer, GLuint stepSize/* = 16384*/)
 : mPboSaveID(0), mTextureBuffer(textureBuffer), mSavingState(0), mSavingCursor(0), mSavingBuffer(NULL),
   mFilename(savingPath), mImage(NULL), mSize(0), mStepSize(stepSize)
 {
     glGenBuffersARB(1, &mPboSaveID);
 }
 DRSaveTexture::~DRSaveTexture()
 {
    if(mSavingBuffer)
	{
		DR_SAVE_DELETE_ARRAY(mSavingBuffer);
	}
    if(mImage)
	{
		DRIImage::deleteImage(mImage);
		mImage = NULL;
	}
    if(mPboSaveID)
    {
        LOG_WARNING("PBO wasn't deleted");
        glDeleteBuffersARB(1, &mPboSaveID);
    }
 }

DRReturn DRSaveTexture::getPixelsToSave()
{
	if(!mPboSaveID)
	{
		glGenBuffersARB(1, &mPboSaveID);
	}
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &mSize.x);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &mSize.y);
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, mPboSaveID);
	//printf("8Texture::getPixelsToSave] size: %d, %d\n", size(0), size(1));
	mSavingBuffer = new u8[mSize.x*mSize.y*4];
	mSavingCursor = 0;
	glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, mSize.x*mSize.y*4*sizeof(u8), NULL, GL_STREAM_READ_ARB);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0);
	//glReadPixels(0, 0, size(0), size(1), GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	DRGrafikError("[Texture::getPixelsToSave] glGetTexImage");
		
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
	
	if(DRGrafikError("[Texture::getPixelsToSave()] error by asynchronously saving an image!"))
		LOG_ERROR("Fehler bei save image", DR_ERROR);
	mSavingState = 1;

	return DR_OK;
}

DRReturn DRSaveTexture::putPixelsToImage()
{
    if(mSavingState == 2) return DR_OK;
	if(!mPboSaveID) return DR_ZERO_POINTER;
	if(!mImage) mImage = DRIImage::newImage();
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, mPboSaveID);
	GLubyte* ptr = static_cast<GLubyte*>(glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB));
	DRGrafikError("[Texture::putPixelsToImage] map Buffer");
	//if(!ptr) LOG_ERROR("glMapBuffer return ZERO Pointer", DR_ERROR);
	if(ptr)
	{
		GLuint bufferSize = mSize.x*mSize.y*4*sizeof(u8);
        //memcpy(mSavingBuffer, ptr, bufferSize);
        //mSavingState = 2;
        
		//printf("\rstepSize KByte: %d, KBytes left: %d", stepSize/1024, (bufferSize-mSavingCursor)/1024);
		if(mSavingCursor+mStepSize >= bufferSize)
		{
			memcpy(&mSavingBuffer[mSavingCursor], &ptr[mSavingCursor], bufferSize-mSavingCursor);
            mSavingState = 2;
		}
		else
		{
			memcpy(&mSavingBuffer[mSavingCursor], &ptr[mSavingCursor], mStepSize);
			mSavingCursor += mStepSize;
		}
//*/
		glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB);
	}

	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
    if(mSavingState == 2)
	{
		glDeleteBuffersARB(1, &mPboSaveID);
		mPboSaveID = 0;
        mTextureBuffer = DRTextureBufferPtr();
	}
    

	if(DRGrafikError("[Texture::getPixelsToSave()] error by asynchronously saving an image!"))
		LOG_ERROR("Fehler bei save image", DR_ERROR);
	return DR_OK;
}

DRReturn DRSaveTexture::saveImage()
{
	if(!mImage) return DR_ZERO_POINTER;
	if(mSavingState < 2) LOG_ERROR("Image data not ready!", DR_ERROR);
	if(mSavingState > 2) LOG_ERROR("ImageData already saved!", DR_OK);

	if(mSize.x <= 0 || mSize.y <= 0) LOG_ERROR("error getSize return zero", DR_ERROR);
	if(mSavingBuffer)
	{
		mImage->setSize(mSize);
		mImage->setImageFormat(-1);
		mImage->setPixel(mSavingBuffer);		
		DR_SAVE_DELETE_ARRAY(mSavingBuffer);
	}

	if(mImage->saveIntoFile(mFilename.data()))
		LOG_ERROR("fehler bei save", DR_ERROR);
	DRIImage::deleteImage(mImage);
	mImage = NULL;
		
	mSavingState = 3;
	//printf("image saved\n");
	return DR_OK;
}