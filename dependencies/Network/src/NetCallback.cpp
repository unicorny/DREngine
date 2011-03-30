#include "NetCallback.h"

NetCallback::NetCallback(Connection* parent)
: mCurrentState(NET_NOT_READY), mParent(parent), finishedCallback(NULL)
{}

bool NetCallback::setStatus(DRNet_Status state)
{
    if(state == NET_COMPLETE && finishedCallback)
    {
        if(finishedCallback(state) == NET_DATA_CORRUPTED)
        {
            LOG_ERROR("NET Data Corrupted", true);
        }
    }

     mCurrentState = state;
     return false;
}
