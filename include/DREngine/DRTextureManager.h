/*/*************************************************************************
 *                                                                         *
 * DREngine, Engine for my programs, using SDL and OpenGL                 *
 * Copyright (C) 2012, 2013, 2014 github.com/unicorny                      *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * any later version.                                                      *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 *                                                                         *
 ***************************************************************************/

#ifndef __DR_ENGINE2_TEXTURE_MANAGER__
#define __DR_ENGINE2_TEXTURE_MANAGER__

#include <map>
#include <queue>
#include <mutex>

#include "OpenGL.h"
#include "DREngine/export.h"
#include "DRCore2/DRTypes.h"
#include "DRCore2/Foundation/DRVector2i.h"
#include "DRCore2/Foundation/DRColor.h"
#include "DRCore2/Threading/DRThread.h"

class DRSaveTexture;
class DRTexture;
class DRTextureBuffer;
typedef std::shared_ptr<DRTexture> DRTexturePtr;
typedef std::shared_ptr<DRTextureBuffer> DRTextureBufferPtr;

class DRENGINE_EXPORT DRTextureManager
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

    DRSaveTexture* saveTexture(DRTexturePtr texture, const char* path, GLuint stepSize = 16384);
    DRSaveTexture* saveTexture(DRTexturePtr texture, DRSaveTexture* saveTexture);


    // update timeout, release lange nicht verwendete Texturen
    DRReturn move(float fTime);

    void test();

    inline GLuint getGrafikMemoryTexture() const {return mGrafikMemTexture;}
private:
    DRTextureManager();
    inline void addGrafikMemTexture(GLuint addValue) {mGrafikMemTexture += addValue;}
    inline void removeGrafikMemTexture(GLuint subtractValue) {mGrafikMemTexture -= subtractValue;}
    void calculateGrafikMemTexture();
    DRTextureBufferPtr _getTexture(DRVector2i size, GLuint format);

     //! schaut nach ob solche eine Texture in der Liste steckt, wenn nicht wird eine neue erstellt
    //GLuint    getGLTextureMemory(GLuint width, GLuint height, GLuint format);
    //! packt die Textur in die Liste, falls noch jemand den Speicher benötigt
    void      freeTexture(DRTextureBufferPtr textureID);


    //! daten für alte Einträge, dessen Speicher noch Verwendung finden könnte
    struct TextureMemoryEntry
    {
        TextureMemoryEntry(DRVector2i _size, GLint _format)
                : textureID(), size(_size), format(_format) {}
        TextureMemoryEntry()
                : textureID(), size(0, 0), format(0) {}
        void print() {printf("width: %d, height: %d, format: %d", size.x, size.y, format);}
        void clear();
        DRTextureBufferPtr textureID;
        DRVector2i size;
        GLint format;
        float  timeout; //Speicher wird freigegeben, wenn null erreicht,  0 kein timeout
    };

    class TextureLoadThread : public DRThread
    {
    public:
        TextureLoadThread() : DRThread("DRTexLoa") {}
        ~TextureLoadThread() {while(!mAsynchronLoadTextures.empty()) mAsynchronLoadTextures.pop();
                              while(!mLoadedAsynchronLoadTextures.empty()) mLoadedAsynchronLoadTextures.pop();}

        virtual int ThreadFunction();
        void addLoadTask(DRTexturePtr texture);
        DRReturn move();
    protected:
        std::queue<DRTexturePtr> mAsynchronLoadTextures;
        std::queue<DRTexturePtr> mLoadedAsynchronLoadTextures;
        std::mutex mMutex;
    };

    class TextureSaveThread : public DRThread
    {
    public:
        TextureSaveThread() : DRThread("DRTexSav") {}
        ~TextureSaveThread() {while(!mAsynchronSaveTextures.empty()) mAsynchronSaveTextures.pop();
                              while(!mAsynchronReadyToSaveTextures.empty()) mAsynchronReadyToSaveTextures.pop();}

        virtual int ThreadFunction();
        inline void addSaveTask(DRSaveTexture* texture) {mAsynchronSaveTextures.push(texture);}
        DRReturn move();
    protected:
        std::queue<DRSaveTexture*> mAsynchronSaveTextures;
        std::queue<DRSaveTexture*> mAsynchronReadyToSaveTextures;
        std::mutex mMutex;
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

    TextureLoadThread*     mTextureLoadThread;
    TextureSaveThread*     mTextureSaveThread;
    std::mutex             mGetTextureMutex;
};

#endif //__DR_ENGINE2_TEXTURE_MANAGER__