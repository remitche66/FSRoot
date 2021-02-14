#include <iostream>
#include <fstream>
#include <string>
#include "IUAmpTools/ConfigFileParser.h"
#include "IUAmpTools/ConfigurationInfo.h"
#include "TString.h"
#include "TH1F.h"
#include "TFile.h"
#include "FSBasic/FSString.h"

using namespace std;

int main(int argc, char** argv){
  string configfile(argv[1]);
  string outfile(argv[2]);
  ConfigFileParser::setVerboseParsing(false);
  ConfigFileParser parser(configfile);
  ConfigurationInfo* configInfo = parser.getConfigurationInfo();
  vector<AmplitudeInfo*> amps = configInfo->amplitudeList(); 
  ofstream outstream(outfile.c_str());
  if (!outstream){
    cout << "problem opening file" << endl;
    exit(0);
  }
  for (unsigned int i = 0; i < amps.size(); i++){
    outstream << "FSAMP:  " << amps[i]->fullName() << "  "  << real(amps[i]->value()) << "  " << imag(amps[i]->value()) << endl;
  }
  outstream.close();
}


