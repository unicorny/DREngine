#ifndef __REQUEST_H
#define __REQUEST_H

#include <Engine2Main.h>
#include <SDLNet/SDL_net.h>
#include "Connection.h"

class Request : public Connection
{
public:
    Request(const char* url_host, DRNet_RequestTyp typ, const std::string& parameter, const char* userAgent);

    virtual ~Request();

    virtual DRNet_Status send(const void* data, int length);
protected:

    std::string makeToString(std::map<std::string, std::string>* parameter);

private:
    std::string mParameter;
    std::string mUserAgent;
    DRNet_RequestTyp    mTyp;

};

#endif // REQUEST_H
