#include "Core2Main.h"

// 2D-Vektor mit Matrix multiplizieren
DRVector2 DRVector2::transformCoords(const DRMatrix& m, DRReal* const pfOutW/* = NULL*/) const
{
    // Vektor mit Matrix transformieren
	DRVector2 vResult(x * m.m11 + y * m.m21 + m.m41,
		              x * m.m12 + y * m.m22 + m.m42);

	// Vierte Koordinate (w) berechnen. Wenn diese ungleich eins
	// ist, müssen die anderen Vektorelemente durch sie geteilt
	// werden.
	const DRReal w = x * m.m14 + y * m.m24 + m.m44;
	if(w != 1.0f) vResult /= w;

	// Wenn erwünscht, w kopieren
	if(pfOutW) *pfOutW = w;

	return vResult.normalize() * length();
}

//--------------------------------------------------------------------
DRVector2 DRVector2::transformNormal(const DRMatrix& m) const
{
    // Vektorlänge berechnen
	const DRReal fLength = length();
	if(fLength == 0.0f) return *this;

	// Transponierte inverse Matrix berechnen
	//const DRMatrix mTransform(DRMatrixTranspose(DRMatrixInvert(m)));
	const DRMatrix mTransform(m.invert().transpose());

	// Vektor mit Matrix transformieren und ursprüngliche Länge wiederherstellen
	return DRVector2(x * mTransform.m11 + y * mTransform.m21,
                     x * mTransform.m12 + y * mTransform.m22).normalize() * fLength;
}
