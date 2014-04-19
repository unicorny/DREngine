#include "NetworkMain.h"


using namespace std;


Network::Network()
: mConnectionThread(NULL), mServerThread(NULL),
  mNewConnectionCondition(NULL), mNewServerCondition(NULL),
  mThreadRunning(true),
  mConnectionWorkingMutex(NULL), mNetCallbackMutex(NULL), mServerWorkingMutex(NULL),
  mNetCallbackID(0)
{
    //ctor
}

Network::~Network()
{
    //dtor
}


DRReturn Network::init()
{
    if(mInitalized) return DR_OK;
    SDLNet_Init();

    mNetCallbackID = 0;

    if(mConnectionThread) LOG_ERROR("mConnectionThread is already active", DR_ERROR);
    if(mServerThread)     LOG_ERROR("mServerThread is already actvie", DR_ERROR);

    mNewConnectionCondition = SDL_CreateCond();  LOG_WARNING_SDL();
    mNewServerCondition     = SDL_CreateCond();  LOG_WARNING_SDL();

    mConnectionWorkingMutex = SDL_CreateMutex(); LOG_WARNING_SDL();
    mNetCallbackMutex       = SDL_CreateMutex(); LOG_WARNING_SDL();
    mServerWorkingMutex     = SDL_CreateMutex(); LOG_WARNING_SDL();
#if SDL_VERSION_ATLEAST(1,3,0)
	mConnectionThread = SDL_CreateThread(ConnectThread, "DRNetcon", this);
	mServerThread     = SDL_CreateThread(ServerThread, "DRNetServ", this);
#else
    mConnectionThread = SDL_CreateThread(ConnectThread, this);
    mServerThread     = SDL_CreateThread(ServerThread, this);
#endif

    LOG_INFO("Network Init");
    mInitalized = true;
    return DR_OK;
}

void Network::exit()
{
    if(!mInitalized) return;

    mThreadRunning = false;
    SDL_Delay(300);
//    SDL_KillThread(mConnectionThread);          LOG_WARNING_SDL();
    //SDL_KillThread(mServerThread);              LOG_WARNING_SDL();
    SDL_WaitThread(mConnectionThread, NULL);
    SDL_WaitThread(mServerThread, NULL);

    SDL_DestroyMutex(mConnectionWorkingMutex);  LOG_WARNING_SDL();
    SDL_DestroyMutex(mNetCallbackMutex);        LOG_WARNING_SDL();
    SDL_DestroyMutex(mServerWorkingMutex);      LOG_WARNING_SDL();

    SDL_DestroyCond(mNewConnectionCondition);   LOG_WARNING_SDL();
    SDL_DestroyCond(mNewServerCondition);       LOG_WARNING_SDL();


    for(int i = 0; i < mNetCallbackList.getNItems(); i++)
    {
        NetCallback* temp = (NetCallback*)mNetCallbackList.findByIndex(i);
        DR_SAVE_DELETE(temp);
    }
    mNetCallbackList.clear(true);

    for(list<Connection*>::iterator it = mConnectionList.begin(); it != mConnectionList.end(); it++)
    {
        DR_SAVE_DELETE(*it);
    }

    SDLNet_Quit();
    LOG_INFO("Network Exit");
}

//************************************************************************************


int Network::ServerThread(void* data)
{
     //Pointer for fast access to singelton class
    Network* N = (Network*)data;

    LOG_INFO("Server Thread gestartet");

    //While exit not set, do loop
    while(N->mThreadRunning)
    {
        // Lock work mutex
        SDL_LockMutex(N->mServerWorkingMutex); LOG_ERROR_SDL(-1);
        //wait if we can make a new stream, 0.25 seconds or,
        //if fading is working, 0.10 seconds (10 fps)
        Uint32 milliSecondsToWait = 250;
        int status = SDL_CondWaitTimeout(N->mNewServerCondition, N->mServerWorkingMutex, milliSecondsToWait); LOG_ERROR_SDL(-1);
        if( status == 0)
        {


            SDL_UnlockMutex(N->mServerWorkingMutex); LOG_ERROR_SDL(-1);
        }
        else if(status == SDL_MUTEX_TIMEDOUT)
        {
            // update
            SDL_UnlockMutex(N->mServerWorkingMutex); LOG_ERROR_SDL(-1);
        }
        else
        {
            //unlock mutex and exit
            SDL_UnlockMutex(N->mServerWorkingMutex); LOG_ERROR_SDL(-1);
            LOG_ERROR("Fehler im Server Thread, exit", -1);
        }
    }
    LOG_ERROR_SDL(-1);
    LOG_INFO("Server Thread beendet");

    return 0;
}

int Network::ConnectThread(void* data)
{
    //Pointer for fast access to singelton class
    Network* N = (Network*)data;

    LOG_INFO("Connect Thread gestartet");

    //While exit not set, do loop
    while(N->mThreadRunning)
    {
        // Lock work mutex
        SDL_LockMutex(N->mConnectionWorkingMutex); LOG_ERROR_SDL(-1);
        //wait if we can make a new stream, 0.25 seconds or,
        //if fading is working, 0.10 seconds (10 fps)
        Uint32 milliSecondsToWait = 250;
        int status = SDL_CondWaitTimeout(N->mNewConnectionCondition, N->mConnectionWorkingMutex, milliSecondsToWait); LOG_ERROR_SDL(-1);
        if( status == 0)
        {
            // new connection
            for(list<Connection*>::iterator it = N->mConnectionList.begin(); it != N->mConnectionList.end(); it++)
            {
                Connection* cn = *it;
                if(cn->send(NULL, 0) == NET_COMPLETE)
                {
                    LOG_INFO("complete");

                    it = N->mConnectionList.erase(it);
                    SDL_UnlockMutex(N->mConnectionWorkingMutex);

                    // work with Net Callback
                    SDL_LockMutex(N->mNetCallbackMutex);
                    NetCallback* temp = (NetCallback*)N->mNetCallbackList.findByHash(cn->getCallbackIndex());
                    bool ret = temp->setStatus(NET_COMPLETE);
                    SDL_UnlockMutex(N->mNetCallbackMutex);

                    SDL_LockMutex(N->mConnectionWorkingMutex);

                    // connection nur löschen wenn daten übertragung erfolgreich
                    if(!ret)
                    {
                        DR_SAVE_DELETE(cn);
                    }
                    // sonst wieder einreihen
                    else
                    {
                        N->mConnectionList.push_back(cn);
                    }

                    //beenden aber anzeigen, das am ende gleich weitergemacht werden kann (hm.. oder pause lassen für neue anfragen?)
                    if(N->mConnectionList.size() > 0)
                    {
                        if(SDL_CondSignal(N->mNewConnectionCondition)== -1) //LOG_ERROR_SDL(DR_ERROR);
                        {
                            LOG_WARNING_SDL();
                            LOG_WARNING("Fehler beim Aufruf von SDL_CondSignal");
                        }
                    }
                    break;
                }
            }

            SDL_UnlockMutex(N->mConnectionWorkingMutex); LOG_ERROR_SDL(-1);
        }
        else if(status == SDL_MUTEX_TIMEDOUT)
        {
            // update
            SDL_UnlockMutex(N->mConnectionWorkingMutex); LOG_ERROR_SDL(-1);
        }
        else
        {
            //unlock mutex and exit
            SDL_UnlockMutex(N->mConnectionWorkingMutex); LOG_ERROR_SDL(-1);
            LOG_ERROR("Fehler im Connect Thread, exit", -1);
        }
    }
    LOG_ERROR_SDL(-1);
    LOG_INFO("Connect Thread beendet");

    return 0;
}

int Network::connect(const char* host_ip, int port)
{
    return intoConnectList(new Connection(host_ip, port));
}

 int Network::HTTPRequest(const char* url, DRNet_RequestTyp typ, const std::string& parameter, const char* userAgent/* = "Freies Leben"*/)
 {
     return intoConnectList((Connection*)new Request(url, typ, parameter, userAgent));
 }

 int Network::intoConnectList(Connection* con)
 {
    if(!mInitalized) return 0;

    SDL_LockMutex(mNetCallbackMutex);
    int hash = getNetCallbackID();
    mNetCallbackList.addByHash(hash, new NetCallback(con));
    con->setCallbackIndex(hash);
    SDL_UnlockMutex(mNetCallbackMutex);

    SDL_LockMutex(mConnectionWorkingMutex); LOG_WARNING_SDL();

    mConnectionList.push_back(con);

    SDL_UnlockMutex(mConnectionWorkingMutex); LOG_WARNING_SDL();

    if(SDL_CondSignal(mNewConnectionCondition)== -1) //LOG_ERROR_SDL(DR_ERROR);
    {
        LOG_WARNING_SDL();
        LOG_WARNING("Fehler beim Aufruf von SDL_CondSignal");
    }
    return hash;
 }

DRNet_Status Network::getData(int dataIndex, void* buffer, int bufferSize)
{
    LOG_WARNING("not implemented");
    return NET_ERROR;
}

