#include "Engine2Main.h"

DRGeometrieIcoSphere::DRGeometrieIcoSphere(GLuint maxFaceBuffer)
: DRGeometrieSphere(), mMaxEbene(0), mMaxFaceBuffer(maxFaceBuffer)
{
    
}

DRGeometrieIcoSphere::~DRGeometrieIcoSphere()
{
    for(std::list<IcoSphereFace*>::iterator it = mFreeIcoFaceMemory.begin(); it != mFreeIcoFaceMemory.end(); it++)
    {
        DR_SAVE_DELETE(*it);
    }
    mFreeIcoFaceMemory.clear();
}

DRReturn DRGeometrieIcoSphere::initIcoSphere(u8 maxEbene)
{
    reset();
    float tao = 1.61803399;
    mMaxEbene = maxEbene;
    
    GLuint indices[] =     {1,4,0,   4,9,0,    4,5,9,    8,5,4,    1,8,4,
                            1,10,8,  10,3,8,   8,3,5,    3,2,5,    3,7,2,
                            3,10,7,  10,6,7,   6,11,7,   6,0,11,   6,1,0,
                            10,1,6,  11,0,9,   2,11,9,   5,2,9,    11,2,7};  
    
    GLuint neighbors[] =   {4,1,14,  2,16,0,   3,18,1,   7,2,4,    5,3,0,
                            15,6,4,  10,7,5,   6,8,3,    9,18,7,   10,19,8,
                            6,11,9,  15,12,10, 13,19,11, 14,16,12, 15,0,13,
                            5,14,11, 13,1,17,  19,16,18, 8,17,2,   17,9,12}; 
    
    const GLuint indexCount = 60;
    
    if(init(12, indexCount, 0, true))
        LOG_ERROR("Fehler bei init Geometrie", DR_ERROR);
    memcpy(mIndices, indices, sizeof(GLuint)*indexCount);
            
    mVertices[0] = DRVector3(-1.0f, 0.0f, tao);
    mVertices[1] = DRVector3(1.0f, 0.0f, tao);
    mVertices[2] = DRVector3(-1.0f, 0.0f, -tao);
    mVertices[3] = DRVector3(1.0f, 0.0f, -tao);
    
    mVertices[4] = DRVector3(0.0f, tao, 1.0f);
    mVertices[5] = DRVector3(0.0f, tao, -1.0f);
    mVertices[6] = DRVector3(0.0f, -tao, 1.0f);
    mVertices[7] = DRVector3(0.0f, -tao, -1.0f);
    
    mVertices[8] = DRVector3(tao, 1.0f, 0.0f);
    mVertices[9] = DRVector3(-tao, 1.0f, 0.0f);
    mVertices[10] = DRVector3(tao, -1.0f, 0.0f);
    mVertices[11] = DRVector3(-tao, -1.0f, 0.0f);
    
    //einfärben der Start Vertices
    for(int i = 0; i < 12; i++)
    {
        float percent = (float)i*(1.0f/12.0f);
        printf("percent: %f\n", percent);
        mColors[i] = DRColor(percent, 0.0f, 0.0f);//1.0f-percent, (percent+0.0001f)/2.0f);
    }
    
    // Initial Faces erstellen
    for(int iFace = 0; iFace < 20; iFace++)
    {
        //mRootSphereFaces[i]
        for(int i = 0; i < 3; i++)
        {
            mRootSphereFaces[iFace].mIndices[i] = indices[iFace*3+i];
            mRootSphereFaces[iFace].mNeighbors[i] = &mRootSphereFaces[neighbors[iFace*3+i]];
        }
    }
    
    
    mRenderMode = GL_TRIANGLES;
    
    //IcoSphereFace Konstruktor Test (nicht notwendig)
    IcoSphereFace f;
    if(f.mParent || f.mNeighbors[2] || f.mChilds[3] || f.mIndices[2])
        LOG_ERROR("IcoSphereFace Konstruktor Error", DR_ERROR);
            
    return DR_OK;
}

void DRGeometrieIcoSphere::reset()
{
    
}

DRGeometrieIcoSphere::IcoSphereFace* DRGeometrieIcoSphere::addNewFace(GLuint index1, GLuint index2, GLuint index3)
{
    GLuint index[3];
    index[0] = index1;
    index[1] = index2;
    index[2] = index3;
    
    return addNewFace(index);
}

DRGeometrieIcoSphere::IcoSphereFace* DRGeometrieIcoSphere::addNewFace(GLuint index[3])
{
    return NULL;
}

DRReturn DRGeometrieIcoSphere::addNewFacesAtBorder()
{
    return DR_OK;
}
DRReturn DRGeometrieIcoSphere::grabIndicesFromFaces()
{
    return DR_OK;
}
 //*/
// ------------------------- IcoSphereFace ---------------------------------------------
//

DRGeometrieIcoSphere::IcoSphereFace::IcoSphereFace()
: mParent(NULL)
{
    reset();
}

void DRGeometrieIcoSphere::IcoSphereFace::reset()
{
    mParent = NULL;
    memset(mNeighbors, 0, sizeof(IcoSphereFace*)*3);
    memset(mChilds, 0, sizeof(IcoSphereFace*)*4);
    memset(mIndices, 0, sizeof(GLuint)*3);
}

DRGeometrieIcoSphere::IcoSphereFace* DRGeometrieIcoSphere::newFace()
{
    if(mFreeIcoFaceMemory.size())
    {
        IcoSphereFace* f = mFreeIcoFaceMemory.back();
        mFreeIcoFaceMemory.pop_back();
        f->reset();
        return f;
    }
    else
    {
        return new IcoSphereFace;
    }
}

void DRGeometrieIcoSphere::deleteFace(DRGeometrieIcoSphere::IcoSphereFace* face)
{
    if(mFreeIcoFaceMemory.size() >= mMaxFaceBuffer)
    {
        DR_SAVE_DELETE(face);
    }
    else
    {
        mFreeIcoFaceMemory.push_back(face);
    }
}