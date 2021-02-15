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
vector<TString> FSAmpTools::m_setNames;
map<TString, TString> FSAmpTools::m_ampSetMap;



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
  if (m_ampNames.size() > 1){
    for (unsigned int i = 0; i < m_ampNames.size()-1; i++){
    for (unsigned int j = i+1; j < m_ampNames.size(); j++){
      if (m_ampNames[j] < m_ampNames[i]){
        TString tempName = m_ampNames[j];
        m_ampNames[j] = m_ampNames[i];
        m_ampNames[i] = tempName;
      }
    }}
  }
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
  if (show) showAmplitudeNames(amplitudeNameLogic);
  return ampNames;
}

void
FSAmpTools::showAmplitudeNames(TString amplitudeNameLogic){
  vector<TString> ampNames = getAmplitudeNames(amplitudeNameLogic);
  cout << "    AMPLITUDES:" << endl;
  for (unsigned int i = 0; i < ampNames.size(); i++){
    cout << "      (AMP " << i+1 << ")  " << ampNames[i] << endl;
  }
}

void
FSAmpTools::clearAmplitudeNames(TString amplitudeNameLogic, bool show){
  if (amplitudeNameLogic == "") amplitudeNameLogic = "*";
  amplitudeNameLogic = "!("+amplitudeNameLogic+")";
  m_ampNames = getAmplitudeNames(amplitudeNameLogic);
  if (show) showAmplitudeNames();
}


void
FSAmpTools::defineAmplitudeSet(TString setName, TString amplitudeNameLogic, bool show){
  if (setName == "") return;
  if (amplitudeNameLogic == "") amplitudeNameLogic = "*";
  bool found = false;
  for (unsigned int j = 0; j < m_setNames.size(); j++){
    if (setName == m_setNames[j]){
      m_ampSetMap[setName] = amplitudeNameLogic;
      found = true; break;
    }
  }
  if (!found){
    m_setNames.push_back(setName);
    m_ampSetMap[setName] = amplitudeNameLogic;
  }
  if (m_setNames.size() > 1){
    for (unsigned int i = 0; i < m_setNames.size()-1; i++){
    for (unsigned int j = i+1; j < m_setNames.size(); j++){
      if (m_setNames[j] < m_setNames[i]){
        TString tempName = m_setNames[j];
        m_setNames[j] = m_setNames[i];
        m_setNames[i] = tempName;
      }
    }}
  }
  if (show) showAmplitudeSets();
}


vector<TString> 
FSAmpTools::getAmplitudeSets(TString setNameLogic, bool show){
  if (setNameLogic == "") setNameLogic = "*";
  vector<TString> setNames;
  for (unsigned int i = 0; i < m_setNames.size(); i++){
    vector<TString> categories;  categories.push_back(m_setNames[i]);
    if (FSString::evalLogicalTString(setNameLogic,categories)) setNames.push_back(m_setNames[i]);
  }
  if (show) showAmplitudeSets(setNameLogic);
  return setNames;
}


 
vector<TString>
FSAmpTools::getAmplitudeNamesBySet(TString setNameLogic, bool show){
  if (setNameLogic == "") setNameLogic = "*";
  vector<TString> setNames = getAmplitudeSets(setNameLogic);
  if (setNames.size() == 0){
    return getAmplitudeNames("!*",show);
  }
  TString ampNameLogic = "";
  for (unsigned int i = 0; i < setNames.size(); i++){
    TString ampLogicI = m_ampSetMap[setNames[i]];
    if (ampLogicI == "") ampLogicI = "*";
    ampLogicI = "("+ampLogicI+")";
    if (ampNameLogic != "") ampNameLogic += "||";
    ampNameLogic += ampLogicI;
  }
  return getAmplitudeNames(ampNameLogic,show);
}


void
FSAmpTools::showAmplitudeSets(TString setNameLogic){
  vector<TString> setNames = getAmplitudeSets(setNameLogic);
  cout << "AMPLITUDE SETS:" << endl;
  for (unsigned int i = 0; i < setNames.size(); i++){
    cout << "  (SET " << i+1 << ")  " << setNames[i] << "  " << m_ampSetMap[setNames[i]] << endl;
    showAmplitudeNames(m_ampSetMap[setNames[i]]);
  }
}

void
FSAmpTools::clearAmplitudeSets(TString setNameLogic, bool show){
  if (setNameLogic == "") setNameLogic = "*";
  setNameLogic = "!("+setNameLogic+")";
  m_setNames = getAmplitudeSets(setNameLogic);
  if (m_setNames.size() == 0) m_ampSetMap.clear();
  if (show) showAmplitudeSets();
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
