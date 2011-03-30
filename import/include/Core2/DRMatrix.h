
/*/*************************************************************************
 *                                                                       *
 * Core, Core-Lib for my programs, Core doesn't need any libraries		 *
 * Copyright (C) 2009, 2010, 2011 Dario Rekowski.						 *
 * Email: dariofrodo@gmx.de   Web: www.mathe-programme.de.tk             *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * any later version.													 *
 *																		 *
 * This program is distributed in the hope that it will be useful,		 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of		 *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the		 *
 * GNU General Public License for more details.							 *
 *																		 *
 * You should have received a copy of the GNU General Public License	 *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

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

#ifndef __DR_MATRIX__
#define __DR_MATRIX__

// Verwendete Symbole vordefinieren
//class DRMatrix;


// ******************************************************************
// Die Matrixklasse
class CORE2_API DRMatrix
{
public:
	// Variablen
	union
	{
		struct
		{
			DRReal m11, m12, m13, m14,	// Elemente der Matrix
				  m21, m22, m23, m24,
				  m31, m32, m33, m34,
				  m41, m42, m43, m44;
		};

		DRReal		m[4][4];			// Zweidimensionales Array der Elemente
		DRReal		n[16];				// Eindimensionales Array der Elemente
	};

	// Konstruktoren
	DRMatrix() {}

	DRMatrix(const DRMatrix& m) : m11(m.m11), m12(m.m12), m13(m.m13), m14(m.m14),
                                  m21(m.m21), m22(m.m22), m23(m.m23), m24(m.m24),
								  m31(m.m31), m32(m.m32), m33(m.m33), m34(m.m34),
								  m41(m.m41), m42(m.m42), m43(m.m43), m44(m.m44) {}

	DRMatrix(DRReal _m11, DRReal _m12, DRReal _m13, DRReal _m14,
			 DRReal _m21, DRReal _m22, DRReal _m23, DRReal _m24,
			 DRReal _m31, DRReal _m32, DRReal _m33, DRReal _m34,
			 DRReal _m41, DRReal _m42, DRReal _m43, DRReal _m44) : m11(_m11), m12(_m12), m13(_m13), m14(_m14),
			                                                   m21(_m21), m22(_m22), m23(_m23), m24(_m24),
															   m31(_m31), m32(_m32), m33(_m33), m34(_m34),
															   m41(_m41), m42(_m42), m43(_m43), m44(_m44) {}

	// Casting-Opeatoren
	operator DRReal* ()					{return (DRReal*)(n);}

	// Zugriffsoperatoren
	DRReal& operator () (int iRow, int iColumn) {return m[iRow - 1][iColumn - 1];}
	DRReal operator () (int iRow, int iColumn) const {return m[iRow - 1][iColumn - 1];}

    // Zuweisungsoperatoren (mit Rechnung)
	DRMatrix& operator += (const DRMatrix& m)
	{
		m11 += m.m11; m12 += m.m12; m13 += m.m13; m14 += m.m14;
		m21 += m.m21; m22 += m.m22; m23 += m.m23; m24 += m.m24;
		m31 += m.m31; m32 += m.m32; m33 += m.m33; m34 += m.m34;
		m41 += m.m41; m42 += m.m42; m43 += m.m43; m44 += m.m44;
		return *this;
	}

	DRMatrix& operator -= (const DRMatrix& m)
	{
		m11 -= m.m11; m12 -= m.m12; m13 -= m.m13; m14 -= m.m14;
		m21 -= m.m21; m22 -= m.m22; m23 -= m.m23; m24 -= m.m24;
		m31 -= m.m31; m32 -= m.m32; m33 -= m.m33; m34 -= m.m34;
		m41 -= m.m41; m42 -= m.m42; m43 -= m.m43; m44 -= m.m44;
		return *this;
	}

	DRMatrix& operator *= (const DRMatrix& m)
	{
		return *this = DRMatrix(m.m11 * m11 + m.m21 * m12 + m.m31 * m13 + m.m41 * m14,
								m.m12 * m11 + m.m22 * m12 + m.m32 * m13 + m.m42 * m14,
								m.m13 * m11 + m.m23 * m12 + m.m33 * m13 + m.m43 * m14,
								m.m14 * m11 + m.m24 * m12 + m.m34 * m13 + m.m44 * m14,
								m.m11 * m21 + m.m21 * m22 + m.m31 * m23 + m.m41 * m24,
								m.m12 * m21 + m.m22 * m22 + m.m32 * m23 + m.m42 * m24,
								m.m13 * m21 + m.m23 * m22 + m.m33 * m23 + m.m43 * m24,
								m.m14 * m21 + m.m24 * m22 + m.m34 * m23 + m.m44 * m24,
								m.m11 * m31 + m.m21 * m32 + m.m31 * m33 + m.m41 * m34,
								m.m12 * m31 + m.m22 * m32 + m.m32 * m33 + m.m42 * m34,
								m.m13 * m31 + m.m23 * m32 + m.m33 * m33 + m.m43 * m34,
								m.m14 * m31 + m.m24 * m32 + m.m34 * m33 + m.m44 * m34,
								m.m11 * m41 + m.m21 * m42 + m.m31 * m43 + m.m41 * m44,
								m.m12 * m41 + m.m22 * m42 + m.m32 * m43 + m.m42 * m44,
								m.m13 * m41 + m.m23 * m42 + m.m33 * m43 + m.m43 * m44,
								m.m14 * m41 + m.m24 * m42 + m.m34 * m43 + m.m44 * m44);
	}

	DRMatrix& operator *= (const DRReal f)
	{
		m11 *= f; m12 *= f; m13 *= f; m14 *= f;
		m21 *= f; m22 *= f; m23 *= f; m24 *= f;
		m31 *= f; m32 *= f; m33 *= f; m34 *= f;
		m41 *= f; m42 *= f; m43 *= f; m44 *= f;
		return *this;
	}

	DRMatrix& operator /= (const DRMatrix& m)
	{
		return *this *= m.invert();
	}

	DRMatrix& operator /= (const DRReal f)
	{
		m11 /= f; m12 /= f; m13 /= f; m14 /= f;
		m21 /= f; m22 /= f; m23 /= f; m24 /= f;
		m31 /= f; m32 /= f; m33 /= f; m34 /= f;
		m41 /= f; m42 /= f; m43 /= f; m44 /= f;
		return *this;
	}


    // Arithmetische Operatoren
    inline DRMatrix operator + (const DRMatrix& b)	const {return DRMatrix(m11 + b.m11, m12 + b.m12, m13 + b.m13, m14 + b.m14, m21 + b.m21, m22 + b.m22, m23 + b.m23, m24 + b.m24, m31 + b.m31, m32 + b.m32, m33 + b.m33, m34 + b.m34, m41 + b.m41, m42 + b.m42, m43 + b.m43, m44 + b.m44);}
    inline DRMatrix operator - (const DRMatrix& b)	const {return DRMatrix(m11 - b.m11, m12 - b.m12, m13 - b.m13, m14 - b.m14, m21 - b.m21, m22 - b.m22, m23 - b.m23, m24 - b.m24, m31 - b.m31, m32 - b.m32, m33 - b.m33, m34 - b.m34, m41 - b.m41, m42 - b.m42, m43 - b.m43, m44 - b.m44);}
    inline DRMatrix operator - ()					const {return DRMatrix(-m11, -m12, -m13, -m14, -m21, -m22, -m23, -m24, -m31, -m32, -m33, -m34, -m41, -m42, -m43, -m44);}

    inline DRMatrix operator * (const DRMatrix& b) const
    {
        return DRMatrix(b.m11 * m11 + b.m21 * m12 + b.m31 * m13 + b.m41 * m14,
                        b.m12 * m11 + b.m22 * m12 + b.m32 * m13 + b.m42 * m14,
                        b.m13 * m11 + b.m23 * m12 + b.m33 * m13 + b.m43 * m14,
                        b.m14 * m11 + b.m24 * m12 + b.m34 * m13 + b.m44 * m14,
                        b.m11 * m21 + b.m21 * m22 + b.m31 * m23 + b.m41 * m24,
                        b.m12 * m21 + b.m22 * m22 + b.m32 * m23 + b.m42 * m24,
                        b.m13 * m21 + b.m23 * m22 + b.m33 * m23 + b.m43 * m24,
                        b.m14 * m21 + b.m24 * m22 + b.m34 * m23 + b.m44 * m24,
                        b.m11 * m31 + b.m21 * m32 + b.m31 * m33 + b.m41 * m34,
                        b.m12 * m31 + b.m22 * m32 + b.m32 * m33 + b.m42 * m34,
                        b.m13 * m31 + b.m23 * m32 + b.m33 * m33 + b.m43 * m34,
                        b.m14 * m31 + b.m24 * m32 + b.m34 * m33 + b.m44 * m34,
                        b.m11 * m41 + b.m21 * m42 + b.m31 * m43 + b.m41 * m44,
                        b.m12 * m41 + b.m22 * m42 + b.m32 * m43 + b.m42 * m44,
                        b.m13 * m41 + b.m23 * m42 + b.m33 * m43 + b.m43 * m44,
                        b.m14 * m41 + b.m24 * m42 + b.m34 * m43 + b.m44 * m44);
    }

    inline DRMatrix operator * (const DRReal f) const
    {
        return DRMatrix(m11 * f, m12 * f, m13 * f, m14 * f,
                        m21 * f, m22 * f, m23 * f, m24 * f,
                        m31 * f, m32 * f, m33 * f, m34 * f,
                        m41 * f, m42 * f, m43 * f, m44 * f);
    }

    inline DRMatrix operator / (const DRMatrix& b) const {return *this * b.invert();}

    inline DRMatrix operator / (const DRReal f) const
    {
        return DRMatrix(m11 / f, m12 / f, m13 / f, m14 / f,
                        m21 / f, m22 / f, m23 / f, m24 / f,
                        m31 / f, m32 / f, m33 / f, m34 / f,
                        m41 / f, m42 / f, m43 / f, m44 / f);
    }

    // Vergleichsoperatoren
    inline bool operator == (const DRMatrix& b) const
    {
        if(m11 != b.m11) return false;
        if(m12 != b.m12) return false;
        if(m13 != b.m13) return false;
        if(m14 != b.m14) return false;
        if(m21 != b.m21) return false;
        if(m22 != b.m22) return false;
        if(m23 != b.m23) return false;
        if(m24 != b.m24) return false;
        if(m31 != b.m31) return false;
        if(m32 != b.m32) return false;
        if(m33 != b.m33) return false;
        if(m34 != b.m34) return false;
        if(m41 != b.m41) return false;
        if(m42 != b.m42) return false;
        if(m43 != b.m43) return false;
        return m44 == b.m44;
    }

    // Vergleichsoperatoren
    inline bool operator != (const DRMatrix& b) const
    {
        if(m11 != b.m11) return true;
        if(m12 != b.m12) return true;
        if(m13 != b.m13) return true;
        if(m14 != b.m14) return true;
        if(m21 != b.m21) return true;
        if(m22 != b.m22) return true;
        if(m23 != b.m23) return true;
        if(m24 != b.m24) return true;
        if(m31 != b.m31) return true;
        if(m32 != b.m32) return true;
        if(m33 != b.m33) return true;
        if(m34 != b.m34) return true;
        if(m41 != b.m41) return true;
        if(m42 != b.m42) return true;
        if(m43 != b.m43) return true;
        return m44 != b.m44;
    }

    // ******************************************************************
    // Funktionen deklarieren
    inline static DRMatrix	identity() {return DRMatrix(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);}	// Identitätsmatrix liefern
    static DRMatrix	translation(const DRVector3& v);                          			// Translationsmatrix (Verschiebungsmatrix) berechnen
    static DRMatrix	rotationX(const DRReal f);                                			// Rotationsmatrix um die X-Achse berechnen
    static DRMatrix	rotationY(const DRReal f);                                			// Rotationsmatrix um die Y-Achse berechnen
    static DRMatrix	rotationZ(const DRReal f);                                			// Rotationsmatrix um die Z-Achse berechnen
    static DRMatrix	rotation(const DRReal x, const DRReal y, const DRReal z); 			// Rotiert um alle drei Achsen
    static DRMatrix	rotation(const DRVector3& v);                             			// Rotiert um alle drei Achsen
    static DRMatrix	rotationAxis(const DRVector3& v, const DRReal f);					// Rotationsmatrix um eine beliebige Achse berechnen
    static DRMatrix	scaling(const DRVector3& v);										// Skalierungsmatrix berechnen
    static DRMatrix axes(const DRVector3& vXAxis, const DRVector3& vYAxis, const DRVector3& vZAxis); // Liefert eine Achsenmatrix
    DRMatrix invert()           const;
    DRReal	 det()              const;																// Determinante berechnen
    DRMatrix transpose()        const;																// Transponierte Matrix berechnen
    static DRMatrix projection(const DRReal fFOV, const DRReal fAspect, const DRReal fNearPlane, const DRReal fFarPlane);       // Projektionsmatrix berechnen
    static DRMatrix	camera(const DRVector3& vPos, const DRVector3& vLookAt, const DRVector3& vUp = DRVector3(0.0f, 1.0f, 0.0f));// Kameramatrix erzeugen
    DRMatrix toTex2DRMatrix() const;																							// In Texturmatrix umwandeln
    //! ist das gleiche wie transpose
    //! same as transpose
    DRMatrix getglMatrix() const;

};
// externer Operator
inline DRMatrix operator * (const DRReal f,
							const DRMatrix& m)
{
	return DRMatrix(m.m11 * f, m.m12 * f, m.m13 * f, m.m14 * f,
			        m.m21 * f, m.m22 * f, m.m23 * f, m.m24 * f,
					m.m31 * f, m.m32 * f, m.m33 * f, m.m34 * f,
					m.m41 * f, m.m42 * f, m.m43 * f, m.m44 * f);
}

// ******************************************************************

#endif
