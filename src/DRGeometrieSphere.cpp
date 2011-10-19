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
    for(uint i = 0; i < segmentSize; i++)
    {
        DRReal _sin = sinf(((PI)/(float)(segmentSize-1))*(float)i);
        DRReal _cos = cosf(((PI)/(float)(segmentSize-1))*(float)i);        

        mVertices[i] = DRVector3(_cos, _sin, 0.0f);
    }   

    // rotate the line to make a whole sphere (rotation count = segmentSize)
    for(uint j = 0; j < segmentSize; j++)
    {
        // PI/segs*2 = 360°
        // PI/segs = 180°
        DRMatrix rot = DRMatrix::rotationX((PI/(segmentSize-1)*2)*j);
        for(uint i = 0; i < segmentSize; i++)
        {
            if(segmentSize*j + i >= vertexCount) LOG_ERROR("critical 1", DR_ERROR);
            mVertices[segmentSize*j + i] = mVertices[i].transformNormal(rot);//*DRRandom::rReal(1.02, 0.98f);
            mColors[segmentSize*j + i] = DRColor((float)i/(float)segmentSize, (float)j/(float)segmentSize, fabs((float)i/((float)j+0.001f)));
        }
    }
	mVertexCount = vertexCount;

    // generate indices to render sphere as QUAD_STRIP
    for(uint j = 0; j < segmentSize-1; j++)
    {
        for(uint i = 0; i < segmentSize; i++)
        {
            if(i*2+1+(segmentSize*2)*j >= indexCount) LOG_ERROR("critical 2", DR_ERROR);
            if(j*segmentSize + segmentSize+i >= vertexCount) LOG_ERROR("critical 3", DR_ERROR);
            
            mIndices[i*2+(segmentSize*2)*j]   = j*segmentSize + segmentSize+i; 
            mIndices[i*2+1+(segmentSize*2)*j] = j*segmentSize + i;    
         }
    }
	mIndexCount = indexCount;
    
    mRenderMode = GL_QUAD_STRIP;
   
    return DR_OK;
}



void DRGeometrieSphere::makeSphericalLandscape(GLuint numIterations, GLuint randomSeed)
{
    if(!mVertexCount) LOG_ERROR_VOID("keine Vertices zum manipulieren!");
    DRRandom::seed(randomSeed);
    
    const int threadCount = 4;   
    PlaneData planes(numIterations, randomSeed);
    LandscapeGenerateMultithreadData workingData[threadCount];
    SDL_Thread* threads[threadCount];
    
    for(int i = 0; i < threadCount; i++)
    {
        workingData[i].planes = &planes;
        workingData[i].vertices = &mVertices[mVertexCount/threadCount*i];
        workingData[i].vertexCount = mVertexCount/threadCount;
#if SDL_VERSION_ATLEAST(1,3,0)
		threads[i] = SDL_CreateThread(makeLandscapeThread, "DRGeoLSC" ,&workingData[i]);
#else
		threads[i] = SDL_CreateThread(makeLandscapeThread, &workingData[i]);
#endif
		printf("thread: %d, vertexIndex: %d, vertexCount: %d, ges vertexCount: %d\n",i, mVertexCount/threadCount*i, mVertexCount/threadCount, mVertexCount);
    }
    
    for(int i = 0; i < threadCount; i++)
    {
        int returnValue = 0;
        SDL_WaitThread(threads[i], &returnValue);
        if(returnValue)
        {
            LOG_WARNING("Fehler in Thread occured");
            DRLog.writeToLog("Thread %d return with error: %d", i, returnValue);            
        }
    }    
}

int DRGeometrieSphere::makeLandscapeThread(void* data)
{
    LandscapeGenerateMultithreadData* d = (LandscapeGenerateMultithreadData*)data;
    PlaneData* planes = d->planes;
    DRVector3* vertices = d->vertices;
            
    int m = 1;
    DRPlane pl;
    
    for(uint i = 0; i < planes->absPlaneCount; i++)
    {
        planes->lock();
        //plane gibts noch nicht? dann machen wir eine neue
        if(planes->planeCount < i)
        {
           //DRRandom::seed(planes->seed);
		   //planes->seed = rand();
           DRVector3 n = DRRandom::rVector3(1.0f);           
           pl = DRPlane(n, n.length());
           planes->planes[i] = pl;
           planes->planeCount++;
        }
        else
        {
            //vorhandene Plane holen
            pl = planes->planes[i];
        }        
        planes->unlock();
        m = -m;
        
        for(int j = 0; j < d->vertexCount; j++)
        {
            float d = 1.0f -  vertices[j].length();
            if(pl.dotCoords(vertices[j]) >= 1)
            {
                vertices[j] = vertices[j] * (1.0f+(float)m/(float)planes->absPlaneCount);
            }
            else
            {
                vertices[j] = vertices[j] * (1.0f-(float)m/(float)planes->absPlaneCount);
            }
        }
    }
    return 0;
}
