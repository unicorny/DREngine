#include "Core2Main.h"
/*
DRSpring::DRSpring()
: mVar(0.0f), mTime(0.0f), mTimeTarget(0.0f)
{
    
}

void DRSpring::setVar(DRReal value, DRReal time)
{
    mTime = 0.0f;
    mTarget = value;
    mDelta = mTarget - mVar;
    mTimeTarget = time;
}

void DRSpring::update(DRReal deltaTime)
{
    mTime += deltaTime;
    if(hasReachedTarget()) mVar = mTarget;
    else mVar += (deltaTime/mTimeTarget) * mDelta;
}

DRReal DRSpring::operator = (DRReal value)
{
    mVar		= value;
    mTarget		= value;
    mTime		= 0.0f;
    mTimeTarget         = 0.0f;
    return mVar;
}

bool DRSpring::hasReachedTarget() const
{
    return mTime >= mTimeTarget;
}
 //*/

DRDampingSpring::DRDampingSpring(DRReal damping, DRReal spring)
: mDamping(damping), mSpring(spring), // set the constants
  mTarget(0.0f), mVelocity(0.0f), mCurrent(0.0f)
{
}

DRDampingSpring::DRDampingSpring()
: mDamping(1), mSpring(1), // set constants bth to 1
mTarget(0.0f), mVelocity(0.0f), mCurrent(0.0f)
{

}

void DRDampingSpring::setParameter(DRReal damping, DRReal spring)
{
    //copy parameters
    mDamping = damping;
    mSpring  = spring;
}

void DRDampingSpring::update(DRReal timeSinceLastFrame)
{
    // vector between current and target
    DRReal displace = mCurrent - mTarget;

    // spring acceleration to come to target
    // F = ma = -ksx-kdv // force = -spring*vector - damping*velocity
    DRReal springAccel = (-mSpring * displace) - (mDamping * mVelocity);

    // update current and velocity with euler-integration
    mVelocity += springAccel * timeSinceLastFrame;
    mCurrent  += mVelocity   * timeSinceLastFrame;
}

