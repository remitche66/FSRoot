#include <iostream>
#include <vector>
#include <utility>
#include "TSystem.h"
#include "FSBasic/FSString.h"
#include "FSAmpTools/FSAmpTools.h"


void
FSAmpTools::readAmplitudes(TString configFile){
  cout << "looking at config file: " << configFile << endl;
  //system("$FSROOT/FSAmpTools/AmpToolsExternalTemp/FSAmpTools/FSAmpToolsExe/FSAmpToolsReadAmplitudes "+configFile+" tempAmplitudes.txt");
  ConfigFileParser parser(FSString::TString2string(configFile));
  ConfigurationInfo* cfg = parser.getConfigurationInfo();
  cfg->display();
}

