#include "Connection.h"
//#include "DRINetwork.h"

using namespace std;

Connection::Connection(const char* url_host, int port)
: mURL(""), mHOST(""), mSocket(NULL), mCallbackIndex(0)
{
    //ctor
    mIP.port = port;
    splitURL_HOST(url_host);
}

Connection::~Connection()
{
    //dtor
    if(mSocket)
        SDLNet_TCP_Close(mSocket);
}

TCPsocket Connection::getSocket()
{
    if(!mSocket)
    {
        if(getIP()) LOG_ERROR("ungültige IP", NULL);
        mSocket = SDLNet_TCP_Open(&mIP);
        LOG_ERROR_SDL(NULL);
    }

    return mSocket;
}

DRReturn Connection::getIP()
{
    DRLog.writeToLog("host: %d, port: %d, host: %s", mIP.host, mIP.port, mHOST.data());
    int ret = SDLNet_ResolveHost(&mIP, mHOST.data(), mIP.port);
    DRLog.writeToLog("resolve Host: %d, host: %d, port: %d", ret, mIP.host, mIP.port);
    if(ret == 0) return DR_OK;

    //host could't be resolve, we look if we have get a ip adresse

    unsigned char count = 0;
    unsigned int host = 0;
    std::string tempStr;
    for(unsigned int i = 0; i < mHOST.length(); i++)
    {
        char c = mHOST[i];
        if(c != '.')
        {
            tempStr += c;
        }
        else
        {
            //mIP.host
            int temp = atoi(tempStr.data());
            host |= (temp << count*4);
            count++;
            if(count > 3) LOG_ERROR("ip string ist ungültig", DR_ERROR);
        }
    }
    int temp = atoi(tempStr.data());
    host |= temp << 3*4;
    SDLNet_Write32(host, &mIP.host);

    return DR_OK;
}

void Connection::splitURL_HOST(const char* url_host)
{
    string temp = url_host;
    string::size_type first = temp.find("\\");
    if(first == temp.npos)
    {
        first = temp.find("/");
        if(first == temp.npos)
        {
            mHOST = temp;
            mURL = "";
            return;
        }
    }
    mHOST = temp.substr(0, first);
    mURL = temp.substr(first, temp.npos);

    DRLog.writeToLog("Host: %s, URL: %s", mHOST.data(), mURL.data());
}

DRNet_Status Connection::send(const void* data, int length)
{
    LOG_INFO("send");
    if(!getSocket()) return NET_SOCKET_ERROR;

    int sended = SDLNet_TCP_Send(mSocket, data, length);
    DRLog.writeToLog("%d bytes gesendet", sended);

    /* temp */
    char buffer[2084];
    memset(buffer, 0, 2084);
    int ret = 0;
    int readedBytes = 0;

    FILE* out = fopen("out.html", "wt");
    while(ret = SDLNet_TCP_Recv(mSocket, &buffer[readedBytes], 2048-readedBytes))
    {
        readedBytes += ret;
        if(readedBytes >= 2048)
        {
            LOG_WARNING("buffer ist zu klein für empfangene Daten");
            //memset(buffer, 0, 2084);
            DRLog.writeToLog("ret: %d, readedBytes: %d", ret, readedBytes);
            fwrite(buffer, 1, 2048, out);
            readedBytes = ret = 0;
            //DRLog.writeToLogDirect("\n\nbuffer: %s", buffer);

            //break;
        }
    }
    fclose(out);
    LOG_WARNING_SDL();
    DRLog.writeToLog("buffer: <pre>%s</pre>", buffer);
    /* */

    if(sended == length) return NET_COMPLETE;

    LOG_ERROR("nicht alles gesendet", NET_NOT_COMPLETE);
}
