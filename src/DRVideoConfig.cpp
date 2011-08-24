#include "Engine2Main.h"

DRVideoConfig::DRVideoConfig(int width/* = 640*/, int height/* = 480*/, int bpp/* = 32*/, int fullscreen/* = 0*/, int zbuffer /*= 8*/, int stencilBuffer/* = 0*/, int multiSampling/* = 0*/)
: mVersion(VIDEOCONFIGVERSION), mWidth(width), mHeight(height), mPixelDeep(bpp), mFullscreen(fullscreen), mZBuffer(zbuffer), mStencilBuffer(stencilBuffer), mMultiSampling(multiSampling)
{
}

DRVideoConfig::~DRVideoConfig()
{
}


void DRVideoConfig::print()
{
    printf("---- Video Config -----\n");
    printf("Version: %f\n", mVersion);
    printf("%dx%dx%d\n", mWidth, mHeight, mPixelDeep);
    if(mFullscreen)
        printf("Vollbildmodus!\n");
    if(mZBuffer)
        printf("Z-Buffer: %d Bit\n", mZBuffer);
    if(mStencilBuffer)
        printf("Stencil-Buffer: %d Bit\n", mStencilBuffer);
    if(mMultiSampling)
        printf("Multi-Sampling: %d-fach\n", mMultiSampling);
    printf("---- Ende Video Config ----\n");
}

void DRVideoConfig::setGLParams()
{
    //32 Bit Farben
//	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, mPixelDeep);
//8 Bit Z-Buffer (brauchen wir eigentlich nicht)
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, mZBuffer);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        //vsync aktivieren
#if SDL_VERSION_ATLEAST(1,3,0)
        SDL_GL_SetSwapInterval(1);
#else /* SDL_VERSION_ATLEAST(1,3,0) */
#ifdef SDL_GL_SWAP_CONTROL
        SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
#else /* SDL_GL_SWAP_CONTROL */
        LOG_WARNING("VSync unsupported on old SDL versions (before 1.2.10).");
#endif /* SDL_GL_SWAP_CONTROL */
#endif /* SDL_VERSION_ATLEAST(1,3,0) */ 
        
	//Multisampling aktivieren
	if(mMultiSampling)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, mMultiSampling);
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		DRLog.writeToLog("MultiSample aktiviert? %s", SDL_GetError());
	}
}
