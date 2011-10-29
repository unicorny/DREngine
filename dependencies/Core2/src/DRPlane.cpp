#include "Core2Main.h"

DRPlane DRPlane::transform(const DRMatrix& m) const
{
    const float _a = a * m(0,0) + b * m(0,1) + c * m(0,2);
    const float _b = a * m(1,0) + b * m(1,1) + c * m(1,2);
    const float _c = a * m(2,0) + b * m(2,1) + c * m(2,2);

    return DRPlane(_a, _b, _c, d - (_a * m(0,3) + _b * m(1,3) + _c * m(2,3)));
}
