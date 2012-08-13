#include "Engine2Main.h"

DRThread::DRThread(const char* threadName/* = NULL*/)
: mutex(NULL), thread(NULL), condition(NULL), semaphore(NULL)
{
    semaphore = SDL_CreateSemaphore(1); LOG_WARNING_SDL();
    condition = SDL_CreateCond(); LOG_WARNING_SDL();
    SDL_SemWait(semaphore); LOG_WARNING_SDL();
    mutex = SDL_CreateMutex(); LOG_WARNING_SDL();  
    
#if SDL_VERSION_ATLEAST(1,3,0)
    thread = SDL_CreateThread(Thread, threadName, this);
#else
    thread = SDL_CreateThread(Thread, this);
#endif
    
} 

DRThread::~DRThread()
{
    if(thread)
    {
        //Post Exit to Stream Thread
        SDL_SemPost(semaphore); LOG_WARNING_SDL();
        //kill TextureLoad Thread after 1/2 second
        SDL_Delay(500);
        SDL_KillThread(thread);
        LOG_WARNING_SDL();

        thread = NULL;
        SDL_DestroySemaphore(semaphore);
        SDL_DestroyMutex(mutex);
        SDL_DestroyCond(condition);
    }
}

DRReturn DRThread::condSignal()
{
    if(SDL_CondSignal(condition)== -1) //LOG_ERROR_SDL(DR_ERROR);
    {
        LOG_WARNING("Fehler beim Aufruf von SDL_CondSignal"); 
        LOG_ERROR_SDL(DR_ERROR);
    }
    return DR_OK;
}

int DRThread::Thread(void* data)
{
	DRThread* t = static_cast<DRThread*>(data);
	while(SDL_SemTryWait(t->semaphore)==SDL_MUTEX_TIMEDOUT)
	{
		// Lock work mutex
		t->lock();
		int status = SDL_CondWait(t->condition, t->mutex); LOG_ERROR_SDL(-1);
		if( status == 0)
		{
            int ret = t->ThreadFunction();
            t->unlock();
            if(ret)
            {
                DREngineLog.writeToLog("error-code: %d", ret);
                LOG_ERROR("error in user defined thread, exit thread", -2);
            }
  		}
		else
		{
			//unlock mutex and exit
            t->unlock();
			LOG_ERROR("Fehler in Thread, exit", -1);
		}
	}
    return 0;
}