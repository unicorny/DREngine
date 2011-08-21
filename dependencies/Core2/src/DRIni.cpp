#include "Core2Main.h"

using namespace std;

DRIni::DRIni(const char* fileName)
: mFilePointer(NULL),
mValid(false)
{
	mFilePointer  = fopen(fileName, "rt");
	char c;
	if(!mFilePointer)
	{
		fprintf(stderr, "Achtung, %s konnte nicht ge�ffnet werden!! (DRIni::DRIni)", fileName);
	}
	else
	{
    do {
      c = fgetc (mFilePointer);
	  if(c == '[')
		  readEntry();
	 // printf("%c", c);
    } while (c != EOF);
    fclose (mFilePointer);
	mFilePointer = NULL;
	mValid = true;
	}
}

DRIni::~DRIni()
{
	if(mFilePointer) fclose(mFilePointer);
	mKeyMap.clear();
}
//------------------------------
void DRIni::readEntry()
{
	char c;
	bool isString = false;
	string name;
	do {
		c = fgetc(mFilePointer);
		if(c != ']')
			name += c;
		else
			break;
	} while (c != ']');
	mKeyMap.insert(MAPENTRY(name, EntryMap()));
	//printf("name: %s\n", name.data());
	string line;
	while(c != '[' && c != EOF)
	{
		c = fgetc(mFilePointer);
		if(c == '#')
		{
			while (c != '\n')
			{
				c = fgetc(mFilePointer);
			}
		}
		if(c != '\n' && c != ' ' && c != '\t' && c != '#' && c != '"')
			line += c;
        if(c == '"') isString = !isString;
		if(c == '\n' && !isString)
		{
			readParameters(name, line);
			line.clear();
		}
	}
	if(c == '[')
		readEntry();

	//mKeyMap[name].insert(STRINGENTRY("HAllo", "test"));
}
//-----------------------------------------

void DRIni::readParameters(string _name, string line)
{
	if(line.empty()) return;
	//fprintf(stderr, "line: %s\n", line.data());
	char c;
	string name;
	string key;
	string* target = &name;
	for(int i = 0; i < line.size(); i++)
	{
		c = line.data()[i];
		if(c == '=') target = &key;
		else if(c != ' ' && c != '\t' && c != '#') *target += c;
	};
	//printf("name: %s, key: %s\n", name.data(), key.data());
	mKeyMap[_name].insert(STRINGENTRY(name, key));
}

//****************************************************************
string* DRIni::getStr(const char *section, const char *key)
{
	mTemp = mKeyMap[section][key];
	return &mTemp;
}

//------------------------------------------------------------------
int DRIni::getInt(const char *section, const char *key)
{
	string* tStr = getStr(section, key);
	if(tStr->empty()) return 0;
	return atoi(tStr->data());
}
//-----------------------------------------------------------------------------------------------------------

DRVector3 DRIni::getVector3(const char* section, const char* key)
{
	string* tStr = getStr(section, key);
	if(tStr->empty()) return DRVector3(-1.0f);
	DRVector3 vec;
	sscanf(tStr->data(), "%f, %f, %f", &vec.x, &vec.y, &vec.z);
	return vec;
}
//-----------------------------------------------------------------------------------------------------------

DRReal DRIni::getReal(const char* section, const char* key)
{
	string* tStr = getStr(section, key);
	if(tStr->empty()) return 0.0f;
	return atof(tStr->data());
}
//-----------------------------------------------------------------------------------------------------------
DRVector2 DRIni::getVector2(const char* section, const char* key)
{
	string* tStr = getStr(section, key);
	if(tStr->empty()) return DRVector2(-1.0f);
	DRVector2 vec;
	sscanf(tStr->data(), "%f, %f", &vec.x, &vec.y);
	return vec;
}
//-----------------------------------------------------------------------------------------------------------
DRColor DRIni::getColor(const char* section, const char* key)
{
	string* tStr = getStr(section, key);
	if(tStr->empty()) return DRColor(0.0f);
	DRColor farbe;
	sscanf(tStr->data(), "%f, %f, %f, %f", &farbe.r, &farbe.g, &farbe.b, &farbe.a);
	return farbe;
}
///***********************************************************************************************************
