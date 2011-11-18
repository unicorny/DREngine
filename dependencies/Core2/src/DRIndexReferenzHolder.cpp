#include "Core2Main.h"


DRIndexReferenzHolder::DRIndexReferenzHolder(uint maxIndexCount)
: mReferenzCounter(NULL), mFreePlaces(NULL), mFreePlaceCursor(0), mMaxIndexCount(maxIndexCount)
{
    mReferenzCounter = new uint[maxIndexCount];
    memset(mReferenzCounter, 0, sizeof(uint)*maxIndexCount);
    mFreePlaces = new uint[maxIndexCount];
    if(!mReferenzCounter || !mFreePlaces) LOG_ERROR_VOID("no memory");

    uint index = maxIndexCount-1;
    //Die Zeiger in m_ppFreeObjects werden von vorne nach hinten,
    //von hinten nach vorne auf den reservierten Speicher ausgerichtet
    for(mFreePlaceCursor = 0; mFreePlaceCursor < maxIndexCount; mFreePlaceCursor++)
    {
        mFreePlaces[mFreePlaceCursor] = index--;
    }
}

DRIndexReferenzHolder::~DRIndexReferenzHolder()
{
    DR_SAVE_DELETE_ARRAY(mReferenzCounter);
    DR_SAVE_DELETE_ARRAY(mFreePlaces);
}

void DRIndexReferenzHolder::add(uint index)
{
    if(index >= mMaxIndexCount) return;
    mReferenzCounter[index]++;
}

void DRIndexReferenzHolder::remove(uint index)
{
    mReferenzCounter[index]--;
    if(mReferenzCounter[index] <= 0)
        mFreePlaces[mFreePlaceCursor++] = index;
}

uint DRIndexReferenzHolder::getFree()
{
    if(mFreePlaceCursor <= 0)
    {
        LOG_ERROR("Kein Platz mehr in der Liste!", 0);
    }
    --mFreePlaceCursor;
    mReferenzCounter[mFreePlaces[mFreePlaceCursor]] = 1;
    return mFreePlaces[mFreePlaceCursor];
}