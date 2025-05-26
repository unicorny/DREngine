#ifndef __NET_NET_CALLBACK__
#define __NET_NET_CALLBACK__

#include "Connection.h"
class NetCallback
{
public:
    NetCallback(Connection* parent);

    __inline__ void setParent(Connection* parent) {mParent = parent;};

    __inline__ Connection* getParent() {return mParent;}

    //! return true wenn nochmal gesendet/empfangen werden soll
    //! return false wenn alls okay
    bool setStatus(DRNet_Status state);

protected:

private:

    DRNet_Status mCurrentState;
    Connection* mParent;
    DRNet_Status (*finishedCallback)(DRNet_Status);

};

#endif //__NET_NET_CALLBACK__
