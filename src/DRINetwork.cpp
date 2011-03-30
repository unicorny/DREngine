#include "Engine2Main.h"

DRINetwork* DRINetwork::mTheOneAndOnly = NULL;


DRINetwork::DRINetwork()
: DRInterface(), mInitalized(false)
{
    //ctor
}

DRINetwork::~DRINetwork()
{
    close();
}

DRReturn DRINetwork::init()
{
    if(mInitalized) return DR_OK;

    LOG_INFO("INetwork init");
    mInitalized = true;

    return DR_OK;
}

void DRINetwork::exit()
{
    if(!mInitalized) return;

    LOG_INFO("INetwork exit");
    mInitalized = false;
}


DRINetwork* DRINetwork::Instance()
{
    if(!mTheOneAndOnly)
    {
        mTheOneAndOnly = (DRINetwork*)DRInterface::Instance(__NETWORK_DLL_NAME_);
        if(!mTheOneAndOnly) mTheOneAndOnly = new DRINetwork();
        return mTheOneAndOnly;
    }
    else
    {
        return mTheOneAndOnly;
    }
    return NULL;
}

int DRINetwork::connect(const char* host_ip, int port)
{
    LOG_WARNING("not implemented");
    return 0;
}

int DRINetwork::HTTPRequest(const char* url, DRNet_RequestTyp typ, const std::string& parameter, const char* userAgent /* = "Freies Leben"*/)
{
    LOG_WARNING("not implemented");
    return 0;
}


DRNet_Status DRINetwork::getData(int dataIndex, void* buffer, int bufferSize)
{
    LOG_WARNING("not implemented");
    return NET_ERROR;
}

