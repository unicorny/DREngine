#include "Engine2Main.h"


DRGeometrieSphere::DRGeometrieSphere()
 : DRGeometrie()
{
    
}

DRGeometrieSphere::~DRGeometrieSphere()
{    
}

DRReturn DRGeometrieSphere::initSphere(GLuint segmentSize)
{   
    // vertex and index calculation
    GLuint vertexCount = segmentSize*segmentSize;
    GLuint indexCount =  2*segmentSize*segmentSize-2*segmentSize;
    
    // memory allocation
    if(init(vertexCount, indexCount, 0, true)) LOG_ERROR("no memory allocatet for geometrie!", DR_ERROR);
    
    // generate a round line
    for(int i = 0; i < segmentSize; i++)
    {
        DRReal sin = sinf(((PI)/(double)(segmentSize-1))*(double)i);
        DRReal cos = cosf(((PI)/(double)(segmentSize-1))*(double)i);        

        mVertices[i] = DRVector3(cos, sin, 0.0f);
    }   

    // rotate the line to make a whole sphere (rotation count = segmentSize)
    for(int j = 0; j < segmentSize; j++)
    {
        // PI/segs*2 = 360°
        // PI/segs = 180°
        DRMatrix rot = DRMatrix::rotationX((PI/(segmentSize-1)*2)*j);
        for(int i = 0; i < segmentSize; i++)
        {
            if(segmentSize*j + i >= vertexCount) LOG_ERROR("critical 1", DR_ERROR);
            mVertices[segmentSize*j + i] = mVertices[i].transformNormal(rot);//*DRRandom::rReal(1.02, 0.98f);
            mColors[segmentSize*j + i] = DRColor((float)i/(float)segmentSize, (float)j/(float)segmentSize, fabs((float)i/((float)j+0.001f)));
        }
    }

    // generate indices to render sphere as QUAD_STRIP
    for(int j = 0; j < segmentSize-1; j++)
    {
        for(int i = 0; i < segmentSize; i++)
        {
            if(i*2+1+(segmentSize*2)*j >= indexCount) LOG_ERROR("critical 2", DR_ERROR);
            if(j*segmentSize + segmentSize+i >= vertexCount) LOG_ERROR("critical 3", DR_ERROR);
            
            mIndices[i*2+(segmentSize*2)*j]   = j*segmentSize + segmentSize+i; 
            mIndices[i*2+1+(segmentSize*2)*j] = j*segmentSize + i;    
         }
    }
    
    mRenderMode = GL_QUAD_STRIP;
   
    return DR_OK;
}

void DRGeometrieSphere::makeSphericalLandscape(GLuint numIterations, GLuint randomSeed)
{
    if(!mVertexCount) LOG_ERROR_VOID("keine Vertices zum manipulieren!");
    DRRandom::seed(randomSeed);
    
    int m = 1;
    float max = 0.0f, min = 1.0f;
    for(int i = 0; i < numIterations; i++)
    {
        DRVector3 n = DRRandom::rVector3(1.0f);
        DRPlane pl(n, n.length());
        m = -m;
        
        for(int j = 0; j < mVertexCount; j++)
        {
            float d = 1.0f - mVertices[j].length();
            if(pl.dotCoords(mVertices[j]) >= 1)
            {
                mVertices[j] = mVertices[j] * (1.0f+(float)m/(float)numIterations);
            }
            else
            {
                mVertices[j] = mVertices[j] * (1.0f-(float)m/(float)numIterations);
            }
        }
    }
}
