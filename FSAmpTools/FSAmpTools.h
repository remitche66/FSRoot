#if !defined(FSAMPTOOLS_H)
#define FSAMPTOOLS_H

#include <cmath>
#include "TString.h"
#include "IUAmpTools/ConfigFileParser.h"

using namespace std;


class FSAmpTools{

  public:

    static void readAmplitudes(TString configFile);

};


#endif
