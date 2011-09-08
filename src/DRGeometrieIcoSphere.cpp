#include "Engine2Main.h"

DRGeometrieIcoSphere::DRGeometrieIcoSphere()
: DRGeometrieSphere()
{
    
}

DRGeometrieIcoSphere::~DRGeometrieIcoSphere()
{
    
}

DRReturn DRGeometrieIcoSphere::initIcoSphere(GLuint subdivide)
{
    float tao = 1.61803399;
    
    GLuint indices[] = {1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
                        1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
                        3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
                        10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7};  
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
    
    for(int i = 0; i < 12; i++)
    {
        float percent = (float)i*(1.0f/12.0f);
        printf("percent: %f\n", percent);
        mColors[i] = DRColor(percent, 0.0f, 0.0f);//1.0f-percent, (percent+0.0001f)/2.0f);
    }
    
    mRenderMode = GL_TRIANGLES;
            
    return DR_OK;
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
