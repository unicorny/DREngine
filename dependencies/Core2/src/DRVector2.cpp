#include "Core2Main.h"

// 2D-Vektor mit Matrix multiplizieren
DRVector2 DRVector2::transformCoords(const DRMatrix& m, DRReal* const pfOutW/* = NULL*/) const
{
    // Vektor mit Matrix transformieren
	DRVector2 vResult(x * m(0,0) + y * m(1,0) + m(3,0),
		          x * m(0,1) + y * m(1,1) + m(3,1));

	// Vierte Koordinate (w) berechnen. Wenn diese ungleich eins
	// ist, m�ssen die anderen Vektorelemente durch sie geteilt
	// werden.
	const DRReal w = x * m.m14 + y * m(1,3) + m(3,3);
	if(w != 1.0f) vResult /= w;

	// Wenn erw�nscht, w kopieren
	if(pfOutW) *pfOutW = w;

	return vResult.normalize() * length();
}

//--------------------------------------------------------------------
DRVector2 DRVector2::transformNormal(const DRMatrix& m) const
{
    // Vektorl�nge berechnen
	const DRReal fLength = length();
	if(fLength == 0.0f) return *this;

	// Transponierte inverse Matrix berechnen
	//const DRMatrix mTransform(DRMatrixTranspose(DRMatrixInvert(m)));
	const DRMatrix mTransform(m.invert().transpose());

	// Vektor mit Matrix transformieren und ursprüngliche Länge wiederherstellen
	return DRVector2(x * mTransform(0,0) + y * mTransform(1,0),
                     x * mTransform(0,1) + y * mTransform(1,1)).normalize() * fLength;
}
