#include "Core2Main.h"

DRFrustumCulling::DRFrustumCulling(DRObjekt* camera, DRReal fovAngle, DRReal aspectRatio, DRReal zNearPlane, DRReal zFarPlane)
: mCamera(camera), mNearPlane(zNearPlane), mFarPlane(zFarPlane), mAspectRatio(aspectRatio), mFovAngle(fovAngle), mFovAngleTangent(tan(fovAngle)), mSphereFactorX(0.0f)
{
    updateMatritzen(fovAngle, aspectRatio, zNearPlane, zFarPlane);    
}

#define ANG2RAD 3.14159265358979323846/250.0

DRReturn DRFrustumCulling::updateMatritzen(DRReal fovAngle, DRReal aspectRatio, DRReal zNearPlane, DRReal zFarPlane)
{
    if(!mCamera) LOG_ERROR("no cam!", DR_ERROR);
    
    this->mAspectRatio = aspectRatio;
    this->mFovAngle = fovAngle * ANG2RAD;
    this->mNearPlane = zNearPlane;
    this->mFarPlane = zFarPlane;	
    this->mFovAngleTangent = tan(this->mFovAngle);
    double angleX = atan(this->mFovAngleTangent*this->mAspectRatio);
    this->mSphereFactorX = 1.0f/cos(angleX);
    
    DRMatrix temp(mCamera->getMatrix() * DRMatrix::perspective_projection(fovAngle, aspectRatio, zNearPlane, zFarPlane));
    for(int j = 0; j < 4; j++)
    {
        // linke clipping ebene
        mViewPlanes[0].v[j] = -(temp(j,3) + temp(j,0));
        // rechte clipping ebene
        mViewPlanes[1].v[j] = -(temp(j,3) - temp(j,0));
        // obere clipping ebene
        mViewPlanes[2].v[j] = -(temp(j,3) - temp(j,1));
        // untere clipping ebene
        mViewPlanes[3].v[j] = -(temp(j,3) + temp(j,1));
        // nahe clipping ebene
        mViewPlanes[4].v[j] = -(temp(j,3) + temp(j,2));
        // ferne clipping ebene
        mViewPlanes[5].v[j] = -(temp(j,3) - temp(j,2));
    }
	//for(int i = 0; i < 6; i++)
		//mViewPlanes[i] = mViewPlanes[i].normalize();
    return DR_OK;
}

DRFrustumPosition DRFrustumCulling::isPointInFrustum(const DRVector3& point)
{
    DRReal pcz,pcx,pcy,aux;

    if(!mCamera) return FR_ERROR;
    // p = point
    // compute vector from camera position to p
    DRVector3 v = point - mCamera->getPosition();

    // compute and test the Z coordinate (Z -Axis)
    pcz = v.dot(-mCamera->getZAxis());
    if (pcz > mFarPlane || pcz < mNearPlane)
            return OUTSIDE;
  
    // compute and test the Y coordinate
    pcy = v.dot(mCamera->getYAxis());
    aux = pcz * mFovAngleTangent;
    if (pcy > aux || pcy < -aux)
            return OUTSIDE;

    // compute and test the X coordinate
    pcx = v.dot(mCamera->getXAxis());
    aux = aux * mAspectRatio;
    if (pcx > aux || pcx < -aux)
            return OUTSIDE;

    return INSIDE;
}

DRFrustumPosition DRFrustumCulling::isSphereInFrustum(const DRVector3& middlePoint, DRReal radius)
{
    float d1,d2;
    float az,ax,ay,zz1,zz2;
    DRFrustumPosition result = INSIDE;
    
    if(!mCamera) return FR_ERROR;

    DRVector3 v = middlePoint - mCamera->getPosition();

    az = v.dot(-mCamera->getZAxis());
    if (az > mFarPlane + radius || az < mNearPlane-radius)
            return OUTSIDE;

    ax = v.dot(mCamera->getXAxis());
    zz1 = az * mFovAngleTangent * mAspectRatio;
    d1 = this->mSphereFactorX * radius;
    if (ax > zz1+d1 || ax < -zz1-d1)
            return OUTSIDE;

    ay = v.dot(mCamera->getYAxis());
    zz2 = az * mFovAngleTangent;
    d2 = this->mSphereFactorX * radius;
    if (ay > zz2+d2 || ay < -zz2-d2)
            return OUTSIDE;

    if (az > mFarPlane - radius || az < mNearPlane+radius)
            result = INTERSECT;
    if (ay > zz2-d2 || ay < -zz2+d2)
            result = INTERSECT;
    if (ax > zz1-d1 || ax < -zz1+d1)
            result = INTERSECT;

    return result;
}

DRFrustumPosition DRFrustumCulling::classifyBox(const DRVector3& boxMin,
                                                const DRVector3& boxMax, 
                                                const DRMatrix& invTransform, 
                                                int planeIndex)
{
    DRPlane transformedPlane = mViewPlanes[planeIndex].transform(invTransform);
    
    // Nächsten und fernsten Punkt berechnen
    DRVector3 nearPoint(boxMax);
    DRVector3 farPoint(boxMin);
    for(int i = 0; i < 3; i++)
         if(transformedPlane.v[i] > 0.0f) {nearPoint.c[i] = boxMin.c[i]; farPoint.c[i] = boxMax.c[i];}
    
    // Prüfen, ob der nächste Punkt vor der Ebene liegt.
    // Falls ja, liegt der ganze Quader vor ihr.
    if(transformedPlane.dotCoords(nearPoint) > 0.0f) return BEVORE;
    
    // Prüfen, ob der fernste Punkt vor der Ebene liegt.
    // Falls ja, dann wird der Quader von der Ebene geteilt
    if(transformedPlane.dotCoords(farPoint) > 0.0f) return INTERSECT;
    
    return BEHIND;
}

DRFrustumPosition DRFrustumCulling::isBoxInFrustum(const DRVector3& boxMin, 
                                                   const DRVector3& boxMax, 
                                                   const DRMatrix& transform/* = DRMatrix::identity()*/)
{
    DRMatrix invMatrix = transform.invert();
    DRFrustumPosition result = INSIDE;
    for(int i = 0; i < 6; i++)
    {
        DRFrustumPosition localResult = classifyBox(boxMin, boxMax, invMatrix, i);
        if(localResult == BEVORE) return OUTSIDE;
        else if(localResult == INTERSECT) result = INTERSECT;
    }
    return result;    
}