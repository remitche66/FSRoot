#include <iostream>
#include <vector>
#include <utility>
#include "TSystem.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSSystem.h"
#include "FSAmpTools/FSAmpTools.h"
#include "TGenPhaseSpace.h"
#include "TLorentzVector.h"


void
FSAmpTools::readAmplitudes(TString configFile){
  cout << "looking at config file: " << configFile << endl;
  //system("$FSROOT/FSAmpTools/AmpToolsExternalTemp/FSAmpTools/FSAmpToolsExe/FSAmpToolsReadAmplitudes "+configFile+" tempAmplitudes.txt");
  configFile = FSSystem::makeAbsolutePathName(configFile);
  ConfigFileParser parser(FSString::TString2string(configFile));
  ConfigurationInfo* cfg = parser.getConfigurationInfo();
  cfg->display();
}

void
FSAmpTools::generatePhaseSpace(TString outFile, int numEvents){



    // ************************
    // set up a FSAmpToolsDataWriter object
    // ************************

  cout << "Creating a Data Writer..." << endl;

  FSAmpToolsDataWriter dataWriter(3, FSString::TString2string(outFile));

  cout << "... Finished creating a Data Writer" << endl << endl;


    // ************************
    // use ROOT to generate phase space
    // ************************

  TGenPhaseSpace generator;
  TLorentzVector parent(0.0, 0.0, 0.0, 3.0);
  double daughterMasses[3] = {0.2, 0.2, 0.2};
  generator.SetDecay(parent, 3, daughterMasses);
  double maxWeight = generator.GetWtMax();


    // ************************
    // use the FSAmpToolsDataWriter object to write events to a file
    // ************************

  for (int i = 0; i < numEvents; ++i){


      // generate the decay

    double weight = generator.Generate();
    if (weight < drand48() * maxWeight){
      i--;  continue;
    }


      // pack the decay products into a Kinematics object

    vector<TLorentzVector> fourvectors;
    fourvectors.push_back( TLorentzVector( *generator.GetDecay(0) ) );
    fourvectors.push_back( TLorentzVector( *generator.GetDecay(1) ) );
    fourvectors.push_back( TLorentzVector( *generator.GetDecay(2) ) );
    Kinematics kin(fourvectors);


      // write to a file

    dataWriter.writeEvent(kin);

    if (dataWriter.eventCounter() % 1000 == 0)
      cout << "Event counter = " << dataWriter.eventCounter() << endl;

  }


}
