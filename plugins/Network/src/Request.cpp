#include "Request.h"

using namespace std;

Request::Request(const char* url_host, DRNet_RequestTyp typ, const string& parameter, const char* userAgent)
: Connection(url_host, 80), mParameter(parameter), mTyp(typ), mUserAgent(userAgent)
{

}



Request::~Request()
{
    //dtor
}

std::string Request::makeToString(map<string, string>* parameter)
{
    string text;
    for(map<string, string>::iterator it = parameter->begin(); it != parameter->end(); it++)
    {
        text += it->first + string("=") + it->second;
        if(it != parameter->end())
            text += string("&");
    }
	return text;
}

DRNet_Status Request::send(const void* data, int length)
{
    const char* text = (const char*)data;
    string request;

    // typ, POST or GET
    if(mTyp == NET_GET)         request = "GET ";
    else if(mTyp == NET_POST)   request = "POST ";
    else LOG_ERROR("ungueltiger Typ", NET_ERROR);

    // url, adresse der genauen seite
    request += mURL;

    if(text && text[length-1] != '\0' || !text)
    {
       LOG_WARNING("data ungueltig");
    }
    else
    {
       mParameter += "&" + string(text);
    }

    // parameter anhängen bei GET
    if(mTyp == NET_GET)
        request += "?" + mParameter;
    //  protocol and version
    request += " HTTP/1.1\r\n";
    // user-agent (also der browser)
    request += "User-Agent: " + mUserAgent + "\r\n";
    // Host
    request += "Host: " + mHOST + "\r\n";

    request += "Accept: */*\r\n";
    request += "Accept-Encoding: text/html\r\n";
    if(mTyp == NET_POST)
    {
        char temp[256];
        sprintf(temp, "Content-Length: %d\r\n", mParameter.length());
        request += string(temp);
        request += "Content-Type: application/x-www-form-urlencoded\r\n\r\n";
        request += mParameter + string("\r\n");
    }
    request += "\r\n";
    DRLog.writeToLog("request: %s\nsize: %d\n\n", request.data(), request.length());

    return Connection::send(request.data(), request.length());
}

