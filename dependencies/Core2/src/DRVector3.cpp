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

DRVector3 DRVector3::transformCoords(const DRMatrix& m, float* const pfOutW) const
{
    // Vektor mit Matrix multiplizieren
	DRVector3 vResult(x * m(0,0) + y * m(1,0) + z * m(2,0) + m(3,0),
                      x * m(0,1) + y * m(1,1) + z * m(2,1) + m(3,1),
                      x * m(0,2) + y * m(1,2) + z * m(2,2) + m(3,2));

	// Vierte Koordinate (w) berechnen. Wenn diese ungleich eins
	// ist, müssen die anderen Vektorelemente durch sie geteilt
	// werden.
	const float w = x * m(0,3) + y * m(1,3) + z * m(2,3) + m(3,3);
	if(w != 1.0f) vResult /= w;

	// Wenn erwünscht, w kopieren
	if(pfOutW) *pfOutW = w;

	return vResult;
}

