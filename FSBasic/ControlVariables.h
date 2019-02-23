#if !defined(CONTROLVARIABLES_H)
#define CONTROLVARIABLES_H

#include "TString.h"

using namespace std;


class ControlVariables{

  public:

    static bool CHAINCACHING;       // turn on or off chain caching
    static bool FILECACHING;        // turn on or off file caching
    static bool HISTOGRAMCACHING;   // turn on or off histogram caching
    static bool DEBUG;              // verbose prints for debugging
    static bool QUIET;              // turns off almost all printing

    static TString CHAINFRIEND;     // add a friend when creating a TChain

};



#endif

