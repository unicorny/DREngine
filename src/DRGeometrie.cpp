#include "Engine2Main.h"

DRGeometrie::DRGeometrie()
: mVertices(NULL), mIndices(NULL), mColors(NULL), mTextureCoords(NULL),
  mVertexCount(0), mIndexCount(0), mNumTextureCoords(0), mRenderVertexBuffer(false), mRenderMode(0)
{
    memset(mVertexBufferObjects, 0, sizeof(int)*4);
}

DRGeometrie::~DRGeometrie()
{
    releaseVertexBuffer();
    mVertexCount = 0;
    mIndexCount = 0;
    
    clearData();
}

DRReturn DRGeometrie::init(u32 vertexCount, u32 indexCount/* = 0*/, u32 textureCount/* = 0*/, bool color /*= false*/)
{
    clearData();
    releaseVertexBuffer();

	DRLog.writeToLog("[DRGeometrie::init] vertexCount: %d, indexCount: %d summe mBytes: %f", vertexCount, indexCount, (vertexCount*sizeof(DRReal)*3+indexCount*sizeof(GLuint))/(1024.0f*1024.0f));
    
    if(vertexCount)
    {
        mVertices = new DRVector3[vertexCount];
        mVertexCount = vertexCount;
        if(!mVertices) LOG_ERROR("get no memory for vertices array", DR_ERROR);
        
        if(color)
        {
            mColors = new DRColor[vertexCount];
            if(!mColors) LOG_ERROR("get no memory for colors array", DR_ERROR);
        }
        if(textureCount)
        {
            mTextureCoords = new DRVector2*[textureCount];
            mNumTextureCoords = textureCount;
            if(!mTextureCoords) LOG_ERROR("get no memory for texture coords pointer array", DR_ERROR);
            for(int i = 0; i < textureCount; i++)
            {
                mTextureCoords[i] = new DRVector2[vertexCount];
                if(!mTextureCoords[i]) LOG_ERROR("get no memory for texture coords array", DR_ERROR);
            }
        }            
    }
    if(indexCount)
    {
        mIndices = new GLuint[indexCount];
        mIndexCount = indexCount;
        memset(mIndices, 0, sizeof(GLuint)*indexCount);
        if(!mIndices) LOG_ERROR("get no memory for indices array", DR_ERROR);
    }
    return DR_OK;    
}

void DRGeometrie::clearData()
{
    DR_SAVE_DELETE_ARRAY(mVertices);
    DR_SAVE_DELETE_ARRAY(mIndices);
    DR_SAVE_DELETE_ARRAY(mColors);
    
    if(mNumTextureCoords)
    {
        for(int i = 0; i < mNumTextureCoords; i++)
        {
            DR_SAVE_DELETE_ARRAY(mTextureCoords[i]);
        }
        mNumTextureCoords = NULL;
    }
}

DRReturn DRGeometrie::initVertexBuffer()
{
    if(!glGenBuffersARB) LOG_ERROR("no VBO Extension?", DR_ZERO_POINTER);
    releaseVertexBuffer();
    
    if(mVertexCount)
        glGenBuffersARB(1, &mVertexBufferObjects[0]);
    if(mColors)
        glGenBuffersARB(1, &mVertexBufferObjects[1]);
    if(mIndexCount)
        glGenBuffersARB(1, &mVertexBufferObjects[2]);
    if(mNumTextureCoords)
    {
        if(sizeof(GLuint) != sizeof(int)) LOG_ERROR("[critical] Datatyp error..!", DR_ERROR);
        mVertexBufferObjects[3] = (GLuint)new GLuint[mNumTextureCoords];
        glGenBuffersARB(mNumTextureCoords, (GLuint*)mVertexBufferObjects[3]);
    }
    return DR_OK;
}
DRReturn DRGeometrie::copyDataToVertexBuffer(GLenum usage /*= GL_STATIC_DRAW_ARB*/, bool clearData/* = false*/)
{
    if(!glBindBufferARB) LOG_ERROR("glBindBufferARB ist nicht verfügbar", DR_ZERO_POINTER);
    
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
    if(mNumTextureCoords && mTextureCoords && mVertexBufferObjects[3] && mVertexCount)
    {
        GLuint* buffers = (GLuint*)mVertexBufferObjects[3];
        for(int i = 0; i < mNumTextureCoords; i++)
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
    return DR_OK;
}

DRReturn DRGeometrie::updateIndexDataIntoVertexBuffer(int from, int to, GLenum usage)
{
    if(!mVertexBufferObjects[2] || !mIndexCount || !mIndices ||
        !glBufferDataARB || !glBufferSubDataARB)
        LOG_ERROR("Zero-Pointer", DR_ZERO_POINTER);
    glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mVertexBufferObjects[2]); 
    if(!from && !to || to == mVertexCount)
        glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(GLuint)*mIndexCount, mIndices, usage);
    else
        glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, from, sizeof(GLuint)*(from-to), mIndices);
    
    glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0); 
	return DR_OK;
}

void DRGeometrie::releaseVertexBuffer()
{
    if(!glDeleteBuffersARB) LOG_ERROR_VOID("glDeleteBuffersARB didn't exist");
    
    for(int i = 0; i < 3; i++)
    {
        if(mVertexBufferObjects[i])
        {
            glDeleteBuffersARB(1, &mVertexBufferObjects[i]);
            mVertexBufferObjects[i] = 0;
        }
    }
    if(mNumTextureCoords && mVertexBufferObjects[3])
    {
        glDeleteBuffersARB(mNumTextureCoords, (GLuint*)mVertexBufferObjects[3]);
        delete [] (GLuint*)mVertexBufferObjects[3];
        mVertexBufferObjects[3] = 0;
    }
    mRenderVertexBuffer = false;
}

DRReturn DRGeometrie::render()
{
    if(!mVertexCount) LOG_ERROR("Nichts zum rendern", DR_ERROR);
    if(!mRenderMode)  LOG_ERROR("kein render mode set", DR_ERROR);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    if(mRenderVertexBuffer)
    {
        glBindBufferARB( GL_ARRAY_BUFFER_ARB, mVertexBufferObjects[0]); 
        glVertexPointer(3, GL_FLOAT, 0, (char*)NULL);
    }
    else
    {
        if(mVertices) glVertexPointer(3, GL_FLOAT, 0, mVertices);
        else LOG_ERROR("keine vertices...", DR_ZERO_POINTER);
    }
    
    
    if(mColors || (mVertexBufferObjects[1] && mRenderVertexBuffer))
    {
        glEnableClientState(GL_COLOR_ARRAY);
        if(mRenderVertexBuffer)
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, mVertexBufferObjects[1]);
            glColorPointer(4, GL_FLOAT, 0, (char*)NULL);
        }
        else
        {
            glColorPointer(4, GL_FLOAT, 0, mColors);
        }
    }     
    
    if(mIndices || (mVertexBufferObjects[2] && mRenderVertexBuffer))
    {
        if(!mRenderVertexBuffer)
        {
            glDrawElements(mRenderMode, mIndexCount, GL_UNSIGNED_INT, mIndices);
        }
        else
        {
            glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mVertexBufferObjects[2]);             
            glDrawElements(mRenderMode, mIndexCount, GL_UNSIGNED_INT, 0);
            //glDrawArrays( mRenderMode, 0, mVertexCount ); 
        }            
    }
    else
    {
        glDrawArrays(mRenderMode, 0, mVertexCount);    
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    if(mColors || (mVertexBufferObjects[1] && mRenderVertexBuffer))
        glDisableClientState(GL_COLOR_ARRAY);
    
    if(mRenderVertexBuffer)
    {
        // bind with 0, so, switch back to normal pointer operation
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
    if(DRGrafikError("Geometrie::render")) LOG_ERROR("Fehler beim rendern!", DR_ERROR);
    
    return DR_OK;
}
