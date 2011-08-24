#include "Engine2Main.h"


DRText::DRText(DRFont* pFont)
: m_vPosition(0.0f), pcText(NULL), uiFlags(0),Color1(1.0f), Color2(-1.0f),
  m_vScaling(1.0f), m_fSpace(-2.0f), fItalic(0.0f), fLineSpace(-5.0f),
  m_fTabStops(25.0f), fXOffset(0.0f), iLength(-1),
  m_pFont(pFont)
{
	if(!pFont)
		LOG_ERROR_VOID("Fehler in Text::Text, pFont ist NULL");
	memset(acText, 0, DR_FONT_TEXTBUFFERSIZE);
}

//----------------------------------------------------------------------------------------------------------------------

DRText::DRText(DRText* pText)
{
	memcpy(this, pText, sizeof(DRText));
	pcText = NULL;
}

//----------------------------------------------------------------------------------------------------------------------

void DRText::setText(const char* _pcText, ...)
{
	//variablen
	va_list   Argumente;
	char acBuffer[1024];
	resetText();

	//Buffer f�llen
	va_start(Argumente, _pcText);
 	vsprintf(acBuffer, _pcText, Argumente);
	va_end(Argumente);

	if(strlen(acBuffer) >= DR_FONT_TEXTBUFFERSIZE)
	{
		pcText = new char[strlen(acBuffer)];
		strcpy(pcText, acBuffer);
	}
	else
	{
		sprintf(acText, "%s", acBuffer);
	}
}

//----------------------------------------------------------------------------------------------------------------------

void DRText::setText(char* _pcText)
{
	resetText();
	if(!_pcText) return;
	if(strlen(_pcText) >= DR_FONT_TEXTBUFFERSIZE)
	{
		pcText = new char[strlen(_pcText)];
		strcpy(pcText, _pcText);
	}
	else
	{
		sprintf(acText, "%s", _pcText);
		iLength = strlen(_pcText);
	}
}

//********************************************************************************************************************++
float DRText::getTextHeight()
{
	 if(uiFlags & DR_FF_RELATIVSCALING)
		 return m_pFont->getTextHeight()*(m_pFont->m_vScreenSize.y / 480.0f)*m_vScaling.y;

	 return m_pFont->getTextHeight()*m_vScaling.y;
}

//********************************************************************************************************************++
void DRText::resetText()
{
	if(pcText == acText) pcText = NULL;
	else DR_SAVE_DELETE_ARRAY(pcText);
}

//********************************************************************************************************************++
//********************************************************************************************************************++
DRFont::DRFont()
: mTexture(NULL),
  m_vScreenSize(g_v2WindowLength.x, g_v2WindowLength.y),
  m_pCamera(NULL)

{
}
DRFont::DRFont(const DRFont* font)
: m_FInfo(font->m_FInfo), mTexture(font->mTexture),
  m_pCamera(font->m_pCamera), m_vScreenSize(font->m_vScreenSize)
{

}
//---------------------------------------------------------------------------
DRFont::~DRFont()
{
    if(mTexture)
    {
        DR_SAVE_DELETE(mTexture);
    }
}
//*********************************************************************************
DRReturn DRFont::init(const char* pcTGAFilename, const char* pcTBFFilename)
{
	//Parameter pr�fen
	if(!pcTBFFilename) LOG_ERROR("kein Filename f�r tbf File �bergeben!", DR_ZERO_POINTER);
	if(!pcTGAFilename) LOG_ERROR("kein Filename f�r TGA File �bergeben!", DR_ZERO_POINTER);

	//Textur laden
        mTexture = new DRTextur(pcTGAFilename);
	//m_uiTextureID = LoadTexture(pcTGAFilename, true);
	if(!mTexture->isLoaded())
	{
            printf("TGA Texture: %s konnte nicht geladen werden\n", pcTGAFilename);
            LOG_ERROR("Fehler beim laden der TGA Textur!", DR_ERROR);
	}

	//Info File laden
	FILE* file = fopen(pcTBFFilename, "rb");
        if(!file) LOG_ERROR("tbf File konnte nicht ge�ffnet werden!", DR_ERROR);
	if(fread(&m_FInfo, sizeof(SFontInfo), 1, file) < 1)
	{
		fclose(file);
		printf("Fehler beim lesen von: %s\n", pcTBFFilename);
		LOG_ERROR("Fehler beim lesen der tbf Datei", DR_ERROR);
	}

	fclose(file);
	SDL_Surface* pTempS = SDL_GetVideoSurface();
	m_vScreenSize.x = (DRReal)pTempS->w;
	m_vScreenSize.y = (DRReal)pTempS->h;

	return DR_OK;
}

//*******************************************************************************************************

float DRFont::computeTextWidth(const char* pcText, int iLength /* = -1 */, const float fSpace /* = 0.0f */)
{
	//Parameter Check
	if(!pcText) LOG_ERROR("NULL Pointer", 0);
	if(strlen(pcText) == 0) return 0.0f;

	float fWidth = 0.0f;

	// L�nge anpassen
	if(-1 == iLength) iLength = (int)(strlen(pcText));
	if( 0 == iLength) return 0.0f;

	//Jedes Zeichen durchgehen und Gr��e addieren
	for(int i = 0; i < iLength; i++)
		fWidth += m_FInfo.afCharWidth[pcText[i]];

	// n Zeichen: n - 1 Leerr�ume
	fWidth += (float)(iLength - 1) * fSpace;
	return fWidth;
}


DRReturn DRText::drawText()
{
	if(this == NULL) LOG_ERROR("Fehler in DRText::DrawText, this = NULL", DR_ERROR);

	SFontVertex		aVertex[600];
	unsigned int	uiVertexCursor = 0;
	DRVector2 vScaling = m_vScaling;
	float fTabStops = m_fTabStops;
	float fSpace = m_fSpace;
	DRVector2 vPosition = m_vPosition;

	// Parameter pr�fen
	if(!pcText) pcText = acText;//LOG_ERROR("pcText ist ein Null Pointer", DR_ZERO_POINTER);
	if(strlen(pcText) == 0) return DR_OK;

	if(iLength == -1) iLength = (int)(strlen(pcText));
//	SFontVertex*    aVertex = new SFontVertex[iLength+1];


	if(Color2.r < 0.0f) Color2 = Color1;
	DRVector2 vCursor(vPosition);

	// Relative Koordinaten in absolute umrechnen
	if(uiFlags & DR_FF_RELATIVE)
	{
		vPosition.x *= m_pFont->m_vScreenSize.x;
		vPosition.y *= m_pFont->m_vScreenSize.y;
		fTabStops *= m_pFont->m_vScreenSize.x / 640.0f;
	}
	if(vPosition.y > g_v2WindowLength.y) return DR_OK;

	// Relative Skalierung in absolute umrechnen
	if(uiFlags & DR_FF_RELATIVSCALING)
	{
		vScaling.x *= m_pFont->m_vScreenSize.x / 640.0f;
		vScaling.y *= m_pFont->m_vScreenSize.y / 480.0f;
	}

	// Zeichenh�he berechnen
	float fCharHeight = m_pFont->m_FInfo.fHeight * vScaling.y;

	// Cursor auf die horizontale Startposition setzen
	if(uiFlags & DR_FF_ALIGN_HCENTER)
	{
		// Zentrieren
		float fTextWidth = m_pFont->computeTextWidth(pcText, iLength, fSpace) * vScaling.x;
		vCursor.x = vPosition.x - 0.5f * fTextWidth;
	}
	else if(uiFlags & DR_FF_ALIGN_HRIGHT)
	{
		// Rechtsb�ndig
		float fTextWidth = m_pFont->computeTextWidth(pcText, iLength, fSpace) * vScaling.x;
		vCursor.x = vPosition.x - fTextWidth;
	}
	else
	{
		// Linksb�ndig
		vCursor.x = vPosition.x;
	}

	// L�nge, "Kursivit�t" und Abst�nde anpassen (mit Skalierung multiplizieren)
	fItalic *= vScaling.x;
	fSpace *= vScaling.x;
	fLineSpace *= vScaling.y;
	fXOffset *= vScaling.x;

	// Cursor auf die vertikale Startposition setzen
	if(uiFlags & DR_FF_ALIGN_VCENTER)
	{
		// Text vertikal zentrieren
		vCursor.y = vPosition.y - 0.5f * m_pFont->m_FInfo.fHeight * vScaling.y;
	}
	else if(uiFlags & DR_FF_ALIGN_VTOP)
	{
		// Text oben ausrichten
		vCursor.y = vPosition.y - m_pFont->m_FInfo.fHeight * vScaling.y;
	}
	else
	{
		// Text unten ausrichten
		vCursor.y = vPosition.y;
	}

	// Startcursorposition speichern
	DRVector2 vStartCursor(vCursor);

	// Jedes einzelne Zeichen durchgehen
	for(unsigned int dwChar = 0; dwChar < (unsigned int)(iLength); dwChar++)
	{
		// Wenn das Zeichen ein Neue-Zeile-Zeichen ("\n") ist, dann wird vCursor.y
		// erh�ht und vCursor.x auf vStartCursor.x zur�ckgesetzt.
		if(pcText[dwChar] == '\n')
		{
			vCursor.x = vStartCursor.x;
			vCursor.y += (m_pFont->m_FInfo.fHeight * vScaling.y) + fLineSpace;
		}
		else if(pcText[dwChar] == '\t')
		{
			// Das Zeichen ist ein Tabulator!
			// vCursor.x wird bis zum n�chsten Tab-Stopp bewegt.
			vCursor.x += fTabStops - fmodf(vCursor.x, fTabStops);
		}
		else if(pcText[dwChar] == '\\')
		{
			if(dwChar < (unsigned int)(iLength) - 1)
			{
				if(pcText[dwChar + 1] == 'n')
				{
					vCursor.x = vStartCursor.x;
					vCursor.y += (m_pFont->m_FInfo.fHeight * vScaling.y) + fLineSpace;
					dwChar++;
				}
				else if(pcText[dwChar + 1] == 't')
				{
					vCursor.x += fTabStops - fmodf(vCursor.x, fTabStops);
					dwChar++;
				}
			}
		}
		else
		{
			// Es ist ein anzeigbares Zeichen!
			// Breite des Zeichens abfragen
			float fCharWidth = m_pFont->m_FInfo.afCharWidth[(unsigned char)(pcText[dwChar])] * vScaling.x;

			// Sechs Vertizes f�r das aktuelle Zeichen hinzuf�gen.
			// Dazu verwenden wir Dreieckslisten - jeweils sechs Vertizes
			// beschreiben ein Viereck.
                        glBegin(GL_QUADS);
			// Erster Vertex: links oben
			aVertex[uiVertexCursor].vPosition = DRVector3(vCursor.x + fItalic + fXOffset, vCursor.y, 0.0f);
			aVertex[uiVertexCursor].Diffuse = Color1.interpolate(Color2, (float)(dwChar) / (float)(iLength));
			aVertex[uiVertexCursor].vTexture = m_pFont->m_FInfo.avTopLeft[(unsigned char)(pcText[dwChar])];
                        glColor4fv(aVertex[uiVertexCursor].Diffuse);
                        glTexCoord2fv(aVertex[uiVertexCursor].vTexture);
                        //glVertex3fv(aVertex[uiVertexCursor].vPosition);
                        glVertex3f(vCursor.x + fItalic + fXOffset, vCursor.y, 0.0f);
                        
			// Zweiter Vertex: rechts oben
			aVertex[uiVertexCursor + 1].vPosition = DRVector3(vCursor.x + fCharWidth + fItalic + fXOffset, vCursor.y, 0.0f);
			aVertex[uiVertexCursor + 1].Diffuse = Color1.interpolate(Color2, (float)(dwChar + 1) / (float)(iLength));
			aVertex[uiVertexCursor + 1].vTexture.x = m_pFont->m_FInfo.avBottomRight[(unsigned char)(pcText[dwChar])].x;
			aVertex[uiVertexCursor + 1].vTexture.y = m_pFont->m_FInfo.avTopLeft[(unsigned char)(pcText[dwChar])].y;
                        glColor4fv(aVertex[uiVertexCursor+1].Diffuse);
                        glTexCoord2fv(aVertex[uiVertexCursor+1].vTexture);
                        //glVertex3fv(aVertex[uiVertexCursor+1].vPosition);
                        glVertex3f(vCursor.x + fCharWidth + fItalic + fXOffset, vCursor.y, 0.0f);

			// Dritter Vertex: rechts unten
			aVertex[uiVertexCursor + 2].vPosition = DRVector3(vCursor.x + fCharWidth + fXOffset, vCursor.y + fCharHeight, 0.0f);
			aVertex[uiVertexCursor + 2].Diffuse = aVertex[uiVertexCursor + 1].Diffuse;
			aVertex[uiVertexCursor + 2].vTexture = m_pFont->m_FInfo.avBottomRight[(unsigned char)(pcText[dwChar])];
                        glColor4fv(aVertex[uiVertexCursor+2].Diffuse);
                        glTexCoord2fv(aVertex[uiVertexCursor+2].vTexture);
                        //glVertex3fv(aVertex[uiVertexCursor+2].vPosition);
                        glVertex3f(vCursor.x + fCharWidth + fXOffset, vCursor.y + fCharHeight, 0.0f);

		/*	// Vierter Vertex = erster Vertex
			aVertex[uiVertexCursor + 3] = aVertex[uiVertexCursor];

			// F�nfter Vertex = dritter Vertex
			aVertex[uiVertexCursor + 4] = aVertex[uiVertexCursor + 2];
			*/

			// Sechster Vertex: links unten
			aVertex[uiVertexCursor + 3].vPosition = DRVector3(vCursor.x + fXOffset, vCursor.y + fCharHeight, 0.0f);
			aVertex[uiVertexCursor + 3].Diffuse = aVertex[uiVertexCursor].Diffuse;
			aVertex[uiVertexCursor + 3].vTexture.x = m_pFont->m_FInfo.avTopLeft[(unsigned char)(pcText[dwChar])].x;
			aVertex[uiVertexCursor + 3].vTexture.y = m_pFont->m_FInfo.avBottomRight[(unsigned char)(pcText[dwChar])].y;
                        glColor4fv(aVertex[uiVertexCursor+3].Diffuse);
                        glTexCoord2fv(aVertex[uiVertexCursor+3].vTexture);
                        //glVertex3fv(aVertex[uiVertexCursor+3].vPosition);
                        glVertex3f(vCursor.x + fXOffset, vCursor.y + fCharHeight, 0.0f);
                        
                        glEnd();
                        
                        
			// Vertexcursor aktualisieren
			uiVertexCursor += 4;

			// Cursor aktualisieren
			vCursor.x += fCharWidth + fSpace;
		}

		// Wenn das Vertex-Array voll oder der Text zu Ende ist, dann wird jetzt gezeichnet.
	/*	if(uiVertexCursor >= 600 ||
		   dwChar == (unsigned int)(iLength - 1))
		{
			// Jeden Durchgang des Effekts zeichnen
			for(int iPass = 0; iPass < m_iNumPasses; iPass++)
			{
				m_pEffect->Pass(iPass);
				tbDirect3D::Instance()->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
														uiVertexCursor / 3,
														aVertex,
														sizeof(tbFontVertex));
			}

			// Vertexcursor zur�cksetzen
			uiVertexCursor = 0;
		}
		*/
		if(uiVertexCursor >= 600 || dwChar == (unsigned int)(iLength - 1))
		{
			//Jetzt Zeichnen, mit Vertex-Arrays
			glVertexPointer(3, GL_FLOAT, sizeof(SFontVertex), &aVertex[0].vPosition);
			glColorPointer(4, GL_FLOAT, sizeof(SFontVertex), &aVertex[0].Diffuse);
			glTexCoordPointer(2, GL_FLOAT, sizeof(SFontVertex), &aVertex[0].vTexture);

			//glDrawArrays(GL_QUADS, 0, uiVertexCursor);
                                                
			//glTranslated(10.0, 0.0f, 0.0f);
		}


	}

	if(DRGrafikError("OpenGL Fehler in DrawText")) return DR_ERROR;
	iLength = -1;
//	DR_SAVE_DELETE_ARRAY(aVertex);
        
	return DR_OK;
}

//----------------------------------------------------------------------------------------------------------------------


DRReturn DRFont::begin(bool bSmall /* = false */)
{
	if(this == NULL) LOG_ERROR("Fehler in DRFont::Begin, this = NULL", DR_ERROR);
	if(!bSmall)
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_FOG);
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, m_vScreenSize.x, m_vScreenSize.y, 0, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
                
                glDisable(GL_CULL_FACE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

//	glBindTexture(GL_TEXTURE_2D, m_uiTextureID);
        mTexture->bind();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(DRGrafikError("Fehler in Font::Begin()")) return DR_ERROR;

	return DR_OK;
}


//----------------------------------------------------------------------------------------------------------------------


DRReturn DRFont::end()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_FOG);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(DRGrafikError("Fehler in DRFont::End")) return DR_ERROR;

	return DR_OK;
}

//----------------------------------------------------------------------------------------------------------------------
