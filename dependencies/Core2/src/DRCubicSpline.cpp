#include "Core2Main.h"

DRVector3 DRCubicSpline::getPoint(float fTime)
{
	if(fTime < 0.0f) fTime *= -1.0f;
	if(fTime > 1.0f) fTime = fabs(fTime -(float)(int)fTime);
	DRReal vecto[] = {fTime*fTime*fTime, fTime*fTime};
	DRReal erg[4];
	erg[0] = 2.0f*vecto[0]-3.0f*vecto[1]+1.0f;
	erg[1] = -2.0f*vecto[0]+3*vecto[1];
	erg[2] = vecto[0]-2*vecto[1]+fTime;
	erg[3] = vecto[0]-vecto[1];

	DRVector3 vPos;
	for (int i = 0; i < 3; i++)
	{
		vPos.c[i] = erg[0]*startPos.c[i]+erg[1]*endPos.c[i]+erg[2]*startVector.c[i]+erg[3]*endVector.c[i];
	}
	return vPos;
}
//-----------------------------------------------------------------------------------------------------------------
DRVector3 DRCubicSpline::getPoint(float fTime, int rekursion)
{
	if(rekursion <= 0) return getPoint(fTime);
	// rekursion: 1, segmente: 2
	// 2, 4
	// 3, 8
	// 2^rekursion = segmente
	int numSegments = (int)powf(2.0f, (float)rekursion);
	int index = (int)(fTime * (float)numSegments);
	if(index >= numSegments) index = numSegments - 1;
	DRVector3 startP = getPoint((float)index/(float)numSegments);
	DRVector3 endP =   getPoint((float)(index + 1)/(float)numSegments);

	float newTime = (fTime * (float)numSegments) - index;
	DRCubicSpline spline(startP, endP, startVector, endVector);

	return spline.getPoint(newTime);
	
}
//*****************************************************************************************************************
//*****************************************************************************************************************
DRReturn DRRNS::addNode(DRVector3 _position)
{
	mBuildSplineCalled = false;
	int size = mNodes.size()-1;
	mNodes.push_back(Node(_position));
#ifdef _DEBUG
	if(size +1 == mNodes.size())
		LOG_ERROR("Fehler, node vector ist nicht größer geworden!", DR_ERROR);
	for(int i = 0; i < size; i++)
	{
		if(_position == mNodes[i].position)
		{
			DRLog.WriteVector3ToLog(_position);
			mNodes.pop_back();
			size--;
			LOG_ERROR("Der Neue Punkt ist schon im Array vorhanden", DR_ERROR);
		}
	}
#endif
	if(size >= 0)
	{
		mNodes[size].distance = DRVector3(mNodes[size].position - _position).length();
		mMaxDistance += mNodes[size].distance;
	}

	return DR_OK;
}
//*****************************************************************************************************************
void DRRNS::buildSpline()
{
	int nodeCount = mNodes.size();
	if(nodeCount == 0) return;
	for(int i = 1; i < nodeCount-1; i++)
	{
		// split the angle
		mNodes[i].velocity = DRVector3(mNodes[i+1].position - mNodes[i].position).normalize() -	
							 DRVector3(mNodes[i-1].position - mNodes[i].position).normalize();
		mNodes[i].velocity = DRVector3(mNodes[i].velocity).normalize();
	}

	// calculate start and end velocities
	mNodes[0].velocity = getStartVelocity(0);
	mNodes[nodeCount-1].velocity = getEndVelocity(nodeCount-1);

	mBuildSplineCalled = true;
}
//-----------------------------------------------------------------------------------------------------------------
DRVector3 DRRNS::getStartVelocity(int index)
{
	DRVector3 temp = 3.0f * (mNodes[index+1].position - mNodes[index].position)/mNodes[index].distance;
	return (temp - mNodes[index+1].velocity)*0.5f;
}
//-----------------------------------------------------------------------------------------------------------------
DRVector3 DRRNS::getEndVelocity(int index)
{
	DRVector3 temp = 3.f*(mNodes[index].position - mNodes[index-1].position)/mNodes[index-1].distance;
	return (temp - mNodes[index-1].velocity)*0.5f;
}
//*****************************************************************************************************************
DRVector3 DRRNS::getPoint(float fTime, int rekursion)
{
	if(fTime < 0.0f) fTime *= -1.0f;
	if(fTime > 1.0f) fTime = fabs(fTime -(float)(int)fTime);

	if(!mBuildSplineCalled) buildSpline();
	if(mNodes.size() == 0) return DRVector3(0.0f);
	float distance = fTime * mMaxDistance;
	float currentDistance = 0.f;
	int i = 0;
	while (currentDistance + mNodes[i].distance < distance
	    && i < mNodes.size())
	{
	    currentDistance += mNodes[i].distance;
	    i++;
	}
	float t = (distance - currentDistance) / mNodes[i].distance;
	//t /= mNodes[i].distance; // scale t in range 0 - 1
	DRCubicSpline spline(mNodes[i].position, mNodes[i+1].position,
						 mNodes[i].velocity * mNodes[i].distance,
						 mNodes[i+1].velocity * mNodes[i].distance);
	return spline.getPoint(t, rekursion);
}