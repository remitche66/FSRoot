#include <iostream>
#include <vector>
#include <utility>
#include "TSystem.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSSystem.h"
#include "FSAmpTools/FSAmpTools.h"
#include "TGenPhaseSpace.h"
#include "TLorentzVector.h"

  // static member data
vector<TString> FSAmpTools::m_ampNames;



void
FSAmpTools::readAmplitudeNames(TString configFile, bool show){
  configFile = FSSystem::makeAbsolutePathName(configFile);
  ConfigFileParser::setVerboseParsing(false);
  ConfigFileParser parser(FSString::TString2string(configFile));
  ConfigurationInfo* cfg = parser.getConfigurationInfo();
  //cfg->display();
  vector<AmplitudeInfo*> amps = cfg->amplitudeList(); 
  for (unsigned int i = 0; i < amps.size(); i++){
    //cout << "FSAMP:  " << amps[i]->fullName() << "  "  << real(amps[i]->value()) << "  " << imag(amps[i]->value()) << endl;
    bool found = false;
    for (unsigned int j = 0; j < m_ampNames.size(); j++){
      if (amps[i]->fullName() == m_ampNames[j]){ found = true; break; }
    }
    if (!found) m_ampNames.push_back(amps[i]->fullName());
  }
  if (m_ampNames.size() <= 1) return;
  for (unsigned int i = 0; i < m_ampNames.size()-1; i++){
  for (unsigned int j = i+1; j < m_ampNames.size(); j++){
    if (m_ampNames[j] < m_ampNames[i]){
      TString tempName = m_ampNames[j];
      m_ampNames[j] = m_ampNames[i];
      m_ampNames[i] = tempName;
    }
  }}
  if (show) showAmplitudeNames();
}

vector<TString> 
FSAmpTools::getAmplitudeNames(TString amplitudeNameLogic, bool show){
  if (amplitudeNameLogic == "") amplitudeNameLogic = "*";
  vector<TString> ampNames;
  for (unsigned int i = 0; i < m_ampNames.size(); i++){
    vector<TString> categories;  categories.push_back(m_ampNames[i]);
    if (FSString::evalLogicalTString(amplitudeNameLogic,categories)) ampNames.push_back(m_ampNames[i]);
  }
  if (show) showAmplitudeNames();
  return ampNames;
}

void
FSAmpTools::showAmplitudeNames(TString amplitudeNameLogic){
  vector<TString> ampNames = getAmplitudeNames(amplitudeNameLogic);
  cout << "AMPLITUDES:" << endl;
  for (unsigned int i = 0; i < ampNames.size(); i++){
    cout << "(" << i+1 << ")  " << ampNames[i] << endl;
  }
}

void
FSAmpTools::clearAmplitudeNames(TString amplitudeNameLogic, bool show){
  amplitudeNameLogic = "!("+amplitudeNameLogic+")";
  m_ampNames = getAmplitudeNames(amplitudeNameLogic);
  if (show) showAmplitudeNames();
}


void
FSAmpTools::testSystem(TString configFile){
  system("$FSAMPTOOLS/FSAmpToolsExe/FSAmpToolsReadAmplitudes "+configFile+" tempAmplitudes.txt");
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
