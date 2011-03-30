#include "Core2Main.h"

/********************************************************************
	 _________        __    _____
	/\___  ___\      /\_\  /\  __\
	\/__/\ \__/ _  __\/_/_ \ \ \_\\   ____    _____      __
	    \ \ \  /\`´__\ /\ \ \ \  __\ /\ __\_ /\  __\   /´__`\
	     \ \ \ \ \ \/  \ \ \ \ \ \_\\\ \\_\ \\ \____\ /\  __/
	      \ \_\ \ \_\   \ \_\ \ \____\\ \___\ \ \____\\ \____\
	       \/_/  \/_/    \/_/  \/____/ \/__/   \/____/ \/____/

	DRMatrix.h
	==========
	Diese Datei ist Teil der TriBase-Engine.

	Zweck:
	Matrizenrechnung

	Autor:
	David Scherfgen

********************************************************************/

// ******************************************************************
// Translationsmatrix berechnen
DRMatrix DRMatrix::translation(const DRVector3& v)
{
	return DRMatrix(1.0f, 0.0f, 0.0f, v.x,
		           0.0f, 1.0f, 0.0f, v.y,
				   0.0f, 0.0f, 1.0f, v.z,
				   0.0f, 0.0f, 0.0f, 1.0f);
}

// ******************************************************************
// Rotationsmatrix für Rotation um die x-Achse berechnen
DRMatrix DRMatrix::rotationX(const DRReal f)
{
	DRMatrix mResult;

	// Rotationsmatrix berechnen
	mResult.m11 = 1.0f; mResult.m12 = 0.0f; mResult.m13 = 0.0f; mResult.m14 = 0.0f;
	mResult.m21 = 0.0f;                                         mResult.m24 = 0.0f;
	mResult.m31 = 0.0f;                                         mResult.m34 = 0.0f;
	mResult.m41 = 0.0f; mResult.m42 = 0.0f; mResult.m43 = 0.0f; mResult.m44 = 1.0f;

	mResult.m22 = mResult.m33 = cosf(f);
	mResult.m23 = sinf(f);
	mResult.m32 = -mResult.m23;

	return mResult;
}

// ******************************************************************
// Rotationsmatrix für Rotation um die y-Achse berechnen
DRMatrix DRMatrix::rotationY(const DRReal f)
{
	DRMatrix mResult;

	// Rotationsmatrix berechnen
	                    mResult.m12 = 0.0f;                     mResult.m14 = 0.0f;
	mResult.m21 = 0.0f; mResult.m22 = 1.0f; mResult.m23 = 0.0f; mResult.m24 = 0.0f;
	                    mResult.m32 = 0.0f;                     mResult.m34 = 0.0f;
	mResult.m41 = 0.0f; mResult.m42 = 0.0f; mResult.m43 = 0.0f; mResult.m44 = 1.0f;

	mResult.m11 = mResult.m33 = cosf(f);
	mResult.m31 = sinf(f);
	mResult.m13 = -mResult.m31;

	return mResult;
}

// ******************************************************************
// Rotationsmatrix für Rotation um die z-Achse berechnen
DRMatrix DRMatrix::rotationZ(const DRReal f)
{
	DRMatrix mResult;

	// Rotationsmatrix berechnen
	                                        mResult.m13 = 0.0f; mResult.m14 = 0.0f;
	                                        mResult.m23 = 0.0f; mResult.m24 = 0.0f;
	mResult.m31 = 0.0f; mResult.m32 = 0.0f; mResult.m33 = 1.0f; mResult.m34 = 0.0f;
	mResult.m41 = 0.0f; mResult.m42 = 0.0f; mResult.m43 = 0.0f; mResult.m44 = 1.0f;

	mResult.m11 = mResult.m22 = cosf(f);
	mResult.m12 = sinf(f);
	mResult.m21 = -mResult.m12;

	return mResult;
}

// ******************************************************************
// Rotiert um alle drei Achsen
DRMatrix DRMatrix::rotation(const DRReal x,
                            const DRReal y,
                            const DRReal z)
{
	/*return DRMatrixRotationZ(z) *
		   DRMatrixRotationX(x) *
		   DRMatrixRotationY(y);*/

    return rotationZ(z) *
           rotationX(x) *
           rotationY(y);
}

// ******************************************************************
// Rotiert um alle drei Achsen (Winkel in Vektor)
DRMatrix DRMatrix::rotation(const DRVector3& v)
{
	/*return DRMatrixRotationZ(v.z) *
		   DRMatrixRotationX(v.x) *
		   DRMatrixRotationY(v.y);*/
    return  rotationZ(v.z) *
            rotationX(v.x) *
            rotationY(v.y);
}

// ******************************************************************
// Rotationsmatrix für Rotation um eine beliebige Achse berechnen
DRMatrix DRMatrix::rotationAxis(const DRVector3& v,
                                const DRReal f)
{
	// Sinus und Kosinus berechnen
	const DRReal fSin = sinf(-f);
	const DRReal fCos = cosf(-f);
	const DRReal fOneMinusCos = 1.0f - fCos;

	// Achsenvektor normalisieren
	const DRVector3 vAxis(v.normalize() );

	// Matrix erstellen
	return DRMatrix((vAxis.x * vAxis.x) * fOneMinusCos + fCos,
		            (vAxis.x * vAxis.y) * fOneMinusCos - (vAxis.z * fSin),
				    (vAxis.x * vAxis.z) * fOneMinusCos + (vAxis.y * fSin),
					0.0f,
					(vAxis.y * vAxis.x) * fOneMinusCos + (vAxis.z * fSin),
					(vAxis.y * vAxis.y) * fOneMinusCos + fCos,
					(vAxis.y * vAxis.z) * fOneMinusCos - (vAxis.x * fSin),
					0.0f,
					(vAxis.z * vAxis.x) * fOneMinusCos - (vAxis.y * fSin),
					(vAxis.z * vAxis.y) * fOneMinusCos + (vAxis.x * fSin),
					(vAxis.z * vAxis.z) * fOneMinusCos + fCos,
					0.0f,
					0.0f,
					0.0f,
					0.0f,
					1.0f);
}

// ******************************************************************
// Skalierungsmatrix berechnen
DRMatrix DRMatrix::scaling(const DRVector3& v)
{
	// Skalierungsmatrix berechnen
	return DRMatrix(v.x,  0.0f, 0.0f, 0.0f,
		            0.0f, v.y,  0.0f, 0.0f,
					0.0f, 0.0f, v.z,  0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
}

// ******************************************************************
// Liefert eine Achsenmatrix
DRMatrix DRMatrix::axes(const DRVector3& vXAxis,
                        const DRVector3& vYAxis,
                        const DRVector3& vZAxis)
{
	return DRMatrix(vXAxis.x, vXAxis.y, vXAxis.z, 0.0f,
		            vYAxis.x, vYAxis.y, vYAxis.z, 0.0f,
					vZAxis.x, vZAxis.y, vZAxis.z, 0.0f,
					0.0f,     0.0f,     0.0f,     1.0f);
}

// ******************************************************************
// Determinante einer Matrix berechnen
DRReal DRMatrix::det() const
{
	// Determinante der linken oberen 3x3-Teilmatrix berechnen
	return m11 * (m22 * m33 - m23 * m32) -
           m12 * (m21 * m33 - m23 * m31) +
           m13 * (m21 * m32 - m22 * m31);
}

// ******************************************************************
// Invertierte Matrix berechnen
DRMatrix DRMatrix::invert() const
{
	// Kehrwert der Determinante vorberechnen
	DRReal fInvDet = det();
	if(fInvDet == 0.0f) return identity();
	fInvDet = 1.0f / fInvDet;

	// Invertierte Matrix berechnen
	DRMatrix mResult;
	mResult.m11 =  fInvDet * (m22 * m33 - m23 * m32);
	mResult.m12 = -fInvDet * (m12 * m33 - m13 * m32);
	mResult.m13 =  fInvDet * (m12 * m23 - m13 * m22);
	mResult.m14 =  0.0f;
	mResult.m21 = -fInvDet * (m21 * m33 - m23 * m31);
	mResult.m22 =  fInvDet * (m11 * m33 - m13 * m31);
	mResult.m23 = -fInvDet * (m11 * m23 - m13 * m21);
	mResult.m24 =  0.0f;
	mResult.m31 =  fInvDet * (m21 * m32 - m22 * m31);
	mResult.m32 = -fInvDet * (m11 * m32 - m12 * m31);
	mResult.m33 =  fInvDet * (m11 * m22 - m12 * m21);
	mResult.m34 =  0.0f;
	mResult.m41 = -(m41 * mResult.m11 + m42 * mResult.m21 + m43 * mResult.m31);
	mResult.m42 = -(m41 * mResult.m12 + m42 * mResult.m22 + m43 * mResult.m32);
	mResult.m43 = -(m41 * mResult.m13 + m42 * mResult.m23 + m43 * mResult.m33);
	mResult.m44 =  1.0f;

	return mResult;
}

// ******************************************************************
// Transponierte Matrix berechnen
DRMatrix DRMatrix::transpose() const
{
	// Matrix transponieren
	return DRMatrix(m11, m21, m31, m41,
		            m12, m22, m32, m42,
					m13, m23, m33, m43,
					m14, m24, m34, m44);
}

// ******************************************************************
// Projektionsmatrix berechnen
DRMatrix DRMatrix::projection(const DRReal fFOV,
                              const DRReal fAspect,
                              const DRReal fNearPlane,
                              const DRReal fFarPlane)
{
	const DRReal s = 1.0f / tanf(fFOV * 0.5f);
	const DRReal Q = fFarPlane / (fFarPlane - fNearPlane);

	return DRMatrix(s / fAspect, 0.0f, 0.0f, 0.0f,
		            0.0f, s, 0.0f, 0.0f,
					0.0f, 0.0f, Q, 1.0f,
					0.0f, 0.0f, -Q * fNearPlane, 0.0f);
}

// ******************************************************************
// Kameramatrix berechnen
DRMatrix DRMatrix::camera(const DRVector3& vPos,
                          const DRVector3& vLookAt,
                          const DRVector3& vUp) // = DRVector3(0.0f, 1.0f, 0.0f)
{
	// Die z-Achse des Kamerakoordinatensystems berechnen
	DRVector3 vZAxis(DRVector3(vLookAt - vPos).normalize());

	// Die x-Achse ist das Kreuzprodukt aus y- und z-Achse
	DRVector3 vXAxis(vUp.cross(vZAxis).normalize());

	// y-Achse berechnen
	DRVector3 vYAxis(vZAxis.cross(vXAxis).normalize());

	// Rotationsmatrix erzeugen und die Translationsmatrix mit ihr multiplizieren
//	return DRMatrixTranslation(-vPos) *
    return translation(-vPos) *
	       DRMatrix(vXAxis.x, vYAxis.x, vZAxis.x, 0.0f,
		            vXAxis.y, vYAxis.y, vZAxis.y, 0.0f,
				    vXAxis.z, vYAxis.z, vZAxis.z, 0.0f,
					0.0f,     0.0f,     0.0f,     1.0f);
}

// ******************************************************************
// Texturmatrix berechnen
DRMatrix DRMatrix::toTex2DRMatrix() const
{
	// Texturmatrix berechnen
	return DRMatrix(m11, m12, m14, 0.0f,
		            m21, m22, m24, 0.0f,
					m41, m42, m44, 0.0f,
					0.0f,  0.0f,  0.0f,  1.0f);
}

// ******************************************************************
DRMatrix DRMatrix::getglMatrix() const
{
	return DRMatrix(m11, m21, m31, m41,
                    m12, m22, m32, m42,
                    m13, m23, m33, m43,
                    m14, m24, m34, m44);
}
