/* 
 * File:   DRFrustumCulling.h
 * Author: Dario
 *
 * Created on 20. Oktober 2011, 21:39
 */

#ifndef __DR_FRUSTUM_CULLING_H
#define	__DR_FRUSTUM_CULLING_H

class CORE2_API DRFrustumCulling
{
public:
    DRFrustumCulling(DRMatrix view, DRMatrix projection);
private:
    
    DRPlane mViewPlanes[6];
    
};

#endif	/* __DR_FRUSTUM_CULLING_H */

