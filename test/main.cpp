#include <iostream>
#include "Engine2Main.h"

#include "IniTest.h"

using namespace std;

DRTexturePtr tex;

DRReturn render(float ftime)
{
    glClearColor(0.1f, 0.0f, 0.0f, 0.0f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity ();

    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);

    tex->bind();
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2i(1, 1);
        glTexCoord2f(0.0f, 1.0f); glVertex2i(1, 479);
        glTexCoord2f(1.0f, 1.0f); glVertex2i(639, 479);
        glTexCoord2f(1.0f, 0.0f); glVertex2i(639, 1);
    glEnd();
    return DR_OK;
}

DRReturn move(float ftime)
{

    return DR_OK;
}
#ifdef _WIN32
#undef main
#endif
int main()
{
    cout << "Hello world!" << endl;
    EnInit_Simple();
    EnInit_OpenGL(1.0, DRVideoConfig(640, 480), "Test Suit!");

	int videoDriverCount = SDL_GetNumVideoDrivers();
	for(int i = 0; i < videoDriverCount; i++)
	{
		printf("videodriver(%d): %s\n", i, SDL_GetVideoDriver(i));
	}
	printf("current video drove: %s\n", SDL_GetCurrentVideoDriver());

    printf("\n");
    IniTest ini("test.ini");
    if(!ini.test(false))
        printf("Fehler in der Klasse: DRIni!\n");
    else
        printf("Der Test fuer die Klasse DRIni war erfolgreich!\n");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640, 0, 480, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

   // DRIvlc::getSingleton().init();
  //  DRINetwork::getSingleton().init();
   // DRINetwork::getSingleton().HTTPRequest("www.einhornimmond.de", NET_GET, "");

    //DRINetwork::getSingleton().exit();

    tex = DRTextureManager::Instance().getTexture("test.jpg");

    if(EnGameLoop(move, render, true))
        LOG_WARNING("Fehler in der GameLoop");

    EnExit();
  //  */
    return 0;
}
