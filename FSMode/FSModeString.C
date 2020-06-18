#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "TString.h"
#include "FSBasic/FSString.h"
#include "FSMode/FSModeString.h"


  // ********************************************************
  // CONVERT SYMBOLS TO ROOT FORMAT (e.g. "pi+" TO "#pi^{+}")
  // ********************************************************

TString
FSModeString::rootSymbols(TString input){ 
  return FSString::rootSymbols(input);
}


  // ********************************************************
  // CONVERT SYMBOLS TO LATEX FORMAT (e.g. "pi+" TO "\pi^{+}")
  // ********************************************************

TString
FSModeString::latexSymbols(TString input){ 
  return FSString::latexSymbols(input);
}

