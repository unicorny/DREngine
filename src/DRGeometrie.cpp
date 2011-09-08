#include "Engine2Main.h"

DRGeometrie::DRGeometrie()
: mVertices(NULL), mIndices(NULL), mColors(NULL), mTextureCoords(NULL),
  mVertexCount(0), mIndexCount(0), mNumTextureCoords(0), mRenderMode(0)
{
    
}

DRGeometrie::~DRGeometrie()
{
    DR_SAVE_DELETE_ARRAY(mVertices);
    mVertexCount = 0;
    DR_SAVE_DELETE_ARRAY(mIndices);
    mIndexCount = 0;
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

DRReturn DRGeometrie::init(u32 vertexCount, u32 indexCount/* = 0*/, u32 textureCount/* = 0*/, bool color /*= false*/)
{
    if(vertexCount)
    {
        DR_SAVE_DELETE_ARRAY(mVertices);
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

DRReturn DRGeometrie::render()
{
    if(!mVertexCount) LOG_ERROR("Nichts zum rendern", DR_ERROR);
    if(!mRenderMode)  LOG_ERROR("kein render mode set", DR_ERROR);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, mVertices);
    if(mColors)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, mColors);
    }     
    
    if(mIndices)
        glDrawElements(mRenderMode, mIndexCount, GL_UNSIGNED_INT, mIndices);
    else
        glDrawArrays(mRenderMode, 0, mVertexCount);    
    
    glDisableClientState(GL_VERTEX_ARRAY);
    if(mColors)
        glDisableClientState(GL_COLOR_ARRAY);
    
    if(DRGrafikError("Geometrie::render")) LOG_ERROR("Fehler beim rendern!", DR_ERROR);
    
    return DR_OK;
}
