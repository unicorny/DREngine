#include "Core2Main.h"

DRVector3 DRVector3::transformNormal(const DRMatrix& m) const
{
    // Vektorlänge berechnen
	const float length = DRVector3::length();
	if(length == 0.0f) return *this;

	// Transponierte invertierte Matrix berechnen
	const DRMatrix transform(m.invert().transpose());

	// Vektor mit Matrix transformieren und ursprüngliche Länge wiederherstellen
        return DRVector3(x * transform(0,0) + y * transform(1,0) + z * transform(2,0),
                         x * transform(0,1) + y * transform(1,1) + z * transform(2,1),
                         x * transform(0,2) + y * transform(1,2) + z * transform(2,2)).normalize() * length;
}