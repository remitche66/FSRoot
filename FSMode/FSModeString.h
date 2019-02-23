#if !defined(FSMODESTRING_H)
#define FSMODESTRING_H

#include <cmath>
#include <cstdlib>
#include <vector>
#include <utility>
#include <string>
#include <map>
#include "TString.h"

using namespace std;


class FSModeString{

  public:


      // ********************************************************
      // CONVERT SYMBOLS TO ROOT FORMAT (e.g. "pi+" TO "#pi^{+}")
      // ********************************************************

    static TString rootSymbols(TString input);


      // ********************************************************
      // CONVERT SYMBOLS TO LATEX FORMAT (e.g. "pi+" TO "\pi^{+}")
      // ********************************************************

    static TString latexSymbols(TString input);


};



#endif

