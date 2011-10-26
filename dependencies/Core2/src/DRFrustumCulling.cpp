#include "Core2Main.h"

DRFrustumCulling::DRFrustumCulling(DRMatrix view, DRMatrix projection)
{
    DRMatrix temp(projection * view);
    for(int j = 0; j < 4; j++)
    {
        // linke clipping ebene
        mViewPlanes[0].v[j] = -(temp(3,j) + temp(0,j));
        // rechte clipping ebene
        mViewPlanes[1].v[j] = -(temp(3,j) - temp(0,j));
        // obere clipping ebene
        mViewPlanes[2].v[j] = -(temp(3,j) - temp(1,j));
        // untere clipping ebene
        mViewPlanes[3].v[j] = -(temp(3,j) + temp(1,j));
        // nahe clipping ebene
        mViewPlanes[4].v[j] = -(temp(3,j) + temp(2,j));
        // ferne clipping ebene
        mViewPlanes[5].v[j] = -(temp(3,j) + temp(2,j));
    }
    
}
        