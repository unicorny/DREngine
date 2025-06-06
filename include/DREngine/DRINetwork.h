/*/*************************************************************************
 *                                                                         *
 * DREngine, Engine for my programs, using SDL and OpenGL                 *
 * Copyright (C) 2012, 2013, 2014 github.com/unicorny                      *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * any later version.                                                      *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 *                                                                         *
 ***************************************************************************/

#ifndef __DR_ENGINE2_INETWORK__
#define __DR_ENGINE2_INETWORK__
#ifdef _WIN32
#define __NETWORK_DLL_NAME_ "libNetwork.dll"
#else
#define __NETWORK_DLL_NAME_ "libNetwork.so"
#endif
/*
Netzwerk klasse fuer das einfache arbeiten mit sockets
Autor: Dario Rekowski
Datum: 16.09.09
*/
#include <map>
#include <string>

#include "DREngine/DRInterface.h"

enum DRNet_Status
{
    NET_NOT_READY = 1, // not ready to read or write
    NET_NOT_COMPLETE = 2, // not all writed/readed
    NET_COMPLETE = 4, // all writetd/readed
    NET_DATA_CORRUPTED = 8, // datas defekt
    NET_SOCKET_ERROR = 16, // error by creating socket
    NET_ERROR    = -1 // Fehler
};

#if (_MSC_VER >= 1200 && _MSC_VER < 1310)
enum ENGINE_API DRNet_Status;
#endif

enum DRNet_RequestTyp
{
    NET_POST = 1,
    NET_GET = 2
};

#if (_MSC_VER >= 1200 && _MSC_VER < 1310)
enum ENGINE_API DRNet_RequestTyp;
#endif

// Interface klasse fuer Netzwerk, implementierung erfolgt in Network.dll
class DRENGINE_EXPORT DRINetwork : public DRInterface
{
public:
    virtual ~DRINetwork();

    // Zugrifffunktionen
    static DRINetwork* Instance();

    static inline DRINetwork& getSingleton() {return *Instance();}
    static inline DRINetwork* getSingletonPtr() {return Instance();}

    //init und exit
    virtual DRReturn init();
    virtual void exit();

    // zugriff/arbeits funktionen
    //! \brief baut eine verbindung auf
    //! \param host_ip histname oder ip adresse (durch . getrennnte ziffern)
    //! \param port port number, 80 f�r http
    //! \return verbindungs ID oder null bei Fehler
    virtual int connect(const char* host_ip, int port);

    //! \brief to make a HTTP Request at the target host
    //! \param url complete url
    //! \param request typ, POST oder GET
    //! \param parameter, string containing parameter, will be added at request
    //! \return dataIndex zum empfangen der antwort oder 0 bei Fehler
    virtual int HTTPRequest(const char* url, DRNet_RequestTyp typ, const std::string& parameter, const char* userAgent = "Freies Leben");

    //! \brief zum erhalen der antworten/ read answears
    //! \param dataIndex die nummer des datenaustausches
    //! \param buffer buffer des aufrufers zum erhalten der daten
    //! \param bufferSize die Gr��e des Buffers des Aufrufers
    virtual DRNet_Status getData(int dataIndex, void* buffer, int bufferSize);

protected:
    bool mInitalized;
    static DRINetwork* mTheOneAndOnly;

    DRINetwork();

private:
};

#endif //__DR_ENGINE2_INETWORK__
