#ifndef __NETWORK_H
#define __NETWORK_H

#include <SDL/SDL_thread.h>
//#include "NetworkMain.h"
#include <list>
#include <vector>
#include "Request.h"
#include "Connection.h"
#include "NetCallback.h"

class NETWORK_API Network : public DRINetwork
{
    public:
        Network();
        virtual ~Network();

        static int ConnectThread(void* data);
        static int ServerThread(void* data);

        // zugriff/arbeits funktionen
        //! \brief baut eine verbindung auf
        //! \param host_ip histname oder ip adresse (durch . getrennnte ziffern)
        //! \param port port number, 80 für http
        //! \return verbindungs ID oder null bei Fehler
        int connect(const char* host_ip, int port);

        //! \brief to make a HTTP Request at the target host
        //! \param url complete url
        //! \param request typ, POST oder GET
        //! \param parameter, string containing parameter, will be added at request
        //! \return dataIndex zum empfangen der antwort oder 0 bei Fehler
        int HTTPRequest(const char* url, DRNet_RequestTyp typ, const std::string& parameter, const char* userAgent = "Freies Leben");

        //! \brief zum erhalen der antworten/ read answears, kehrt sofort zurück, recv
        //! \param dataIndex die nummer des datenaustausches
        //! \param buffer buffer des aufrufers zum erhalten der daten
        //! \param bufferSize die Größe des Buffers des Aufrufers
        DRNet_Status getData(int dataIndex, void* buffer, int bufferSize);

        virtual DRReturn init();
        virtual void exit();

    protected:
        __inline__ int getNetCallbackID() {return ++mNetCallbackID;}
        int intoConnectList(Connection* con);

    private:
        SDL_Thread* mConnectionThread;
        SDL_Thread* mServerThread;

        SDL_cond*   mNewConnectionCondition;
        SDL_cond*   mNewServerCondition;

        bool        mThreadRunning; //true while running

        SDL_mutex*  mConnectionWorkingMutex;
        SDL_mutex*  mNetCallbackMutex;
        SDL_mutex*  mServerWorkingMutex;

        std::list<Connection*> mConnectionList;
        DRHashList mNetCallbackList;

        int mNetCallbackID;
};

#endif // __NETWORK_H
