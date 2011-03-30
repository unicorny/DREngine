/*/*************************************************************************
 *                                                                       *
 * Core, Core-Lib for my programs, Core doesn't need any libraries		 *
 * Copyright (C) 2007, 2008, 2009 Dario Rekowski.						 *
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


#ifndef __DR_VECTOR3__
#define __DR_VECTOR3__

/*
Vector3D Klasse, übernommen aus der TriBase-Engine von David Scherfgen
*/
class CORE2_API DRVector3
{
public:

	// Variablen
	union
	{
		struct
		{
			DRReal x;	// Koordinaten
			DRReal y;
			DRReal z;
		};

		struct
		{
			DRReal u;	// Texturkoordinaten
			DRReal v;
			DRReal w;
		};

		DRReal		c[3];		// Array der Koordinaten
	};

	// Konstruktoren
	DRVector3()	: x(0.0f), y(0.0f), z(0.0f)																					{}
	DRVector3(const DRVector3& v) : x(v.x), y(v.y), z(v.z)											{}
	DRVector3(const DRReal f) : x(f), y(f), z(f)														{}
	DRVector3(const DRReal _x, const DRReal _y, const DRReal _z) : x(_x), y(_y), z(_z)					{}
	DRVector3(const DRReal* pfComponent) : x(pfComponent[0]), y(pfComponent[1]), z(pfComponent[2])	{}
	//DRVector3(double dx, double dy, double dz): x(dx), y(dy), z(dz)									{}

	// Casting-Operatoren
	operator DRReal* ()					{return (DRReal*)(c);}

	// Zuweisungsoperatoren
	inline DRVector3& operator = (const DRVector3& v)	{x = v.x; y = v.y; z = v.z; return *this;}
	inline DRVector3& operator += (const DRVector3& v)	{x += v.x; y += v.y; z += v.z; return *this;}
	inline DRVector3& operator -= (const DRVector3& v)	{x -= v.x; y -= v.y; z -= v.z; return *this;}
	inline DRVector3& operator *= (const DRVector3& v)	{x *= v.x; y *= v.y; z *= v.z; return *this;}
	inline DRVector3& operator *= (const DRReal f)		{x *= f; y *= f; z *= f; return *this;}
	inline DRVector3& operator /= (const DRVector3& v)	{x /= v.x; y /= v.y; z /= v.z; return *this;}
	inline DRVector3& operator /= (const DRReal f)		{x /= f; y /= f; z /= f; return *this;}


	//********************************************************************************************************************++


// Arithmetische Operatoren
    inline DRVector3 operator + (const DRVector3& b) const	{return DRVector3(x + b.x, y + b.y, z + b.z);}
    inline DRVector3 operator - (const DRVector3& b) const	{return DRVector3(x - b.x, y - b.y, z - b.z);}
    inline DRVector3 operator - ()					 const 	{return DRVector3(-x, -y, -z);}
    inline DRVector3 operator * (const DRVector3& b) const 	{return DRVector3(x * b.x, y * b.y, z * b.z);}
    inline DRVector3 operator * (const DRReal f)     const	{return DRVector3(x * f, y * f, z * f);}
    inline DRVector3 operator / (const DRVector3& b) const	{return DRVector3(x / b.x, y / b.y, z / b.z);}
    inline DRVector3 operator / (const DRReal f)	 const	{return DRVector3(x / f, y / f, z / f);}

    //----------------------------------------------------------------------------------------------------------------------

    // Vergleichsoperatoren
    inline bool operator == (const DRVector3& b) const {if(x != b.x) return false; if(y != b.y) return false; return z == b.z;}
    inline bool operator != (const DRVector3& b) const {if(x != b.x) return true; if(y != b.y) return true; return z != b.z;}

    //----------------------------------------------------------------------------------------------------------------------
    // Funktionen deklarieren
    inline DRReal		length()										        const {return sqrtf(x*x + y*y + z*z);}
    inline DRReal		lengthSq()  											const {return x*x + y*y + z*z;}
    inline DRVector3	normalize()												const {return *this / length();}
    inline DRVector3	normalizeEx()											const {return *this / (length() + 0.0001f);}
    inline DRVector3	cross(const DRVector3& v2)							    const {return DRVector3(y * v2.z - z * v2.y, z * v2.x - x * v2.z, x * v2.y - y * v2.x);}
    inline DRReal		dot(const DRVector3& v2)								const {return x * v2.x + y * v2.y + z * v2.z;}
    inline DRReal		angle(const DRVector3& v2)							    const {return acosf((x * v2.x + y * v2.y + z * v2.z) / length() * v2.length());}
    inline DRVector3	interpolateCoords(const DRVector3& v2, const DRReal p)  const {return *this + DRVector3((v2 - *this) * p);}
    inline DRVector3	interpolateNormal(const DRVector3& v2, const DRReal p)	const {return interpolateCoords(v2, p).normalize();}
 //   inline DRVector3	min(const DRVector3& v2)								const {return DRVector3(DR_MIN(x, v2.x), DR_MIN(y, v2.y), DR_MIN(z, v2.z));}
 //   inline DRVector3	max(const DRVector3& v2)								const {return DRVector3(DR_MAX(x, v2.x), DR_MAX(y, v2.y), DR_MAX(z, v2.z));}
    //inline DRVector3	DVector3fRandom()																	{return DRVector3NormalizeEx(DRVector3(tbdoubleRandom(-1.0f, 1.0f), tbdoubleRandom(-1.0f, 1.0f), tbdoubleRandom(-1.0f, 1.0f)));}
    //! Achtung, moeglicherweise nicht sehr performant (wegen powf)
    //inline DRReal		distanceToPoint(const DRVector3& v2)					{return sqrtf(powf(fabs(x - v2.x), 2.0f) + powf(fabs(y - v2.y), 2.0f) + powf(fabs(z - v2.z), 2.0f));}
    inline DRReal       distanceToPoint(const DRVector3& v2)                    {return DRVector3(fabs(x-v2.x), fabs(y-v2.y), fabs(z-v2.z)).length();}
    //*/



};


inline DRVector3 operator * (const DRReal f, const DRVector3& v)			{return DRVector3(v.x * f, v.y * f, v.z * f);}

#endif //__DR_VECTOR3__
