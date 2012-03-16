#include "Core2Main.h"


DRObjekt::DRObjekt(const DRVector3& position)
: mPosition(position),
 mXAxis(1.0f, 0.0f, 0.0f), mYAxis(0.0f, 1.0f, 0.0f), mZAxis(0.0f, 0.0f, 1.0f)
{
    update();
}

DRObjekt::DRObjekt()
: mPosition(DRVector3(0.0f)), 
 mXAxis(1.0f, 0.0f, 0.0f), mYAxis(0.0f, 1.0f, 0.0f), mZAxis(0.0f, 0.0f, 1.0f)
 {
     update();
 }

void DRObjekt::translateAbs(const DRVector3& translate)
{
    mPosition += translate;
    update();
}
void DRObjekt::translateRel(const DRVector3& translate)
{
    mPosition += translate.transformNormal(mMatrix);
    update();
}

void DRObjekt::rotateRel(const DRVector3& rotation)
{
	DRMatrix mRot;

	// Rotation um die z-Achse des Objekts
	mRot = DRMatrix::rotationAxis(mZAxis, rotation.z);
	mXAxis = mXAxis.transformNormal(mRot);
	mYAxis = mYAxis.transformNormal(mRot);

	// Rotation um die y-Achse des Objekts
        mRot = DRMatrix::rotationAxis(mYAxis, rotation.y);
	mXAxis = mXAxis.transformNormal(mRot);
	mZAxis = mXAxis.cross(mYAxis);

	// Rotation um die x-Achse des Objekts
	mRot = DRMatrix::rotationAxis(mXAxis, rotation.x);
	mYAxis = mYAxis.transformNormal(mRot);
	mZAxis = mXAxis.cross(mYAxis);

	// Matrizen aktualisieren
	update();
}

void DRObjekt::rotateAbs(const DRVector3& rotation)
{
    // Rotation um die x-Achse
    DRMatrix mRotation(DRMatrix::rotationX(rotation.x));
    mYAxis = mYAxis.transformNormal(mRotation);
    mZAxis = mXAxis.cross(mYAxis);

    // Rotation um die y-Achse
    mRotation = DRMatrix::rotationY(rotation.y);
    mXAxis = mXAxis.transformNormal(mRotation);
    mZAxis = mXAxis.cross(mYAxis);

    // Rotation um die z-Achse
    mRotation = DRMatrix::rotationZ(rotation.z);
    mXAxis = mXAxis.transformNormal(mRotation);
    mYAxis = mYAxis.transformNormal(mRotation);

    // Matrizen aktualisieren
    update();
}

void DRObjekt::update()
{
    DRMatrix m1 = DRMatrix::axis(mXAxis, mYAxis, mZAxis);
    DRMatrix m2 = DRMatrix::translation(mPosition);
    //mMatrix = DRMatrix(m1) * DRMatrix(m2);
    mMatrix = DRMatrix(m2) * DRMatrix(m1);
   // mMatrix.print();
}

void DRObjekt::lookAt(DRVector3 targetPosition, DRVector3 upVector/* = DRVector3(0.0f, 1.0f, 0.0f)*/)
{
    mZAxis = DRVector3(mPosition - targetPosition).normalizeEx();
    mXAxis = upVector.cross(mZAxis).normalizeEx();
    mYAxis = mZAxis.cross(mXAxis).normalizeEx();
    update();
}