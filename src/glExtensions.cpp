#include "Engine2Main.h"

typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);
typedef void (APIENTRY * PFNGLBUFFERSUBDATAARBPROC) (GLenum target, GLint offset, int size, const GLvoid *data);
//Extension

// VBO Extension Funktionszeiger für VertexBuffer
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;  // VBO Namens Generations-Prozedur
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL; // VBO Bind-Prozedur
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;  // VBO Daten-Lade-Prozedur
PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB = NULL;  // VBO Daten-Lade-Prozedur
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL; // VBO Lösch-Prozedur


bool DRIsExtensionSupported(const char* szTargetExtension )
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;

	// Extension Namen sollten keine Leerzeichen enthalten
	pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
	if( pszWhere || *szTargetExtension == '\0' ) return false;

	// ermittle Extensions String
	pszExtensions = glGetString( GL_EXTENSIONS );

	// such nach einer exakten Kopie des Extensions Strings
	pszStart = pszExtensions; for(;;)
	{
		pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
		if( !pszWhere ) break;

		pszTerminator = pszWhere + strlen( szTargetExtension );
		if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
			if( *pszTerminator == ' ' || *pszTerminator == '\0' )
				return true;

		pszStart = pszTerminator;
	}
	return false;

}

//********************************************************************************************************************++

//***********************************************************************************************************************

DRReturn DRActivateVBExtension()
{
	//VertexBuffer Extension
	//schauen ob VertexBuffer überhaupt funktionieren
	if(!DRIsExtensionSupported( "GL_ARB_vertex_buffer_object" )) 
            LOG_ERROR("Extension VertexBuffer wird nicht unterstützt vom aktuellem System!", DR_ERROR);

	// ermittle Zeiger auf die GL Funktionen
	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
        glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC) wglGetProcAddress("glBufferSubDataARB");
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");

	return DR_OK;
}

//********************************************************************************************************************++

void DRWriteExtensionsToLog(const char* pcExtensions, FILE* pFile /* = NULL*/)
{
	if(!pcExtensions) return;
        if(!pFile) return;
        
	unsigned int uiStringSize = strlen(pcExtensions);
    char* buffer = (char*)malloc(uiStringSize+2);
	for (unsigned int i = 0; i < uiStringSize; i++)
	{
            if(pcExtensions[i] == ' ')
                buffer[i] = '\n';// fprintf(pFile, "\n");
            else
                buffer[i] = pcExtensions[i];//fprintf(pFile, "%c", pcExtensions[i]);
	}
        buffer[uiStringSize] = '\n';
        buffer[uiStringSize+1] = '\0';
        fprintf(pFile, buffer);
		free(buffer);

}

//********************************************************************************************************************++


