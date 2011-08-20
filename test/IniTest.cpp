#include "IniTest.h"

IniTest::IniTest(const char* filename)
: mIni(NULL)
{
    mIni = new DRIni(filename);
    if(!mIni->isValid())
    {
        DR_SAVE_DELETE(mIni);
    }

}

IniTest::~IniTest()
{
    DR_SAVE_DELETE(mIni);
}

bool IniTest::test(bool verbose)
{
    if(!mIni)
    {
        message("Die ini Datei konnte nicht geoffnet werden!", verbose);
        return false;
    }
    if(!testString("Test1", "Section", "Section1", verbose)) return false;
    if(mIni->getInt("Test1", "Zahl") == 2)
    {
        message("Der Wert von [Test1] Zahl ist korrekt", verbose);
    }
    else
    {
        message("Der Wert von [Test1] Zahl ist nicht korrekt", verbose);
        return false;
    }
    if(!testString("nichts", "Section2", "sdkdasd", verbose)) return false;
    if(!testString("nichts", "Section", "jkdasdasa2", verbose)) return false;




    return true;
}

bool IniTest::testString(const char* section, const char* key, const char* value, bool verbose)
{
    char buffer[256];
    if((*mIni->getStr(section, key)) == std::string(value))
    {
        sprintf(buffer, "Der Wert von [%s] %s ist korrekt", section, key);
        message(buffer, verbose);
        return true;
    }
    else
    {
        printf("%s == %s\n", std::string(value).data(), mIni->getStr(section, key)->data());
        sprintf(buffer, "Der Wert von [%s] %s ist nicht korrekt", section, key);
        message(buffer, verbose);
        return false;
    }
    return false;
}

void IniTest::message(const char* nachricht, bool verbose)
{
    if(verbose)
    {
        printf("IniTest: %s\n", nachricht);
    }
}
