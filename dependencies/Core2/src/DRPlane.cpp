#include "Core2Main.h"

DRPlane DRPlane::transform(const DRMatrix& m) const
{
    const float _a = a * m(0,0) + b * m(1,0) + c * m(2,0);
    const float _b = a * m(0,1) + b * m(1,1) + c * m(2,1);
    const float _c = a * m(0,2) + b * m(1,2) + c * m(2,2);

    return DRPlane(_a, _b, _c, d - (_a * m(3,0) + _b * m(3,1) + _c * m(3,2)));
}
