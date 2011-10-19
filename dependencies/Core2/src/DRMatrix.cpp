#include "Core2Main.h"

DRMatrix DRMatrix::translation(const DRVector3& v)
{
	return DRMatrix(1.0f, 0.0f, 0.0f, v.x,
		            0.0f, 1.0f, 0.0f, v.y,
					0.0f, 0.0f, 1.0f, v.z,
					0.0f, 0.0f, 0.0f, 1.0f);

    DRMatrix m = identity();
    m(0,3) = v.x;
    m(1,3) = v.y;
    m(2,3) = v.z;
    
    return m;
}
DRMatrix DRMatrix::rotationX(const float f)
{
	return DRMatrix(1.0f, 0.0f,     0.0f,    0.0f,
					0.0f, cosf(f), -sinf(f), 0.0f,
					0.0f, sinf(f),  cosf(f), 0.0f,
					0.0f, 0.0f,     0.0f,    1.0f);

    DRMatrix m = identity();
    m(1,1) =   cosf(f); m(1,2) = sinf(f);
    m(2,1) =  -sinf(f); m(2,2) = cosf(f);
    return m;
}
DRMatrix DRMatrix::rotationY(const float f)
{
	return DRMatrix(cosf(f), 0.0f, sinf(f), 0.0f,
					0.0f,    1.0f, 0.0f,    0.0f,
				   -sinf(f), 0.0f, cosf(f), 0.0f,
					0.0f,    0.0f, 0.0f,    1.0f);

    DRMatrix m = identity();
    m(0,0) =  cosf(f); m(0,2) = -sinf(f);
    m(2,0) =  sinf(f); m(2,2) =  cosf(f);
    return m;
}
DRMatrix DRMatrix::rotationZ(const float f)
{
	return DRMatrix(cosf(f), -sinf(f), 0.0f, 0.0f,
					sinf(f),  cosf(f), 0.0f, 0.0f, 
					0.0f,     0.0f,    1.0f, 0.0f,
					0.0f,     0.0f,    0.0f, 1.0f);
    DRMatrix m = identity();
    m(0,0) =  cosf(f);  m(0,1) = sinf(f);
    m(1,0) = -sinf(f);  m(1,1) = cosf(f);
    return m;
}
DRMatrix DRMatrix::rotationAxis(const DRVector3& v, const float f)
{
	DRVector3 axis = v;
	if(v.lengthSq() != 1.0)
	{
		axis = v.normalize();
		//LOG_WARNING("parameter v isn't normalized!");
		printf("\r [DRMatrix::rotationAxis] parameter v isn't normalized!");
	}
	const float _cos = cosf(f);
	const float _sin = sinf(f);
    const float oneMinusCos = 1.0f - _cos;

	return DRMatrix(axis.x*axis.x * oneMinusCos + _cos,
					axis.x*axis.y * oneMinusCos - axis.z * _sin,
					axis.x*axis.z * oneMinusCos + axis.y * _sin, 
					0.0f,
					axis.y*axis.x * oneMinusCos + axis.z * _sin,
					axis.y*axis.y * oneMinusCos + _cos,
					axis.y*axis.z * oneMinusCos - axis.x * _sin,
					0.0f,
					axis.x*axis.z * oneMinusCos - axis.y * _sin,
					axis.y*axis.z * oneMinusCos + axis.x * _sin,
					axis.z*axis.z * oneMinusCos + _cos,
					0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);

	// Sinus und Kosinus berechnen
	const float fSin = sinf(-f);
	const float fCos = cosf(-f);
	const float fOneMinusCos = 1.0f - fCos;

	// Achsenvektor normalisieren
//	const DRVector3 axis(v.normalize() );

	// DRMatrix erstellen
	DRMatrix m = identity();
	m(0,0) = (axis.x*axis.x) * fOneMinusCos + fCos;
	m(0,1) = (axis.x*axis.y) * fOneMinusCos - (axis.z*fSin);
	m(0,2) = (axis.x*axis.z) * fOneMinusCos + (axis.y*fSin);

	m(1,0) = (axis.y*axis.x) * fOneMinusCos + (axis.z*fSin);
	m(1,1) = (axis.y*axis.y) * fOneMinusCos + fCos;
	m(1,2) = (axis.y*axis.z) * fOneMinusCos - (axis.x*fSin);

	m(2,0) = (axis.z*axis.x) * fOneMinusCos - (axis.y*fSin);
	m(2,1) = (axis.z*axis.y) * fOneMinusCos + (axis.x*fSin);
	m(2,2) = (axis.z*axis.z) * fOneMinusCos + fCos;

    return m;
}
DRMatrix DRMatrix::rotation(const DRVector3& v)
{
    //return rotationZ(v.z) * rotationY(v.y) * rotationX(v.x);
	return rotationX(v.x) * rotationY(v.y) * rotationZ(v.z);
}
DRMatrix DRMatrix::scaling(const DRVector3& v)
{
	return DRMatrix(v.x, 0.0f, 0.0f, 0.0f,
					0.0f, v.y, 0.0f, 0.0f,
					0.0f, 0.0f, v.z, 0.0f, 
					0.0f, 0.0f, 0.0f, 1.0f);
}
DRMatrix DRMatrix::transpose() const
{
    return DRMatrix(m[0][0], m[1][0], m[2][0], m[3][0],
                    m[0][1], m[1][1], m[2][1], m[3][1],
                    m[0][2], m[1][2], m[2][2], m[3][2],
                    m[0][3], m[1][3], m[2][3], m[3][3]);
}

DRMatrix DRMatrix::invert() const
{
    // Kehrwert der Determinante vorberechnen
	float invDet = det();
	if(invDet == 0.0f) return identity();
	invDet = 1.0f / invDet;

	// Invertierte DRMatrix berechnen
	DRMatrix result;
	result.m[0][0] =  invDet * (m[1][1] * m[2][2] - m[1][2] * m[2][1]);
	result.m[0][1] = -invDet * (m[0][1] * m[2][2] - m[0][2] * m[2][1]);
	result.m[0][2] =  invDet * (m[0][1] * m[1][2] - m[0][2] * m[1][1]);
	result.m[0][3] = 0.0f;

	result.m[1][0] = -invDet * (m[1][0] * m[2][2] - m[1][2] * m[2][0]);
	result.m[1][1] =  invDet * (m[0][0] * m[2][2] - m[0][2] * m[2][0]);
	result.m[1][2] = -invDet * (m[0][0] * m[1][2] - m[0][2] * m[1][0]);
	result.m[1][3] = 0.0f;

	result.m[2][0] =  invDet * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
	result.m[2][1] = -invDet * (m[0][0] * m[2][1] - m[0][1] * m[2][0]);
	result.m[2][2] =  invDet * (m[0][0] * m[1][1] - m[0][1] * m[1][0]);
	result.m[2][3] = 0.0f;

	result.m[3][0] = -(m[3][0] * result.m[0][0] + m[3][1] * result.m[1][0] + m[3][2] * result.m[2][0]);
	result.m[3][1] = -(m[3][0] * result.m[0][1] + m[3][1] * result.m[1][1] + m[3][2] * result.m[2][1]);
	result.m[3][2] = -(m[3][0] * result.m[0][2] + m[3][1] * result.m[1][2] + m[3][2] * result.m[2][2]);
	result.m[3][3] = 1.0f;

	return result;
}

float DRMatrix::det() const
{
// Determinante der linken oberen 3x3-Teilmatrix berechnen
	return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
           m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
           m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

DRMatrix DRMatrix::axis(const DRVector3& x_axis, const DRVector3& y_axis, const DRVector3& z_axis)
{
    return DRMatrix(x_axis.x, x_axis.y, x_axis.z, 0.0f,
                    y_axis.x, y_axis.y, y_axis.z, 0.0f,
                    z_axis.x, z_axis.y, z_axis.z, 0.0f,
                    0.0f,     0.0,      0.0f,     1.0f);
}

DRMatrix DRMatrix::view_frustum(const float angle_of_view, const float aspect_ratio,
								const float z_near, const float z_far)
{
	float focalLength = 1 / tanf(angle_of_view/2.0f);

	return DRMatrix(focalLength/aspect_ratio, 0.0f,		   0.0f,					      0.0f,
					0.0f,					  focalLength, 0.0f,						  0.0f,
					0.0f,					  0.0f,		   (z_far+z_near)/(z_near-z_far), (2*z_far*z_near)/(z_near-z_far),
					0.0f,					  0.0f,		   -1.0f,						  0.0f);

	DRMatrix mat(DRMatrix::identity());	
	mat(0,0) = focalLength/aspect_ratio;
	mat(1,1) = focalLength;
	mat(2,2) = (z_far+z_near)/(z_near-z_far);
	mat(3,2) = (2*z_far*z_near)/(z_near-z_far);
	mat(2,3) = -1.0f;
	mat(3,3) = 0.0f;
	return mat;
	
}