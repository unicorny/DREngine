
#include "Core2/Core2Main.h"

class IniTest
{
 public:
    explicit IniTest(const char* filename);
    ~IniTest();

    bool test(bool verbose = false);
 private:
    DRIni* mIni;

    bool testString(const char* section, const char* key, const char* value, bool verbose = false);
    void message(const char* nachricht, bool verbose = false);
};
