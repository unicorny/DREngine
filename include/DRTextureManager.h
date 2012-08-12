/*/*************************************************************************
 *                                                                         *
 * EngineDLL, Engine for my programs, using SDL and OpenGL		   *
 * Copyright (C) 2012, 2013, 2014 Dario Rekowski.			   *
 * Email: dario.rekowski@gmx.de   Web: www.einhornimmond.de                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * any later version.							   *
 *									   *
 * This program is distributed in the hope that it will be useful,	   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of	   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	   *
 * GNU General Public License for more details.				   *
 *									   *
 * You should have received a copy of the GNU General Public License	   *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 *                                                                         *
 ***************************************************************************/
 
#ifndef __DR_ENGINE2_TEXTURE_MANAGER__
#define __DR_ENGINE2_TEXTURE_MANAGER__

class ENGINE2_API DRTextureManager
{
    friend class DRTexture;
public:
    ~DRTextureManager() {if(isInitialized()) exit();};

    // Singleton-Daten
    static DRTextureManager&		Instance();
    inline static DRTextureManager& getSingleton() {return Instance();};
    inline static DRTextureManager* getSingletonPtr() {return &Instance();};
    static bool	isInitialized()	{return Instance().mInitalized;};
    
    DRReturn init();
    
    void exit();
    //! lädt oder return instance auf Textur
    //! thread save, but if called from other thread than main thread (openGL), loadAsynchron must be true
    DRTexturePtr getTexture(const char* filename, bool loadAsynchron = false, GLint glMinFilter = GL_LINEAR, GLint glMagFilter = GL_LINEAR);
    DRTexturePtr getTexture(DRVector2i size, GLuint format, GLubyte* data = NULL, GLint dataSize = 0);
    //! Warning. not testet yet!
    DRTexturePtr getTexture(DRVector2i size, GLuint format, DRColor* colors);
    
    void saveTexture(DRTexturePtr texture, const char* path, GLuint stepSize = 16384);
    
    //! schaut nach ob solche eine Texture in der Liste steckt, wenn nicht wird eine neue erstellt
    //GLuint    getGLTextureMemory(GLuint width, GLuint height, GLuint format);
    //! packt die Textur in die Liste, falls noch jemand den Speicher benötigt
    void      freeTexture(GLuint textureID);
    
    
    // update timeout, release lange nicht verwendete Texturen
    DRReturn move(float fTime);     
    
    void test();
    
    __inline__ GLuint getGrafikMemoryTexture() const {return mGrafikMemTexture;} 
private:
    DRTextureManager();
    __inline__ void addGrafikMemTexture(GLuint addValue) {mGrafikMemTexture += addValue;}
    __inline__ void removeGrafikMemTexture(GLuint subtractValue) {mGrafikMemTexture -= subtractValue;}
    void calculateGrafikMemTexture();
    GLuint _getTexture(DRVector2i size, GLuint format);

    void      addAsynchronTextureLoadTask(DRTexturePtr texture);
    
    static int asynchronTextureLoadThread(void* data);
    
    static int asynchronTextureSaveThread(void* data);
    
    //! daten für alte Einträge, dessen Speicher noch Verwendung finden könnte
    struct TextureMemoryEntry
    {
        TextureMemoryEntry(DRVector2i _size, GLint _format)
                : textureID(0), size(_size), format(_format) {}
        TextureMemoryEntry() 
                : textureID(0), size(0, 0), format(0) {}
        void print() {printf("width: %d, height: %d, format: %d", size.x, size.y, format);}
        void clear();
        GLuint textureID;
        DRVector2i size;
        GLint format;
        float  timeout; //Speicher wird freigegeben, wenn null erreicht,  0 kein timeout
    };
    
    struct TextureThreadData
    {
        TextureThreadData() : mutex(NULL), thread(NULL), condition(NULL), semaphore(NULL)
        {
            semaphore = SDL_CreateSemaphore(1);
            condition = SDL_CreateCond();
            SDL_SemWait(semaphore);
            mutex = SDL_CreateMutex();   
        }
        ~TextureThreadData() 
        {
            if(thread)
            {
                //Post Exit to Stream Thread
                SDL_SemPost(semaphore); LOG_WARNING_SDL();
                //kill TextureLoad Thread after 1/2 second
                SDL_Delay(500);
                SDL_KillThread(thread);
                LOG_WARNING_SDL();

                thread = NULL;
                SDL_DestroySemaphore(semaphore);
                SDL_DestroyMutex(mutex);
                SDL_DestroyCond(condition);
            }
        }
        
        DRReturn lock() {SDL_LockMutex(mutex); LOG_ERROR_SDL(DR_ERROR); return DR_OK;}
        DRReturn unlock() {SDL_UnlockMutex(mutex); LOG_ERROR_SDL(DR_ERROR); return DR_OK;} 
        DRReturn condSignal()
        {
            if(SDL_CondSignal(condition)== -1) //LOG_ERROR_SDL(DR_ERROR);
            {
                LOG_WARNING("Fehler beim Aufruf von SDL_CondSignal"); 
                LOG_ERROR_SDL(DR_ERROR);
            }
            return DR_OK;
        }
        SDL_mutex*		   mutex;
        SDL_Thread*		   thread;
        SDL_cond*		   condition;
        SDL_sem*		   semaphore;
    };
    
    DHASH makeTextureHash(const char* filename, GLint glMinFilter = GL_LINEAR, GLint glMagFilter = GL_LINEAR);
    DHASH makeTextureHash(const TextureMemoryEntry &entry);   
    
    //DRHashList mTextureEntrys;
    std::map<DHASH, DRTexturePtr>                         mTextureEntrys;
    typedef std::pair<DHASH, DRTexturePtr>                DR_TEXTURE_ENTRY;
    typedef std::map<DHASH, DRTexturePtr>::iterator       DR_TEXTURE_ENTRY_ITERATOR;
    std::multimap<DHASH, TextureMemoryEntry>            mTextureMemoryEntrys;
    typedef std::pair<DHASH, TextureMemoryEntry>        DR_TEXTURE_MEMORY_ENTRY;
    bool                                                mInitalized;
    GLuint						mGrafikMemTexture;
    
    //! stuff for asynchron texture load and save
    std::queue<DRTexturePtr> mAsynchronLoadTextures;
    std::queue<DRTexturePtr> mLoadedAsynchronLoadTextures;
    std::queue<DRSaveTexture*> mAsynchronSaveTextures;
    std::queue<DRSaveTexture*> mAsynchronReadyToSaveTextures;
    TextureThreadData*     mTextureLoadThread;
    TextureThreadData*     mTextureSaveThread;
    SDL_mutex*             mGetTextureMutex;
};

#endif //__DR_ENGINE2_TEXTURE_MANAGER__