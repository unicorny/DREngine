#include "Engine2Main.h"

DRGeometrie::DRGeometrie()
: mVertices(NULL), mNormals(NULL), mIndices(NULL), mColors(NULL), mTextureCoords(NULL),
  mVertexCount(0), mIndexCount(0), mMaxVertexCount(0), mMaxIndexCount(0),
  mNumTextureCoords(0), mRenderVertexBuffer(false), mBufferMutex(NULL), mRenderMode(0)
{
	mBufferMutex = SDL_CreateMutex();
	if(!mBufferMutex) LOG_WARNING("didn't create mutex");
    memset(mVertexBufferObjects, 0, sizeof(int)*5);
}

DRGeometrie::~DRGeometrie()
{
    releaseVertexBuffer();
    mVertexCount = 0;
    mIndexCount = 0;
    
    clearData();
    SDL_DestroyMutex(mBufferMutex);
}

DRReturn DRGeometrie::init(u32 vertexCount, u32 indexCount/* = 0*/, u32 textureCount/* = 0*/, bool color /*= false*/, bool normals/* = false*/)
{
    clearData();
    //releaseVertexBuffer();

    DRLog.writeToLog("[DRGeometrie::init] vertexCount: %d, indexCount: %d summe mBytes: %f", vertexCount, indexCount, (vertexCount*sizeof(DRReal)*3+indexCount*sizeof(GLuint))/(1024.0f*1024.0f));
    
    if(vertexCount)
    {
        mVertices = new DRVector3[vertexCount];
        mVertexCount = 0;
        mMaxVertexCount = vertexCount;
        if(!mVertices) LOG_ERROR("get no memory for vertices array", DR_ERROR);
        
        if(color)
        {
            mColors = new DRColor[vertexCount];
            if(!mColors) LOG_ERROR("get no memory for colors array", DR_ERROR);
        }
        if(normals)
        {
            mNormals = new DRVector3[vertexCount];
            if(!mNormals) LOG_ERROR("get no memory for normals array", DR_ERROR);            
        }
        if(textureCount)
        {
            mTextureCoords = new DRVector2*[textureCount];
            mNumTextureCoords = textureCount;
            if(!mTextureCoords) LOG_ERROR("get no memory for texture coords pointer array", DR_ERROR);
            for(uint i = 0; i < textureCount; i++)
            {
                mTextureCoords[i] = new DRVector2[vertexCount];
                if(!mTextureCoords[i]) LOG_ERROR("get no memory for texture coords array", DR_ERROR);
            }
        }            
    }
    if(indexCount)
    {
        mIndices = new GLuint[indexCount];
        mIndexCount = 0;
		mMaxIndexCount = indexCount;
        memset(mIndices, 0, sizeof(GLuint)*indexCount);
        if(!mIndices) LOG_ERROR("get no memory for indices array", DR_ERROR);
    }

    if(initVertexBuffer()) LOG_WARNING("Fehler bei init VertexBuffer!");

    return DR_OK;    
}

void DRGeometrie::clearData()
{
    DR_SAVE_DELETE_ARRAY(mVertices);
    DR_SAVE_DELETE_ARRAY(mIndices);
    DR_SAVE_DELETE_ARRAY(mColors);
    DR_SAVE_DELETE_ARRAY(mNormals);
    
    if(mNumTextureCoords)
    {
        for(uint i = 0; i < mNumTextureCoords; i++)
        {
            DR_SAVE_DELETE_ARRAY(mTextureCoords[i]);
        }
        mNumTextureCoords = NULL;
    }
}

DRReturn DRGeometrie::initVertexBuffer()
{
    if(!glGenBuffersARB) LOG_ERROR("no VBO Extension?", DR_ZERO_POINTER);
    //releaseVertexBuffer();
    
    if(mMaxVertexCount && !mVertexBufferObjects[0])
        glGenBuffersARB(1, &mVertexBufferObjects[0]);

    if(mColors && !mVertexBufferObjects[1])
        glGenBuffersARB(1, &mVertexBufferObjects[1]);

    if(mMaxIndexCount && !mVertexBufferObjects[2])
        glGenBuffersARB(1, &mVertexBufferObjects[2]);
    
    if(mNormals && !mVertexBufferObjects[3])
        glGenBuffersARB(1, &mVertexBufferObjects[3]);
    
    if(mNumTextureCoords && !mVertexBufferObjects[4])
    {
        if(sizeof(GLuint) != sizeof(int)) LOG_ERROR("[critical] Datatyp error..!", DR_ERROR);
        mVertexBufferObjects[4] = (GLuint)new GLuint[mNumTextureCoords];
        glGenBuffersARB(mNumTextureCoords, (GLuint*)mVertexBufferObjects[4]);
    }    
    
    return DR_OK;
}
DRReturn DRGeometrie::copyDataToVertexBuffer(GLenum usage /*= GL_STATIC_DRAW_ARB*/, bool clearData/* = false*/)
{
    if(!glBindBufferARB) LOG_ERROR("glBindBufferARB ist nicht verfügbar", DR_ZERO_POINTER);
	lock();
    
    if(mVertices && mVertexCount && mVertexBufferObjects[0])
    {
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mVertexBufferObjects[0]); 
        glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(DRVector3)*mVertexCount, mVertices, usage);
    }
    if(mColors && mVertexCount && mVertexBufferObjects[1])
    {
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mVertexBufferObjects[1]); 
        glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(DRColor)*mVertexCount, mColors, usage);
    }
    if(mIndexCount && mIndices && mVertexBufferObjects[2])
    {
        glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mVertexBufferObjects[2]); 
        glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(GLuint)*mIndexCount, mIndices, usage);
    }
    if(mNormals && mVertexCount && mVertexBufferObjects[3])
    {
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mVertexBufferObjects[3]); 
        glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(DRVector3)*mVertexCount, mNormals, usage);
    }
    if(mNumTextureCoords && mTextureCoords && mVertexBufferObjects[4] && mVertexCount)
    {
        GLuint* buffers = (GLuint*)mVertexBufferObjects[4];
        for(uint i = 0; i < mNumTextureCoords; i++)
        {
            if(!mTextureCoords[i])
            {
                LOG_WARNING("broken Texture Coords Array!");
                break;
            }
            
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, buffers[i]); 
            glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(GLuint)*mVertexCount, mTextureCoords[i], usage);
        }
    }
    // bind with 0, so, switch back to normal pointer operation
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    
    mRenderVertexBuffer = true;
    if(clearData)
        this->clearData();
    unlock();
    return DR_OK;
}

DRReturn DRGeometrie::updateIndexDataIntoVertexBuffer(int from, int to, GLenum usage)
{
    if(!mVertexBufferObjects[2] || !mMaxIndexCount || !mIndices ||
       !glBufferDataARB || !glBufferSubDataARB)
        LOG_ERROR("Zero-Pointer", DR_ZERO_POINTER);
    lock();
    glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mVertexBufferObjects[2]); 
    if(!from && !to || to == mMaxIndexCount)
        glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(GLuint)*mMaxIndexCount, mIndices, usage);
    else	
        glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, from, sizeof(GLuint)*(from-to), mIndices);

	mIndexCount = to;
    
    glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0); 
    unlock();
    return DR_OK;
}

void DRGeometrie::releaseVertexBuffer()
{
    if(!glDeleteBuffersARB) LOG_ERROR_VOID("glDeleteBuffersARB didn't exist");
	lock();
    
    for(int i = 0; i < 4; i++)
    {
        if(mVertexBufferObjects[i])
        {
            glDeleteBuffersARB(1, &mVertexBufferObjects[i]);
            mVertexBufferObjects[i] = 0;
        }
    }
    if(mNumTextureCoords && mVertexBufferObjects[4])
    {
        glDeleteBuffersARB(mNumTextureCoords, (GLuint*)mVertexBufferObjects[4]);
        delete [] (GLuint*)mVertexBufferObjects[4];
        mVertexBufferObjects[4] = 0;
    }
    mRenderVertexBuffer = false;
    unlock();
}

DRReturn DRGeometrie::render()
{
    if(!mVertexCount) LOG_ERROR("nothing to render", DR_ERROR);
    if(!mRenderMode)  LOG_ERROR("no render mode set", DR_ERROR);
    
    lock();
    glEnableClientState(GL_VERTEX_ARRAY);
    if(mRenderVertexBuffer && mVertexBufferObjects[0])
    {
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mVertexBufferObjects[0]); 
        glVertexPointer(3, GL_FLOAT, 0, (char*)NULL);
    }
    else
    {
        if(mVertices) glVertexPointer(3, GL_FLOAT, 0, mVertices);
        else LOG_ERROR("no vertices...", DR_ZERO_POINTER);
    }
    
    
    if(mColors || (mVertexBufferObjects[1] && mRenderVertexBuffer))
    {
        glEnableClientState(GL_COLOR_ARRAY);
        if(mRenderVertexBuffer && mVertexBufferObjects[1])
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVertexBufferObjects[1]);
            glColorPointer(4, GL_FLOAT, 0, (char*)NULL);
        }
        else
        {
            glColorPointer(4, GL_FLOAT, 0, mColors);
        }
    } 
    
   if(mNormals || (mVertexBufferObjects[3] && mRenderVertexBuffer))
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        if(mRenderVertexBuffer && mVertexBufferObjects[3])
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVertexBufferObjects[3]);
            glNormalPointer(GL_FLOAT, 0, (char*)NULL);
        }
        else
        {
            glNormalPointer(GL_FLOAT, 0, mNormals);
        }
    }
    if((mNumTextureCoords && mTextureCoords) || (mVertexBufferObjects[4] && mRenderVertexBuffer))
    {
        GLuint* buffers = (GLuint*)mVertexBufferObjects[4];
        for(uint i = 0; i < mNumTextureCoords; i++)
        {
            if(i > 0) break;
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            if(mRenderVertexBuffer && buffers[0])
            {
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffers[0]);
                glTexCoordPointer(2, GL_FLOAT, 0, (char*)NULL);
            }
            else
            {
                glTexCoordPointer(2, GL_FLOAT, 0, mTextureCoords[0]);
            }           
        }
    }
    
    if((mIndices || (mVertexBufferObjects[2] && mRenderVertexBuffer)) && mIndexCount)
    {
        if(mRenderVertexBuffer && mVertexBufferObjects[2])
        {
	    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mVertexBufferObjects[2]);             
            glDrawElements(mRenderMode, mIndexCount, GL_UNSIGNED_INT, 0);
            //glDrawArrays( mRenderMode, 0, mVertexCount );             
        }
        else
        {
           glDrawElements(mRenderMode, mIndexCount, GL_UNSIGNED_INT, mIndices);
        }            
    }
    else if(mVertexCount)
    {
        glDrawArrays(mRenderMode, 0, mVertexCount);    
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    if(mColors || (mVertexBufferObjects[1] && mRenderVertexBuffer))
        glDisableClientState(GL_COLOR_ARRAY);
    if(mNormals || (mVertexBufferObjects[3] && mRenderVertexBuffer))
        glDisableClientState(GL_NORMAL_ARRAY);
    if((mNumTextureCoords && mTextureCoords) || (mVertexBufferObjects[4] && mRenderVertexBuffer))
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    if(mRenderVertexBuffer)
    {
        // bind with 0, so, switch back to normal pointer operation
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }

    if(DRGrafikError("Geometrie::render")) LOG_ERROR("Fehler beim rendern!", DR_ERROR);
    unlock();
    
    return DR_OK;
}
