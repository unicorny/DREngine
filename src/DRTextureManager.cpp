#include "DREngine/DRTextureManager.h"
#include "DREngine/DRTexture.h"
#include "DREngine/DRSaveTexture.h"
#include "DREngine/DRLogging.h"
#include "DRCore2/Foundation/DRHash.h"

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
    
    {
        std::lock_guard _lock(mGetTextureMutex);
    }

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
    std::lock_guard _lock(mGetTextureMutex);

    DHASH id = makeTextureHash(filename, glMinFilter, glMagFilter);
	//Schauen ob schon vorhanden
    if(mTextureEntrys.find(id) != mTextureEntrys.end())
    {
        return mTextureEntrys[id];
    }
    auto tex = std::make_shared<DRTexture>(filename);

    if(!tex)
    {
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
        LOG_ERROR("Unerwarteter Fehler in DRTextureManager::getTexture aufgetreten", 0);
    }
    return tex;
}
DRTexturePtr DRTextureManager::getTexture(DRVector2i size, GLuint format, GLubyte* data/* = NULL*/, GLint dataSize /*= 0*/)
{
    if(!mInitalized) return 0;
    
    DRTextureBufferPtr texID = _getTexture(size, format);
    if(texID)
        return std::make_shared<DRTexture>(texID, data, dataSize);
    //error
    LOG_ERROR("couldn't create texture", DRTexturePtr());
}
//! schaut nach ob solche eine Texture in der Liste steckt, wenn nicht wird eine neue erstellt
DRTexturePtr DRTextureManager::getTexture(DRVector2i size, GLuint format, DRColor* colors)
{
    if(!mInitalized) return 0;
    
    DRTextureBufferPtr texID = _getTexture(size, format);
    if(texID)
        return  std::make_shared<DRTexture>(texID, reinterpret_cast<GLubyte*>(colors), size.x*size.y*sizeof(DRColor), GL_FLOAT);
    //error
    LOG_ERROR("couldn't create texture", DRTexturePtr());
}
DRTextureBufferPtr DRTextureManager::_getTexture(DRVector2i size, GLuint format)
{
    if(size.x <= 0 || size.y <= 0) LOG_ERROR("size isn't valid", DRTextureBufferPtr());
    TextureMemoryEntry entry(size, format);
    DHASH id = makeTextureHash(entry);
    if(mTextureMemoryEntrys.find(id) != mTextureMemoryEntrys.end())
    {
        entry = mTextureMemoryEntrys.find(id)->second;
        mTextureMemoryEntrys.erase(id);
        
        return entry.textureID;
    }
    DRTextureBufferPtr textureID(new DRTextureBuffer);
    glBindTexture(GL_TEXTURE_2D, *textureID);
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
void DRTextureManager::freeTexture(DRTextureBufferPtr textureID)
{
    if(!mInitalized) return;
    
    TextureMemoryEntry entry;
    glBindTexture(GL_TEXTURE_2D, *textureID);
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

DRSaveTexture* DRTextureManager::saveTexture(DRTexturePtr texture, const char* path, GLuint stepSize/* = 16384*/)
{
    if(!mInitalized) LOG_ERROR("not initalized", NULL);
    DRSaveTexture* savingTexture = new DRSaveTexture(path, texture->getTextureBuffer(), stepSize);
    return saveTexture(texture, savingTexture);
 }

DRSaveTexture* DRTextureManager::saveTexture(DRTexturePtr texture, DRSaveTexture* saveTexture)
{
    if(!mInitalized) LOG_ERROR("not initalized", NULL);
    texture->bind();
    saveTexture->getPixelsToSave();
    mTextureSaveThread->addSaveTask(saveTexture);
    return saveTexture;
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
            std::lock_guard _lock(mMutex);
            mAsynchronReadyToSaveTextures.push(cur);
            mAsynchronSaveTextures.pop();
            //send the texture load Thread a signal to continue
            condSignal();
        }
    }
    return DR_OK;
}
DRReturn DRTextureManager::TextureLoadThread::move()
{
    std::lock_guard _lock(mMutex);
    if(!mLoadedAsynchronLoadTextures.empty())
    {
        mLoadedAsynchronLoadTextures.front()->pixelsCopyToRenderer();
        mLoadedAsynchronLoadTextures.pop();
    //	printf("\rtextures to load left: %d", mAsynchronLoadTextures.size());
    }
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
        if(it->second.timeout < 0.0f && 
           it->second.textureID &&
           it->second.textureID.use_count() <= 1)
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
        if(it->second.use_count() == 1)
        {
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
    std::lock_guard _lock(mMutex);
	mAsynchronLoadTextures.push(texture);

    condSignal();
}

int DRTextureManager::TextureLoadThread::ThreadFunction()
{
    mMutex.lock();
    while(!mAsynchronLoadTextures.empty())
    {
        // get top textur
        DRTexturePtr cur = mAsynchronLoadTextures.front();
        mAsynchronLoadTextures.pop();
        mMutex.unlock();
        // call load
        DRReturn ret = cur->loadFromFile(); 
        mMutex.lock();
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
    mMutex.unlock();
    return 0;
}
int DRTextureManager::TextureSaveThread::ThreadFunction()
{
    mMutex.lock();
    while(!mAsynchronReadyToSaveTextures.empty())
    {
        // get top textur
        DRSaveTexture* cur = mAsynchronReadyToSaveTextures.front();
        mAsynchronReadyToSaveTextures.pop();
        mMutex.unlock();
        // call save
        cur->saveImage();
        DR_SAVE_DELETE(cur);                
        mMutex.lock();
    }
    mMutex.unlock();
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
        if(it->second &&
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
    printf("first: "); first.print(); printf(" HASH: %ld\n", makeTextureHash(first));
    printf("second: "); second.print(); printf(" HASH: %ld\n", makeTextureHash(second));
    printf("third: "); third.print(); printf(" HASH: %ld\n", makeTextureHash(third));
    printf("four: "); four.print(); printf(" HASH: %ld\n", makeTextureHash(four));
        
    printf("\n--- DRTextureManager::test ende ----\n");
}

void DRTextureManager::TextureMemoryEntry::clear()
{
    //glDeleteTextures(1, &textureID);
	//printf("[DRTextureManager::TextureMemoryEntry::clear] remove %f MB\n", (size.x*size.y*format)/(1024.0f*1024.0f));
    DRTextureManager::Instance().removeGrafikMemTexture(size.x*size.y*format);
}