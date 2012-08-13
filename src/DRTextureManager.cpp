#include "Engine2Main.h"

DRTextureManager::DRTextureManager()
: mInitalized(false), mGrafikMemTexture(0), mTextureLoadThread(NULL), mTextureSaveThread(NULL)
{
    
}

DRTextureManager& DRTextureManager::Instance()
{
    static DRTextureManager TheOneAndOnly;
    return TheOneAndOnly;
}

DRReturn DRTextureManager::init()
{
    mTextureLoadThread = new TextureLoadThread;
    mTextureSaveThread = new TextureSaveThread;
    if(!mTextureSaveThread || !mTextureLoadThread)
        LOG_ERROR("can't init threads", DR_ERROR);
    mGetTextureMutex  = SDL_CreateMutex();
    
    mInitalized = true;
    LOG_INFO("DRTextureManager initalisiert");
    return DR_OK;
}

void DRTextureManager::exit()
{
    mInitalized = false;
    for(std::multimap<DHASH,TextureMemoryEntry>::iterator it = mTextureMemoryEntrys.begin(); it != mTextureMemoryEntrys.end(); it++)
    {
        it->second.clear();
    }
    mTextureMemoryEntrys.clear();
    mTextureEntrys.clear();    
    
    DR_SAVE_DELETE(mTextureLoadThread);
    DR_SAVE_DELETE(mTextureSaveThread);    
    
    SDL_LockMutex(mGetTextureMutex);
    SDL_UnlockMutex(mGetTextureMutex);
    SDL_DestroyMutex(mGetTextureMutex);
    mGetTextureMutex = NULL;

    LOG_INFO("DRTextureManager beendet");
}

DHASH DRTextureManager::makeTextureHash(const char* filename, GLint glMinFilter/* = GL_LINEAR*/, GLint glMagFilter /*= GL_LINEAR*/)
{
    DHASH hash = DRMakeFilenameHash(filename);
    hash |=  glMinFilter| glMagFilter << 4;
    return hash;
}
DHASH DRTextureManager::makeTextureHash(const TextureMemoryEntry &entry)
{
    return entry.size.x | entry.size.y<<4 | entry.format << 16;    
}

//! lädt oder return instance auf Textur
DRTexturePtr DRTextureManager::getTexture(const char* filename, bool loadAsynchron /*= false*/, GLint glMinFilter/* = GL_LINEAR*/, GLint glMagFilter/* = GL_LINEAR*/)
{
    if(!mInitalized) return 0;
    SDL_LockMutex(mGetTextureMutex);

    DHASH id = makeTextureHash(filename, glMinFilter, glMagFilter);
	//Schauen ob schon vorhanden
    if(mTextureEntrys.find(id) != mTextureEntrys.end())
    {
        SDL_UnlockMutex(mGetTextureMutex);
        return mTextureEntrys[id];
    }
    DRTexturePtr tex(new DRTexture(filename));

    if(!tex.getResourcePtrHolder()->mResource)
    {
        SDL_UnlockMutex(mGetTextureMutex);
        LOG_ERROR("No Memory for new Texture left", 0);
    }
    
    if(loadAsynchron)
    {
        mTextureLoadThread->addLoadTask(tex);
    }
    else
    {
        if(tex->loadFromFile()) LOG_ERROR("Fehler by calling loadFromFile", tex);
        if(tex->pixelsCopyToRenderer()) LOG_ERROR("Fehler by calling pixelsCopyToRenderer", tex);
        tex->setFilter(glMinFilter, glMagFilter);
    }
    
    if(!mTextureEntrys.insert(DR_TEXTURE_ENTRY(id, tex)).second)
    {
        SDL_UnlockMutex(mGetTextureMutex);
        LOG_ERROR("Unerwarteter Fehler in DRTextureManager::getTexture aufgetreten", 0);
    }
    SDL_UnlockMutex(mGetTextureMutex);
    return tex;
}
DRTexturePtr DRTextureManager::getTexture(DRVector2i size, GLuint format, GLubyte* data/* = NULL*/, GLint dataSize /*= 0*/)
{
    if(!mInitalized) return 0;
    
    GLuint texID = _getTexture(size, format);
    if(texID)
        return DRTexturePtr(new DRTexture(texID, data, dataSize));
    return DRTexturePtr();
}
//! schaut nach ob solche eine Texture in der Liste steckt, wenn nicht wird eine neue erstellt
DRTexturePtr DRTextureManager::getTexture(DRVector2i size, GLuint format, DRColor* colors)
{
    if(!mInitalized) return 0;
    
    GLuint texID = _getTexture(size, format);
    if(texID)
        return DRTexturePtr(new DRTexture(texID, reinterpret_cast<GLubyte*>(colors), size.x*size.y*sizeof(DRColor), GL_FLOAT));
    return DRTexturePtr();
}
GLuint DRTextureManager::_getTexture(DRVector2i size, GLuint format)
{
    TextureMemoryEntry entry(size, format);
    DHASH id = makeTextureHash(entry);
    if(mTextureMemoryEntrys.find(id) != mTextureMemoryEntrys.end())
    {
        entry = mTextureMemoryEntrys.find(id)->second;
        mTextureMemoryEntrys.erase(id);
        
        return entry.textureID;
    }
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
    GLuint format2 = GL_RGB;
    if(format == 4) format2 = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0,
		format2, GL_UNSIGNED_BYTE, 0);
    
    if(DRGrafikError("[DRTextureManager::_getTexture]"))
        LOG_ERROR("Fehler beim Textur erstellen", 0);
    
    //printf("")
    addGrafikMemTexture(size.x*size.y*format);
    
    return textureID;
}

//! packt die Textur in die Liste, falls noch jemand den Speicher benötigt
void DRTextureManager::freeTexture(GLuint textureID)
{
    if(!mInitalized) return;
    
    TextureMemoryEntry entry;
    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &entry.size.x);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &entry.size.y);
    //glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH, &entry.type);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &entry.format);
    //printf("Infos ueber eine zu befreiende Texture: ");
    //entry.print(); printf("\n");
    entry.textureID = textureID;
    entry.timeout = 4.0f;
    DHASH id = makeTextureHash(entry);
    mTextureMemoryEntrys.insert(DR_TEXTURE_MEMORY_ENTRY(id, entry));       
}

void DRTextureManager::saveTexture(DRTexturePtr texture, const char* path, GLuint stepSize/* = 16384*/)
{
    if(!mInitalized) LOG_ERROR_VOID("not initalized");
    DRSaveTexture* savingTexture = new DRSaveTexture(path, stepSize);
    texture->bind();
    savingTexture->getPixelsToSave();
    mTextureSaveThread->addSaveTask(savingTexture);
 }
    
DRReturn DRTextureManager::TextureSaveThread::move()
{
    if(!mAsynchronSaveTextures.empty())
    {
        DRSaveTexture* cur = mAsynchronSaveTextures.front();

        if(cur->putPixelsToImage())
        {
            DR_SAVE_DELETE(cur);
            mAsynchronSaveTextures.pop();
        }
        else if(cur->isTextureReadyToSave())
        {
            lock();
            mAsynchronReadyToSaveTextures.push(cur);
            mAsynchronSaveTextures.pop();
            //send the texture load Thread a signal to continue
            condSignal();
            unlock();
        }
    }
    return DR_OK;
}
DRReturn DRTextureManager::TextureLoadThread::move()
{
    lock();
    if(!mLoadedAsynchronLoadTextures.empty())
    {
        mLoadedAsynchronLoadTextures.front()->pixelsCopyToRenderer();
        mLoadedAsynchronLoadTextures.pop();
    //	printf("\rtextures to load left: %d", mAsynchronLoadTextures.size());
    }
    unlock();
    return DR_OK;
}
// update timeout, release lange nicht verwendete Texturen
DRReturn DRTextureManager::move(float fTime)
{
    if(!mInitalized) return DR_ERROR;
    
    static float second = 0.0f;
    second += fTime;
    if(second >= 1.0f)
    {
        second = 0.0f;
        calculateGrafikMemTexture();
    }
    
    //timeout TextureMemoryEntry, and removed
    for(std::multimap<DHASH, TextureMemoryEntry>::iterator it = mTextureMemoryEntrys.begin(); it != mTextureMemoryEntrys.end(); it++)
    {
        it->second.timeout -= fTime;
        if(it->second.timeout < 0.0f)
        {
            //printf("DRTextureManager::move, timeout texture will be deleted\n");
            it->second.clear();
            mTextureMemoryEntrys.erase(it);
            break;
        }
    }
    // remove texure with referenz == 1, no other refernz left
    for(DR_TEXTURE_ENTRY_ITERATOR it = mTextureEntrys.begin(); it != mTextureEntrys.end(); it++)
    {
        if(it->second.getResourcePtrHolder()->getRefCount() == 1)
        {
            it->second.release();
            mTextureEntrys.erase(it);
            break;
        }            
    }
    mTextureLoadThread->move();
    mTextureSaveThread->move();    
        
    return DR_OK;
}

void DRTextureManager::TextureLoadThread::addLoadTask(DRTexturePtr texture)
{
    lock();
	mAsynchronLoadTextures.push(texture);

    condSignal();
	
	unlock();
}

int DRTextureManager::TextureLoadThread::ThreadFunction()
{
    while(!mAsynchronLoadTextures.empty())
    {
        // get top textur
        DRTexturePtr cur = mAsynchronLoadTextures.front();
        mAsynchronLoadTextures.pop();
        unlock();
        // call load
        DRReturn ret = cur->loadFromFile(); 
        lock();
        if(ret)
        {
            cur->setErrorByLoading();
        }
        else
        {
            // push it onto the other queue
            mLoadedAsynchronLoadTextures.push(cur);
        }
    }
    return 0;
}
int DRTextureManager::TextureSaveThread::ThreadFunction()
{
    while(!mAsynchronReadyToSaveTextures.empty())
    {
        // get top textur
        DRSaveTexture* cur = mAsynchronReadyToSaveTextures.front();
        mAsynchronReadyToSaveTextures.pop();
        unlock();
        // call save
        cur->saveImage();
        DR_SAVE_DELETE(cur);                
        lock();
    }
    return 0;
}

void DRTextureManager::calculateGrafikMemTexture()
{
    GLuint grafikMemTexture = 0;// mGrafikMemTexture
    // count memory entrys
    for(std::multimap<DHASH, TextureMemoryEntry>::iterator it = mTextureMemoryEntrys.begin(); it != mTextureMemoryEntrys.end(); it++)
        grafikMemTexture += it->second.size.x*it->second.size.y*it->second.format;
    
    for(DR_TEXTURE_ENTRY_ITERATOR it = mTextureEntrys.begin(); it != mTextureEntrys.end(); it++)
    {
        if(it->second.getResourcePtrHolder() &&
           it->second->isLoadingFinished())
        {
            grafikMemTexture += it->second->getResolution().x*it->second->getResolution().y*4;
        }
    }
    
    if(grafikMemTexture != mGrafikMemTexture)
    {
 /*       DRLog.writeToLog("abweichung in der Grafik berechnung: neuberechnet: %f, gezaehlt: %f", 
            static_cast<double>(grafikMemTexture)/(1024.0*1024.0),
            static_cast<double>(mGrafikMemTexture)/(1024.0*1024.0));
  //*/
        mGrafikMemTexture = grafikMemTexture;
    }
}

void DRTextureManager::test()
{
    printf("\n------ DRTextureManager::test ------\n");
    TextureMemoryEntry first(DRVector2i(800, 600), 4);
    TextureMemoryEntry second(DRVector2i(800, 600), 3);
    TextureMemoryEntry third(DRVector2i(1280, 1024), 3);
    TextureMemoryEntry four(DRVector2i(1280, 1024), 4);
    printf("first: "); first.print(); printf(" HASH: %d\n", makeTextureHash(first));
    printf("second: "); second.print(); printf(" HASH: %d\n", makeTextureHash(second));
    printf("third: "); third.print(); printf(" HASH: %d\n", makeTextureHash(third));
    printf("four: "); four.print(); printf(" HASH: %d\n", makeTextureHash(four));
        
    printf("\n--- DRTextureManager::test ende ----\n");
}

void DRTextureManager::TextureMemoryEntry::clear()
{
    glDeleteTextures(1, &textureID);
	//printf("[DRTextureManager::TextureMemoryEntry::clear] remove %f MB\n", (size.x*size.y*format)/(1024.0f*1024.0f));
    DRTextureManager::Instance().removeGrafikMemTexture(size.x*size.y*format);
}