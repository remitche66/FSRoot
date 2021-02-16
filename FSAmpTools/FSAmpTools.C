#include <iostream>
#include <vector>
#include <utility>
#include "TSystem.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSSystem.h"
#include "FSBasic/FSTree.h"
#include "FSAmpTools/FSAmpTools.h"
#include "TGenPhaseSpace.h"
#include "TLorentzVector.h"
#include "FSAmpToolsDataIO/FSAmpToolsDataReader.h"
#include "FSAmpToolsAmp/BreitWigner.h"


  // static member data
ConfigurationInfo* FSAmpTools::m_configInfo;
AmpToolsInterface* FSAmpTools::m_ATI;
vector<TString> FSAmpTools::m_ampNames;
vector<TString> FSAmpTools::m_ampWtNames;
map<TString, TString> FSAmpTools::m_ampWtMap;


void
FSAmpTools::setupFromFitResults(TString fitResultsFile){
  AmpToolsInterface::registerAmplitude(BreitWigner());
  AmpToolsInterface::registerDataReader(FSAmpToolsDataReader());
  fitResultsFile = FSSystem::makeAbsolutePathName(fitResultsFile);
  FitResults* fitResults = new FitResults(FSString::TString2string(fitResultsFile));
  if ((!fitResults) || (!fitResults->valid())){
    cout << "Problem with the fit results file -- skipping" << endl;
    return;
  }
    // FIX (for obvious reasons)
  fitResults->configInfo()->write("xxxxxxxTempConfigFilexxxxxxx.cfg");
  ConfigFileParser parser("xxxxxxxTempConfigFilexxxxxxx.cfg");
  system("rm -f  xxxxxxxTempConfigFilexxxxxxx.cfg");
  m_configInfo = parser.getConfigurationInfo();
  cout << "INITIAL CONFIGURATION INFO:" << endl;  m_configInfo->display();
    // set parameters
  vector<ParameterInfo*> parameterList = m_configInfo->parameterList();
  for (unsigned int i = 0; i < parameterList.size(); i++){
    parameterList[i]->setValue(fitResults->parValue(parameterList[i]->parName()));
  }
    // set production amplitudes
  vector<AmplitudeInfo*> ampList = m_configInfo->amplitudeList(); 
  for (unsigned int i = 0; i < ampList.size(); i++){
    ampList[i]->setValue(fitResults->productionParameter(ampList[i]->fullName()));
  }
  cout << "MODIFIED CONFIGURATION INFO:" << endl;  m_configInfo->display();
  delete fitResults;
    // FIX in case m_ATI already exists
  m_ATI = new AmpToolsInterface(m_configInfo);
  setAmpNamesFromConfigFile();
}


void
FSAmpTools::setupFromConfigFile(TString configFile){
  AmpToolsInterface::registerAmplitude(BreitWigner());
  AmpToolsInterface::registerDataReader(FSAmpToolsDataReader());
  configFile = FSSystem::makeAbsolutePathName(configFile);
  ConfigFileParser::setVerboseParsing(false);
  ConfigFileParser parser(FSString::TString2string(configFile));
  m_configInfo = parser.getConfigurationInfo();
    // FIX in case m_ATI already exists
  m_ATI = new AmpToolsInterface(m_configInfo);
  setAmpNamesFromConfigFile();
}


void
FSAmpTools::setAmpNamesFromConfigFile(){
  m_ampNames.clear();
  vector<AmplitudeInfo*> amps = m_configInfo->amplitudeList(); 
  for (unsigned int i = 0; i < amps.size(); i++){
    //cout << "FSAMP:  " << amps[i]->fullName() << "  "  
    // << real(amps[i]->value()) << "  " << imag(amps[i]->value()) << endl;
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
}

vector<TString>
FSAmpTools::getAmpNames(vector<TString> ampNames, TString ampNameLogic, bool show){
  if (ampNameLogic == "") ampNameLogic = "*";
  vector<TString> ampNamesSubset;
  for (unsigned int i = 0; i < ampNames.size(); i++){
    vector<TString> categories;  categories.push_back(ampNames[i]);
    if (FSString::evalLogicalTString(ampNameLogic,categories)) ampNamesSubset.push_back(ampNames[i]);
  }
  if (show){
    cout << "    AMPLITUDES:" << endl;
    for (unsigned int i = 0; i < ampNamesSubset.size(); i++){
      cout << "      (AMP " << i+1 << ")  " << ampNamesSubset[i] << endl;
    }
  }
  return ampNamesSubset;
}


vector<TString> 
FSAmpTools::getAmpNames(TString ampNameLogic, bool show){
  return getAmpNames(m_ampNames,ampNameLogic,show);
}


void
FSAmpTools::showAmpNames(TString ampNameLogic){
  getAmpNames(ampNameLogic, true);
}


void
FSAmpTools::defineAmpWt(TString ampWtName, TString ampNameLogic, bool show){
  if (ampWtName.Length() < 3){ 
    cout << "Amplitude weight name too short -- skipping" << endl; return; }
  TString wtType = FSString::subString(ampWtName,0,2);
  if ((wtType != "RE") && (wtType != "IM") && (wtType != "PH") && (wtType != "IN")){
    cout << "Allowed types are RE, IM, PH, and IN -- skipping" << endl; return; }
  if (ampNameLogic == "") ampNameLogic = "*";
  bool found = false;
  for (unsigned int j = 0; j < m_ampWtNames.size(); j++){
    if (ampWtName == m_ampWtNames[j]){
      m_ampWtMap[ampWtName] = ampNameLogic;
      found = true; break;
    }
  }
  if (!found){
    m_ampWtNames.push_back(ampWtName);
    m_ampWtMap[ampWtName] = ampNameLogic;
  }
  if (m_ampWtNames.size() > 1){
    for (unsigned int i = 0; i < m_ampWtNames.size()-1; i++){
    for (unsigned int j = i+1; j < m_ampWtNames.size(); j++){
      if (m_ampWtNames[j] < m_ampWtNames[i]){
        TString tempName = m_ampWtNames[j];
        m_ampWtNames[j] = m_ampWtNames[i];
        m_ampWtNames[i] = tempName;
      }
    }}
  }
  if (show) getAmpWtNames("*",true);
}


vector<TString> 
FSAmpTools::getAmpWtNames(TString ampWtNameLogic, bool show){
  if (ampWtNameLogic == "") ampWtNameLogic = "*";
  vector<TString> ampWtNames;
  for (unsigned int i = 0; i < m_ampWtNames.size(); i++){
    vector<TString> categories;  categories.push_back(m_ampWtNames[i]);
    if (FSString::evalLogicalTString(ampWtNameLogic,categories)) 
      ampWtNames.push_back(m_ampWtNames[i]);
  }
  if (show){
    cout << "SETS OF AMPLITUDES FOR WEIGHTS:" << endl;
    for (unsigned int i = 0; i < ampWtNames.size(); i++){
      cout << "  (SET " << i+1 << ")  " << ampWtNames[i] << "  " << m_ampWtMap[ampWtNames[i]] << endl;
      showAmpNames(m_ampWtMap[ampWtNames[i]]);
    }
  }
  return ampWtNames;
}


void
FSAmpTools::showAmpWts(TString ampWtNameLogic){
  getAmpWtNames(ampWtNameLogic,true);
}

void
FSAmpTools::clearAmpWts(TString ampWtNameLogic, bool show){
  if (ampWtNameLogic == "") ampWtNameLogic = "*";
  ampWtNameLogic = "!("+ampWtNameLogic+")";
  m_ampWtNames = getAmpWtNames(ampWtNameLogic);
  if (m_ampWtNames.size() == 0) m_ampWtMap.clear();
  if (show) showAmpWts();
}


void
FSAmpTools::makeAmpWts(TString fileName, TString treeName, TString reactionName, int numParticles){
  fileName = FSSystem::makeAbsolutePathName(fileName);
    // FIX: loop over filenames
  vector<string> args;
  args.push_back(FSString::TString2string(fileName));
  args.push_back(FSString::TString2string(treeName));
  args.push_back(FSString::TString2string(FSString::int2TString(numParticles)));
    // FIX: search for an existing one before making a new one
  FSAmpToolsDataReader* dataReader = new FSAmpToolsDataReader(args);
  m_ATI->clearEvents();
  m_ATI->loadEvents(dataReader);
  double maxIntensity = m_ATI->processEvents(FSString::TString2string(reactionName));
  double totIntensity = 0.0;
  int numEvents = m_ATI->numEvents();
  for (int i = 0; i < numEvents; i++){
    totIntensity += m_ATI->intensity(i);
  }
  int dataEvents = dataReader->numEvents();
  double wtScale = 1.0;  if (totIntensity > 0) wtScale = dataEvents/totIntensity;

// need to know wtType, wtName

  vector< pair< TString, vector< vector<string> > > > vpairsWts;
  vector<TString> ampWtNames;
  for (unsigned int i = 0; i < m_ampWtNames.size(); i++){
    TString wtType = FSString::subString(m_ampWtNames[i],0,2);
    TString reactionNameLogic = reactionName+"::*";
    vector<TString> ampNames = getAmpNames(m_ampWtMap[m_ampWtNames[i]]);
    ampNames = getAmpNames(ampNames,reactionNameLogic);
    if (ampNames.size() == 0) continue;
    vector< vector<TString> > sortedAmps = sortAmpsIntoSums(ampNames);
    if (((wtType == "RE") || (wtType == "IM") || (wtType == "PH")) && sortedAmps.size() != 1){
      cout << "Can't use RE/IM/PH with amps from different sums -- skipping " 
           << m_ampWtNames[i] << endl; continue; }
    ampWtNames.push_back(m_ampWtNames[i]);
    vector< vector<string> > sortedAmpsStrings = vvTString2string(sortedAmps);
    vpairsWts.push_back(pair<TString, vector< vector<string> > >(wtType,sortedAmpsStrings));
  }

  if (ampWtNames.size() == 0){
    cout << "No weights to calculate -- skipping" << endl; return;
  }
  if (ampWtNames.size() > 100){
    cout << "Current limit is 100 weights -- skipping" << endl; return;
  }
    // set up a friend tree
  FSTree::addFriendTree("AmpWts");
  TString fileName_wt(fileName);  fileName_wt += ".AmpWts";
  TString treeName_wt(treeName);  treeName_wt += "_AmpWts";
  TFile* wtTFile = new TFile(fileName_wt,"recreate");  wtTFile->cd();
  TTree* wtTTree = new TTree(treeName_wt, treeName_wt);
  Double_t WTS[100];
  for (unsigned int i = 0; i < ampWtNames.size(); i++){
    wtTTree->Branch(ampWtNames[i], &WTS[i], ampWtNames[i]+"/D");
  }

    // fill the friend tree
  for (int iEvent = 0; iEvent < numEvents; iEvent++){
    for (unsigned int i = 0; i < ampWtNames.size(); i++){
      if (vpairsWts[i].first == "IN") WTS[i] = calcINFromATI(iEvent, vpairsWts[i].second);
      if (vpairsWts[i].first == "RE") WTS[i] = calcREFromATI(iEvent, vpairsWts[i].second[0]);
      if (vpairsWts[i].first == "IM") WTS[i] = calcIMFromATI(iEvent, vpairsWts[i].second[0]);
      WTS[i] *= wtScale;
    }
    wtTTree->Fill();
  }

    // write the friend tree
  wtTFile->cd();
  wtTTree->Write();
  delete wtTFile;
  gDirectory->cd();
}



//cout << "amplitudes for " << wtType << "  " << m_ampWtNames[i] << endl;

vector< vector<TString> >
FSAmpTools::sortAmpsIntoSums(vector<TString> ampNames){
  vector< vector<TString> > sortedAmps;
  for (unsigned int i = 0; i < ampNames.size(); i++){
    vector<TString> parts = FSString::parseTString(ampNames[i],"::");
    TString ampNameWild = parts[0]+"::"+parts[1]+"::*";
    bool found = false;
    for (unsigned int j = 0; j < sortedAmps.size(); j++){
      if (FSString::compareTStrings(sortedAmps[j][0],ampNameWild)){
        found = true;
        sortedAmps[j].push_back(ampNames[i]);
      }
    }
    if (!found){
      vector<TString> sortedAmp;
      sortedAmp.push_back(ampNames[i]);
      sortedAmps.push_back(sortedAmp);
    }
  }
  return sortedAmps;
}


double
FSAmpTools::calcINFromATI(int iEvent, const vector< vector<string> >& ampNames){
  double IN = 0.0;
  complex<double> INPart(0.0,0.0);
  for (unsigned int i = 0; i < ampNames.size(); i++){
    INPart = complex<double>(0.0,0.0);
    for (unsigned int j = 0; j < ampNames[i].size(); j++){
      complex<double> P = m_ATI->scaledProductionAmplitude(ampNames[i][j]);
      complex<double> D = m_ATI->decayAmplitude(iEvent,ampNames[i][j]);
      INPart += (P*D);
    }
    IN += norm(INPart);
  } 
  return IN;
}

double
FSAmpTools::calcREFromATI(int iEvent, const vector<string>& ampNames){
  complex<double> AMP(0.0,0.0);
  for (unsigned int i = 0; i < ampNames.size(); i++){
    AMP += m_ATI->decayAmplitude(iEvent,ampNames[i]);
  }
  return real(AMP);
}

double
FSAmpTools::calcIMFromATI(int iEvent, const vector<string>& ampNames){
  complex<double> AMP(0.0,0.0);
  for (unsigned int i = 0; i < ampNames.size(); i++){
    AMP += m_ATI->decayAmplitude(iEvent,ampNames[i]);
  }
  return imag(AMP);
}

vector<string>
FSAmpTools::vTString2string(vector<TString> vTStrings){
  vector<string> vStrings;
  for (unsigned int i = 0; i < vTStrings.size(); i++){
    vStrings.push_back(FSString::TString2string(vTStrings[i]));
  }
  return vStrings;
}

vector< vector<string> >
FSAmpTools::vvTString2string(vector< vector<TString> > vvTStrings){
  vector< vector<string> > vvStrings;
  for (unsigned int i = 0; i < vvTStrings.size(); i++){
    vvStrings.push_back(vTString2string(vvTStrings[i]));
  }
  return vvStrings;
}



/*
void
FSAmpTools::testSystem(TString configFile){
  system("$FSAMPTOOLS/FSAmpToolsExe/FSAmpToolsReadAmplitudes "+configFile+" tempAmplitudes.txt");
}
*/

