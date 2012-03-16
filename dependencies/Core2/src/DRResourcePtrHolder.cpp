#include "Core2Main.h"

DRResourcePtrHolder::DRResourcePtrHolder(DRIResource* resource)
: mResource(resource), mRefCount(0)
{
}

DRResourcePtrHolder::~DRResourcePtrHolder() 
{ 
    DR_SAVE_DELETE(mResource);
}


int DRResourcePtrHolder::getRefCount () const
{
	return mRefCount;
}


int DRResourcePtrHolder::addRef ()
{
	mRefCount++;
	return mRefCount;
}


int DRResourcePtrHolder::release ()
{
	int tmpRefCount;

	mRefCount--;
	tmpRefCount = mRefCount;

	if ( mRefCount <= 0 )
		delete this;

	return mRefCount;
}

