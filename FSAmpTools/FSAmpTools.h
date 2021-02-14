#if !defined(FSAMPTOOLS_H)
#define FSAMPTOOLS_H

#include <cmath>
#include "TString.h"
//#include "$AMPTOOLS/IUAmpTools/ConfigFileParser.h"
//R__ADD_LIBRARY_PATH($AMPTOOLS/lib) // if needed
//R__LOAD_LIBRARY($AMPTOOLS/lib/libIUAmpTools.a)

using namespace std;


class FSAmpTools{

  public:

    static void readAmplitudes(TString configFile);

};


#endif
