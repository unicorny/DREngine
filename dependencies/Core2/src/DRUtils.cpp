#include "Core2Main.h"


int DRGetNextPotenz(int iIn)
{
	int iRes = 2;
	while(iRes < iIn)
	{
		iRes *= 2;
	}
	return iRes;
}


//****************************************************************

DRReturn DRGetTermVorChar(DRString* sInput, DRString* sOutput, char cTrenn)
{
	u32 n = 0, n2 = 0;
	const char* pcTemp = sInput->data();
	char* pcOut = new char[sInput->length()];
	while(pcTemp[n] != cTrenn &&
		  pcTemp[n] != '\0')
		 {
			if(pcTemp[n] != ' ')
			{
				pcOut[n2] = pcTemp[n];
				n2++;
			}
			n++;
			if(n > sInput->length()) return DR_ERROR;
		 }
	pcOut[n2] = '\0';
	*sOutput = pcOut;
	delete[] pcOut;

	return DR_OK;
}

//------------------------------------------------------------------------------

DRReturn DRGetTermNachChar(DRString* sInput, DRString* sOutput, char cTrenn)
{
	int n = sInput->length(), n2 = sInput->length();
	const char* pcTemp = sInput->data();
	char* pcOut = new char[sInput->length()];
	while(pcTemp[n] != cTrenn)
		 {
			if(pcTemp[n] != ' ' &&
			   pcTemp[n] != '\0')
			{
				pcOut[n2] = pcTemp[n];
				n2--;
			}
			n--;
			if(n <= 0) return DR_ERROR;
		 }
	*sOutput = pcOut;
	delete[] pcOut;
	return DR_OK;
}

//*************************************************************************

bool DRisMathChar(char czeichen)
{
	// 0 -> 9 . , () +*-/
	if((int)czeichen >= 40 &&
	   (int)czeichen <= 57) return true;
	//, und .
//	if((int)czeichen == 44 ||
//	   (int)czeichen == 46) return true;
	//^
	if((int)czeichen == 94) return true;

	//- und +
//	if((int)czeichen == 45 ||
//	   (int)czeichen == 43) return true;

	return false;
}

//*****************************************************************************

int DRCountCharsOf(DRString* sInput, char cCountable)
{
	int iret = 0;
	const char* pcTemp = sInput->data();

	for(u32 i = 0; i < sInput->length(); i++)
	{
		if(pcTemp[i] == cCountable) iret++;
	}
	return iret;
}

//*************************************************************************

int DRCountNumbers(DRString* sInput)
{
	int iret = 0;
	for(u32 i = 0; i < sInput->length(); i++)
	{
		int c = (int)(sInput->data()[i]);
		if(c >= 48 &&
		   c <= 57) iret++;
	}

	return iret;
}

//****************************************************************

bool DRisNumber(char cZeichen)
{
	// 0 - 9
	if((int)cZeichen >= 48 &&
	   (int)cZeichen <= 57) return true;

	return false;

}


//********************************************************************

// Entfernt HTML-Tags aus einem String
DRReturn DRRemoveHTMLTags(char* pcIn,
						char* pcOut,
						u32 dwOutLength)
{
	u32 dwInLength = strlen(pcIn);
	u32 dwOut = 0;


	memset(pcOut, 0, dwOutLength);
	for(u32 dwIn = 0; dwIn < dwInLength; dwIn++)
	{
		if(pcIn[dwIn] != '<') {pcOut[dwOut] = pcIn[dwIn]; dwOut++;}
		else
		{
			while(pcIn[dwIn] != '>') dwIn++;
		}
	}

	return DR_OK;
}

//*****************************************************************+

const char* DRRemoveDir(const char* pcFilename)
{
	int iLastBackSlash;
	int iChar;


	// Letzten Back-Slash ("\") suchen
	iLastBackSlash = -1;
	iChar = 0;
	while(pcFilename[iChar] != 0)
	{
		if(pcFilename[iChar] == '\\') iLastBackSlash = iChar;
		iChar++;
	}

	// String ab dem letzten Back-Slash plus 1 zur�ckliefern
	return pcFilename + iLastBackSlash + 1;
}


/*
//********************************************************************************************************************++
//Vectorberechnungen

// ******************************************************************
// 2D-Positionsvektor transformieren
DRVector2 DRVector2TransformCoords(const DRVector2& v,
											   const DRMatrix& m,
											   DRReal* const pfOutW) // = NULL
{
	// Vektor mit Matrix transformieren
	DRVector2 vResult(v.x * m.m11 + v.y * m.m21 + m.m41,
		              v.x * m.m12 + v.y * m.m22 + m.m42);

	// Vierte Koordinate (w) berechnen. Wenn diese ungleich eins
	// ist, m�ssen die anderen Vektorelemente durch sie geteilt
	// werden.
	const DRReal w = v.x * m.m14 + v.y * m.m24 + m.m44;
	if(w != 1.0f) vResult /= w;

	// Wenn erw�nscht, w kopieren
	if(pfOutW) *pfOutW = w;

	return DRVector2Normalize(vResult) * DRVector2Length(v);
}

// ******************************************************************
// 2D-Richtungsvektor transformieren
DRVector2 DRVector2TransformNormal(const DRVector2& v,
											   const DRMatrix& m)
{
	// Vektorl�nge berechnen
	const DRReal fLength = DRVector2Length(v);
	if(fLength == 0.0f) return v;

	// Transponierte inverse Matrix berechnen
	const DRMatrix mTransform(DRMatrixTranspose(DRMatrixInvert(m)));

	// Vektor mit Matrix transformieren und urspr�ngliche L�nge wiederherstellen
	return DRVector2Normalize(DRVector2(v.x * mTransform.m11 + v.y * mTransform.m21,
		                                v.x * mTransform.m12 + v.y * mTransform.m22))
	       * fLength;
}

// ******************************************************************
// 2D-Richtungsvektor transformieren (transponierte invertierte Matrix)
DRVector2 DRVector2TransformNormal_TranspInv(const DRVector2& v,
														 const DRMatrix& m)
{
	// Vektorl�nge berechnen
	const DRReal fLength = DRVector2Length(v);
	if(fLength == 0.0f) return v;

	// Vektor mit Matrix transformieren und urspr�ngliche L�nge wiederherstellen
	return DRVector2Normalize(DRVector2(v.x * m.m11 + v.y * m.m21,
		                                v.x * m.m12 + v.y * m.m22))
	       * fLength;
}

// ******************************************************************
// 3D-Positionsvektor transformieren
DRVector3 DRVector3TransformCoords(const DRVector3& v,
											   const DRMatrix& m,
											   DRReal* const pfOutW) // = NULL
{
	// Vektor mit Matrix multiplizieren
	DRVector3 vResult(v.x * m.m11 + v.y * m.m21 + v.z * m.m31 + m.m41,
		              v.x * m.m12 + v.y * m.m22 + v.z * m.m32 + m.m42,
					  v.x * m.m13 + v.y * m.m23 + v.z * m.m33 + m.m43);

	// Vierte Koordinate (w) berechnen. Wenn diese ungleich eins
	// ist, m�ssen die anderen Vektorelemente durch sie geteilt
	// werden.
	const DRReal w = v.x * m.m14 + v.y * m.m24 + v.z * m.m34 + m.m44;
	if(w != 1.0f) vResult /= w;

	// Wenn erw�nscht, w kopieren
	if(pfOutW) *pfOutW = w;

	return vResult;
}

// ******************************************************************
// 3D-Richtungsvektor transformieren
DRVector3 DRVector3TransformNormal(const DRVector3& v,
											   const DRMatrix& m)
{
	// Vektorl�nge berechnen
	const DRReal fLength = DRVector3Length(v);
	if(fLength == 0.0f) return v;

	// Transponierte invertierte Matrix berechnen
	const DRMatrix mTransform(DRMatrixTranspose(DRMatrixInvert(m)));

	// Vektor mit Matrix transformieren und urspr�ngliche L�nge wiederherstellen
	return DRVector3Normalize(DRVector3(v.x * mTransform.m11 + v.y * mTransform.m21 + v.z * mTransform.m31,
		                                v.x * mTransform.m12 + v.y * mTransform.m22 + v.z * mTransform.m32,
										v.x * mTransform.m13 + v.y * mTransform.m23 + v.z * mTransform.m33))
		   * fLength;
}

// ******************************************************************
// 3D-Richtungsvektor transformieren (transponierte invertierte Matrix)
DRVector3 DRVector3TransformNormal_TranspInv(const DRVector3& v,
														 const DRMatrix& m)
{
	// Vektorl�nge berechnen
	const DRReal fLength = DRVector3Length(v);
	if(fLength == 0.0f) return v;

	// Vektor mit Matrix transformieren und urspr�ngliche L�nge wiederherstellen
	return DRVector3Normalize(DRVector3(v.x * m.m11 + v.y * m.m21 + v.z * m.m31,
		                                v.x * m.m12 + v.y * m.m22 + v.z * m.m32,
										v.x * m.m13 + v.y * m.m23 + v.z * m.m33))
		   * fLength;
}

//********************************************************************************************************************++
*/
/*
bool DRIfPointInQuad(DRVector3 vPoint, DRVector3 vMin, DRVector3 vMax)
{
	DRVector3 vTemp1, vTemp2;
	vTemp1 = vPoint - vMin;
	vTemp2 = vMax - vMin;	//Differenz
	if(fabs(vTemp1.x) > fabs(vTemp2.x)) return false;
	if(fabs(vTemp1.y) > fabs(vTemp2.y)) return false;
	if(fabs(vTemp1.z) > fabs(vTemp2.z)) return false;

	return true;
}
*/

//********************************************************************************************************************++

bool DRCheckEndung(const char* pcInput, const char* pcEndung)
{
	int iLastDot;
	int iChar;

	// Letzten Punkt (".") im Dateinamen suchen
	iLastDot = -1;
	iChar = 0;
	while(pcInput[iChar] != 0)
	{
		if(pcInput[iChar] == '.') iLastDot = iChar;
		iChar++;
	}

	// String ab dem letzten Punkt plus 1 zur�ckliefern
	return !strcmp(&pcInput[iLastDot+1], pcEndung);
}


//********************************************************************************************************************++
DRReturn DRGetPfad(const char* pcInput, char* pcOutput, int iOutBufferSize /* = 256 */)
{
	if(!pcInput ||
	   !pcOutput) return DR_ZERO_POINTER;

	int iMark = 0;
	int iLength = strlen(pcInput);
	int i = 0;
	for (i = 0; i < iLength; i++)
	{
		if(pcInput[i] == '\\' ||
		   pcInput[i] == '/')
		   iMark = i;
	}
	for (i = 0; i < iMark; i++)
	{
		pcOutput[i] = pcInput[i];
		if(i >= iOutBufferSize) LOG_ERROR("GetPfad OutputBuffer ist zu klein!", DR_ERROR);
	}
	return DR_OK;
}

