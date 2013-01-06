#include "Engine2Main.h"

//F�r Versionskontrolle
const DRReal g_fVersion = 2.0f;

//Z�hler, wie oft die dll gerade benutzt wird
int         g_iProzess = 0;
int			g_iProzessFunk = 0;
SDL_Surface* g_pSDLWindow = NULL;
ENGINE2_API DRVector2  g_v2WindowLength = DRVector2(0.0f);
bool		g_bOgre = false;
bool		g_bEnInit = false;
bool		g_bGL = false;
DRGameStateManager* g_pGameStateManager = NULL;

Uint8*		g_piPressed = NULL;
u16             g_CPU_Count = 0;
DREngineLogger DREngineLog;

//********************************************************************************************************************++

#ifdef _WIN32
//DLL Main Funktion
int WINAPI DllMain(HINSTANCE DllHandle, unsigned long ReasonForCall, void* Reserved)
{
	//Wenn ein weiterer Prozess die dll benutzt
   switch (ReasonForCall)
   {
      case DLL_PROCESS_ATTACH:
      {
         //Der Referenzz�hler wird um eins erh�ht
		  //Start
		  g_iProzess++;
      } break;

      case DLL_PROCESS_DETACH:
      {
         //Der Referenzz�hler wird um eins reduziert
		  //Ende
		  g_iProzess--;

			//Sicherstellen, das Exit aufgerufen wird
		  if(g_iProzessFunk > g_iProzess)
		  {
			  LOG_WARNING("Exit musste automatisch aufgerufen werden! SDL");
			  EnExit();
			 // GameDLLExit();
		  }
	  } break;

   }


   return 1;
}
#endif //_WIN32

//********************************************************************************************************************++

//********************************************************************************************************************++
DRReturn fVersionCheck(DRReal fVersion)
{
	if(fVersion > g_fVersion)
	{
		DRLog.writeToLog("Programm ben�tigt eine zu hohe EngineDLL Version, ben�tigt: %f, verf�gbar: %f", fVersion, g_fVersion);
		return DR_ERROR;
	}
	return DR_OK;
}

//****************************************************************************************************************************
DRReturn EnInit_Simple(DRReal fVersion /* = 0.0f*/, bool initSound/* = false*/)
{
	if(fVersionCheck(fVersion)) return DR_ERROR;

	if(!g_bEnInit)
            if(EnInit(fVersion, initSound))
            {
                    printf("Fehler bei EnInit");
                    return DR_ERROR;
            }

	if(SDL_Init(SDL_INIT_TIMER) < 0)
	{
            DRLog.writeToLog("SDL konnte nicht initalisiert werden! Fehler: %s\n", SDL_GetError());
            LOG_ERROR("Fehler bei SDL Init", DR_ERROR);
	}

	
	//Not Exit Funktion festlegen
	atexit(SDL_Quit);

	LOG_INFO("Engine Simple Initalisiert!");
	return DR_OK;
}
//******************************************************************************************
DRReturn EnInit(DRReal fVersion /* = 1.0f */, bool initSound/* = false*/)
{
    DREngineLog.init("EngineLogger.html", true);
	Core2_init("Logger.html");
	if(fVersionCheck(fVersion)) return DR_ERROR;

    g_pGameStateManager = new DRGameStateManager;
    
	g_bEnInit = true;
	LOG_INFO("Engine wurde initalisiert");
	return DR_OK;
}

DRReturn EnInit_INI(const char* iniFileName)
{
	DRIni ini(iniFileName);
	if(!ini.isValid())
	{
        fprintf(stderr, "given ini file isn't valid\n");
        return DR_ERROR;
	}
	DRReal version = ini.getReal("Base", "Version");
	if(version == 0.0f) version = 1.0f;
	//int initSound = ini.getInt("Base", "Sound");
	const char* tempString = ini.getStr("Base", "Mode")->data();
    printf("\ncompareStr: \n%s\n compare with: %s, result: %d\n", tempString, "OpenGL", strcmp("OpenGL", tempString));
	if(!strcmp("OpenGL", tempString))
	//|| !strcmp("SDL", tempString))
	{
            // Titel
            char* titel = NULL;
            tempString = ini.getStr("Base", "Titel")->data();
            if(tempString)
            {
                    if(strlen(tempString) > 5)
                    {
                            titel = new char[strlen(tempString)+1];
                            strcpy(titel, tempString);
                    }
            }
            // BMP Icon
            char* icon = NULL;
            tempString = ini.getStr("Base", "Icon")->data();
            if(tempString)
            {
                    if(strlen(tempString) > 5)
                    {
                            icon = new char[strlen(tempString)+1];
                            strcpy(icon, tempString);	
                    }
            }
            // Video Config
            DRVideoConfig config;
            config.setFullscreen(ini.getInt("OpenGL", "Vollbild"));
#ifdef _DEBUG
            config.setFullscreen(0);
#endif
            if(ini.getInt("OpenGL", "Breite"))
                config.setWidth(ini.getInt("OpenGL", "Breite"));
            if(ini.getInt("OpenGL", "Hoehe"))
                    config.setHeight(ini.getInt("OpenGL", "Hoehe"));
            if(ini.getInt("OpenGL", "MultiSample"))
                    config.setMultiSampling(ini.getInt("OpenGL", "MultiSample"));
            if(ini.getInt("OpenGL", "ZBuffer"))
                    config.setZBuffer(ini.getInt("OpenGL", "ZBuffer"));

            tempString = ini.getStr("Base", "Mode")->data();
            DRReturn returnValue;
            //if(!strcmp("OpenGL", tempString))
                returnValue = EnInit_OpenGL(version, config, titel, icon);
            //else 
            //	returnValue = EnInit_SDL(version, config, titel, icon);
            DR_SAVE_DELETE_ARRAY(titel);
            DR_SAVE_DELETE_ARRAY(icon);


            return returnValue;
	}
	else if(!strcmp("Core", tempString))
	{
            return Core2_init();
	}
	else
	{
            if(strcmp("Simple", tempString))
                    fprintf(stderr, "Mode in ini File isn't give, using Simple\n");

            //return EnInit_Simple(version, (bool)initSound);
            return EnInit_Simple(version);
                
	}
	return DR_OK;
	//*/
}

DRReturn EnInit_OpenGL(DRReal fVersion/* = 1.0f*/, DRVideoConfig video/* = DRVideoConfig()*/, const char* pcTitel/* = "OpenGL Render Fenster"*/, const char* pcBMPIcon /* = */, bool bInitSound /* = true*/)
{
	if(!g_bEnInit)
	{
        if(EnInit(fVersion, bInitSound))
        {
            fprintf(stderr, "Fehler bei EnInit\n");
            return DR_ERROR;
        }
	}

	if(fVersionCheck(fVersion))
	{
	    fprintf(stderr, "Falsche Version\n");
	    return DR_ERROR;
	}

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
            DRLog.writeToLog("SDL konnte nicht initalisiert werden! Fehler: %s\n", SDL_GetError());
            LOG_ERROR("Fehler bei SDL Init", DR_ERROR);
	}
    DRLog.writeToLog("SDL-Version: %d", SDL_COMPILEDVERSION);
	        
#if SDL_VERSION_ATLEAST(1,3,0)
        g_CPU_Count = SDL_GetCPUCount();
#endif

	//Not Exit Funktion festlegen
	atexit(SDL_Quit);

	//Fenster Titel setzen
	SDL_WM_SetCaption(pcTitel, NULL);
	if(pcBMPIcon)
		//Icon setzen
		SDL_WM_SetIcon(SDL_LoadBMP(pcBMPIcon), NULL);

	//Grafik Einstellungen
	g_v2WindowLength = video.getResolution();

	//Zufalllsgenerator starten
#ifdef _WIN32
	DRRandom::seed(timeGetTime());
#else
	DRRandom::seed(SDL_GetTicks());
#endif //_WIN32

	//OpenGL Einstellungen tätigen
	video.setGLParams();

	Uint32 uiVorGLInit = SDL_GetTicks();

	Uint32 flags = 0;
	if(video.isFullscreen())
		flags = SDL_OPENGL | SDL_FULLSCREEN;
	else
		flags = SDL_OPENGL;
	fprintf(stderr, "Bitte einen Augenblick Geduld, OpenGL wird initalisiert....\n");
#ifndef _DEBUG
	g_pSDLWindow = SDL_SetVideoMode(XWIDTH, YHEIGHT, 32, flags);
#else
	g_pSDLWindow = SDL_SetVideoMode((int)XWIDTH, (int)YHEIGHT, 32, SDL_OPENGL);
#endif //_DEBUG
	if(!g_pSDLWindow)
	{
		DRLog.writeToLog("Konnte Bildschirmmodus nicht setzen!: %s\n", SDL_GetError());
		return DR_ERROR;
	}
	Uint32 uiNachGLInit = SDL_GetTicks();
	DRLog.writeToLog("Für OpenGL Init benötigte zeit: %f Sekunden", (float)(uiNachGLInit-uiVorGLInit)/1000.0f);

	//OpenGL einrichten f�r Ohrtogonale Projection
	glViewport(0, 0, g_pSDLWindow->w, g_pSDLWindow->h);
    
    //DRActivateVBExtension();
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        DRLog.writeToLog("Error: %s\n", glewGetErrorString(err));
        LOG_ERROR("Fehler bei Glew Init", DR_ERROR);
    }
    DRLog.writeToLog("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
        
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, g_pSDLWindow->w, g_pSDLWindow->h, 0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if(video.getMultiSampling()) glEnable(GL_MULTISAMPLE_ARB);

	//----------------------------------------------------------------------------------------------------------------------
	//ein Paar Infos zu OpenGL ausgeben
	FILE* pFile = fopen("ComputerInfo.txt", "wt");
	if(pFile)
	{
	/*	DRLog.WriteToLog("------------------OpenGL Daten------------------------");
		DRLog.WriteToLog("Grafikkarten Hersteller: %s", glGetString(GL_VENDOR));
		DRLog.WriteToLog("Grafikkarte: %s", glGetString(GL_RENDERER));
		DRLog.WriteToLog("Version: %s", glGetString(GL_VERSION));
		DRLog.WriteToLog("Unterst�tzte Erweiterungen:");
		*/
		fprintf(pFile, "\n\n");
		fprintf(pFile, "------------------OpenGL Daten------------------------\n");
		fprintf(pFile, "Grafikkarten Hersteller: %s\n", glGetString(GL_VENDOR));
		fprintf(pFile, "Grafikkarte: %s\n", glGetString(GL_RENDERER));
		fprintf(pFile, "Version: %s\n", glGetString(GL_VERSION));
        fprintf(pFile, "Shader: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		fprintf(pFile, "Unterst�tzte Erweiterungen:\n");

		//DRWriteExtensionsToLog((const char*)glGetString(GL_EXTENSIONS), pFile);
                fprintf(pFile, "%s", (const char*)glGetString(GL_EXTENSIONS));

	//	DRLog.WriteToLog("------------------OpenGL Daten Ende ------------------------");
		fprintf(pFile, "------------------OpenGL Daten Ende ------------------------\n");
		fclose(pFile);
	}
	else
		LOG_WARNING("Fehler beim erstellen der OpenGL Infos Datei");

        printf("vor cursor hide\n");
	//Cursor im Release Modus verstecken
#ifndef _DEBUG
	SDL_ShowCursor(SDL_DISABLE);
#endif
    DRTextureManager::Instance().init();
    
	g_bGL = true;
	LOG_INFO("Engine OpenGL Initalisiert!");
	return DR_OK;
}


void EnExit()
{
    DR_SAVE_DELETE(g_pGameStateManager);
    if(g_bGL)
    {
        DRTextureManager::Instance().exit();
    }
	if(g_bEnInit)
	{
        SDL_Quit();
	}    
    DREngineLog.exit();
	Core2_exit();
    
}

//********************************************************************************************************************++

void EnPostExitMessageToSDL()
{
	SDL_Event Event;
	Event.type = SDL_QUIT;
	SDL_PushEvent(&Event);
}

//********************************************************************************************************************++
bool EnIsButtonPressed(SDLKey button)
{
	if(!g_piPressed) return false;
	if(g_piPressed[button] == 1)
	{
		g_piPressed[button] = 2;
		return true;
	}
	else
		return false;
}

// -----------------------------------------------------------------------------------------
DRString EnGetTimeSinceStart()
{
    char timeBuffer[256];
    double seconds = static_cast<double>(SDL_GetTicks())/1000.0;
    double minutes = 0.0;
	seconds = modf(seconds/60.0, &minutes);
    seconds *= 60.0; 
    sprintf(timeBuffer, "[%.0f:%02.0f] ", minutes, seconds);
    
    return DRString(timeBuffer);
}

//----------------------------------------------------------------------------------------------------------------------


DRReal EnSDL_Loop()
{
	static Uint32 uiLastTicks = 0;
	Uint32 uiTicks = SDL_GetTicks();

	//SDL Events abarbeiten
	SDL_Event event;

    while(SDL_PollEvent(&event))
	{
        switch(event.type)
		{
          case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				EnPostExitMessageToSDL();
				break;
			}
			break;
         }
	}
	DRReal fRet = (DRReal)(uiTicks - uiLastTicks)/1000.0f;
	uiLastTicks = uiTicks;
   	return fRet;

}
 //********************************************************************************************+

DRReturn EnGameLoop(DRGameStatePtr firstGameState, bool bExitByEsc)
{    
    g_pGameStateManager->pushState(firstGameState);
    return EnGameLoop(NULL, NULL, bExitByEsc);
}

//Hauptspielschleife
DRReturn EnGameLoop(DRReturn (*pMoveProc)(DRReal), DRReturn (*pRenderProc)(DRReal), bool bExitByEsc /* = false*/)
{
    bool bRun = true;
	int iNumKeys = 0;
#if SDL_VERSION_ATLEAST(1,3,0)
	SDL_GetKeyboardState(&iNumKeys);
#else
	SDL_GetKeyState(&iNumKeys);
#endif
	g_piPressed = new Uint8[iNumKeys];

	//Time Variablen (Frames Per Seecond geglättet)
	Uint32 uiStartTime = 0;
	Uint32 uiEndTime = SDL_GetTicks();
	DRReal fTime = 0.0f;
	double dTimes[6]; memset(dTimes, 0, 5*sizeof(double));
	char    cCounter = 0;

	//Variablen für Time Statistik
	double dGesTime = 0.0, dGesTime2 = 0.0;
	long   lNumWerte = 0;

    while(bRun)
	{
		uiStartTime = SDL_GetTicks();
#if SDL_VERSION_ATLEAST(1,3,0)
		Uint8* pKeys = SDL_GetKeyboardState(NULL);
#else
		Uint8* pKeys = SDL_GetKeyState(NULL);
#endif		
		int i = 0;
		for (i = 0; i < iNumKeys; i++)
		{
			if(pKeys[i])
			{
				if(g_piPressed[i] == 0)
					g_piPressed[i] = 1;
				else if(g_piPressed[i] == 1)
					g_piPressed[i] = 2;
			}
			else
				g_piPressed[i] = 0;
		}

	    SDL_Event event;

		//SDL Events abarbeiten
        while(SDL_PollEvent(&event))
		{
            switch(event.type)
			{
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
                    if(bExitByEsc)
                        bRun = false;
					break;
				}
                break;
            case SDL_QUIT:
				bRun = false;
                break;
            default: break;
            }
        }
        DRTextureManager::Instance().move(fTime);
		//Render und Move aufrufen
        if(pMoveProc)
            if(pMoveProc(fTime)) LOG_ERROR("Fehler beim Bewegen des Spiels", DR_ERROR);
        if(pRenderProc)
            if(pRenderProc(fTime)) LOG_ERROR("Fehler beim Rendern des Spiels", DR_ERROR);
        if(g_pGameStateManager && g_pGameStateManager->getStateCount())
        {
            if(g_pGameStateManager->move(fTime)) LOG_ERROR("Error by moving gameStateManager", DR_ERROR);
            if(g_pGameStateManager->render(fTime)) LOG_ERROR("Error by rendering gameStateManager", DR_ERROR);
        }

		dGesTime += double(SDL_GetTicks() - uiStartTime)/1000.0;
        // Pageflip: Back- und Frontbuffer werden vertauscht
		if(g_bGL)
		{
            SDL_GL_SwapBuffers();
		}
		else
		{
			if(g_pSDLWindow)
				SDL_Flip(g_pSDLWindow);
		}

		uiEndTime = SDL_GetTicks();
		//fTimes gl�tten, Zwischen 5 Werten interpolieren (den Durschnitt errechnen)
		if(uiEndTime > uiStartTime) dTimes[cCounter++] = (double)(uiEndTime - uiStartTime)/1000.0;
		else if(uiStartTime == uiEndTime) dTimes[cCounter++] = 0.0001;
		else continue;

		if(cCounter > 5) cCounter = 0;
		fTime = 0.0f;
		for (i = 0; i < 5; i++)
			fTime += (float)dTimes[i];
		fTime /= 5.0f;
		dGesTime2 += fTime;
		lNumWerte++;
    }

	//Time Statistik Auswertung
	DRLog.writeToLog("\n--------Geschwindigkeitsstatistik-----------\n");
	DRLog.writeToLog("M�gliche Frames Per Second im Schnitt: %d", (int)(1.0/(dGesTime/(double)lNumWerte)));
	DRLog.writeToLog("Tats�chliche Frames Per Second im Schnitt: %d", (int)(1.0/(dGesTime2/(double)lNumWerte)));
	DRLog.writeToLog("Programmgesamtlaufzeit %.2f Sekunden", (double)SDL_GetTicks()/1000.0);
	DRLog.writeToLog("Gemesssende Anzahl Gesamt Frames: %d", lNumWerte);
	DRLog.writeToLog("\n---------Ende Geschwindigkeitsstatistik-------------\n");

	DR_SAVE_DELETE_ARRAY(g_piPressed);

	return DR_OK;
}

//********************************************************************************************************************
const char* DRGetGLErrorText(GLenum eError)
{
	switch(eError)
	{
	case GL_INVALID_ENUM:		return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:		return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:	return "GL_INVALID_OPERATION";
	case GL_STACK_OVERFLOW:		return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW:	return "GL_STACK_UNDERFLOW";
	case GL_OUT_OF_MEMORY:		return "GL_OUT_OF_MEMORY";
    case GL_NO_ERROR:			return "GL_NO_ERROR";
	default: return "- gl Unknown error-";
	}
	return "- error -";
}


DRReturn DRGrafikError(const char* pcErrorMessage)
{
	GLenum GLError = glGetError();
	if(GLError)
	{
		DREngineLog.writeToLog("OpenGL Fehler: %s (%d)", DRGetGLErrorText(GLError), GLError);
		LOG_ERROR(pcErrorMessage, DR_ERROR);
	}
	return DR_OK;
}
