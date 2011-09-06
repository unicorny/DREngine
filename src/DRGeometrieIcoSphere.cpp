#include "Engine2Main.h"

DRGeometrieIcoSphere::DRGeometrieIcoSphere()
{
    
}

DRGeometrieIcoSphere::~DRGeometrieIcoSphere()
{
    
}

DRReturn DRGeometrieIcoSphere::initIcoSphere(GLuint subdivide)
{
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
    
}

DRReturn DRGeometrieIcoSphere::addNewFacesAtBorder()
{
    
}
DRReturn DRGeometrieIcoSphere::grabIndicesFromFaces()
{
    
}