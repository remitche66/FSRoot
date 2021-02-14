#include <iostream>
#include <vector>
#include <utility>
#include "TSystem.h"
#include "FSBasic/FSString.h"
#include "FSAmpTools/FSAmpTools.h"


void
FSAmpTools::readAmplitudes(TString configFile){
  cout << configFile << endl;
  system("ls");
  system("mkdir temptemp");
  system("$FSROOT/FSAmpTools/AmpToolsExternalTemp/FSAmpTools/FSAmpToolsExe/FSAmpToolsReadAmplitudes "+configFile+" tempAmplitudes.txt");
}


//setenv DYLD_LIBRARY_PATH $DYLD_LIBRARY_PATH\:$ROOTSYS/lib