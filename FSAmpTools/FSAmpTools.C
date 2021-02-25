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
#include "FSAmpToolsAmp/BreitWignerNils.h"
#include "FSAmpToolsAmp/flexAmp.h"
#include "FSAmpToolsAmp/Zlm.h"


  // static member data
ConfigurationInfo* FSAmpTools::m_configInfo = NULL;
AmpToolsInterface* FSAmpTools::m_ATI = NULL;
vector<TString> FSAmpTools::m_ampNames;
vector<TString> FSAmpTools::m_rctNames;
vector<TString> FSAmpTools::m_ampWtNames;
map<TString, TString> FSAmpTools::m_ampWtMap;
bool FSAmpTools::m_parametersFromFit = false;
TString FSAmpTools::m_loadedATIReaction = "";

void
FSAmpTools::setupFromFitResults(TString fitResultsFile){
  AmpToolsInterface::registerAmplitude(BreitWigner());
  AmpToolsInterface::registerAmplitude(BreitWignerNils());
  AmpToolsInterface::registerAmplitude(flexAmp());
  AmpToolsInterface::registerAmplitude(Zlm());
  AmpToolsInterface::registerDataReader(FSAmpToolsDataReader());
  m_parametersFromFit = true;
  fitResultsFile = FSSystem::makeAbsolutePathName(fitResultsFile);
  FitResults* fitResults = new FitResults(FSString::TString2string(fitResultsFile));
  if ((!fitResults) || (!fitResults->valid())){
    cout << "Problem with the fit results file -- skipping" << endl;
    return;
  }
    // (owned by FitResults -- so this won't work after "delete fitResults" below)
  //ConfigurationInfo* m_configInfo = const_cast<ConfigurationInfo*>( fitResults->configInfo() );
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
  setAmpNamesFromConfigFile();
  setRctNamesFromConfigFile();
  if (m_ATI) delete m_ATI;
  m_loadedATIReaction = "";
}


void
FSAmpTools::setupFromConfigFile(TString configFile){
  AmpToolsInterface::registerAmplitude(BreitWigner());
  AmpToolsInterface::registerAmplitude(BreitWignerNils());
  AmpToolsInterface::registerAmplitude(flexAmp());
  AmpToolsInterface::registerAmplitude(Zlm());
  AmpToolsInterface::registerDataReader(FSAmpToolsDataReader());
  m_parametersFromFit = false;
  configFile = FSSystem::makeAbsolutePathName(configFile);
  ConfigFileParser::setVerboseParsing(false);
  ConfigFileParser parser(FSString::TString2string(configFile));
  m_configInfo = parser.getConfigurationInfo();
  setAmpNamesFromConfigFile();
  setRctNamesFromConfigFile();
  if (m_ATI) delete m_ATI;
  m_loadedATIReaction = "";
}

void
FSAmpTools::showConfigInfo(){
  if (!m_configInfo){
    cout << "FSAmpTools::showConfigInfo:  ConfigurationInfo object not found" << endl;
    return;
  }
  m_configInfo->display();
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


void
FSAmpTools::setRctNamesFromConfigFile(){
  m_rctNames.clear();
  vector<ReactionInfo*> rcts = m_configInfo->reactionList(); 
  for (unsigned int i = 0; i < rcts.size(); i++){
    bool found = false;
    for (unsigned int j = 0; j < m_rctNames.size(); j++){
      if (rcts[i]->reactionName() == m_rctNames[j]){ found = true; break; }
    }
    if (!found) m_rctNames.push_back(rcts[i]->reactionName());
  }
  if (m_rctNames.size() > 1){
    for (unsigned int i = 0; i < m_rctNames.size()-1; i++){
    for (unsigned int j = i+1; j < m_rctNames.size(); j++){
      if (m_rctNames[j] < m_rctNames[i]){
        TString tempName = m_rctNames[j];
        m_rctNames[j] = m_rctNames[i];
        m_rctNames[i] = tempName;
      }
    }}
  }
}


vector<TString>
FSAmpTools::getAmpNames(TString ampNameLogic, TString rctNameLogic, bool show){
  if (ampNameLogic == "") ampNameLogic = "*";
  if (rctNameLogic == "") rctNameLogic = "*";
  TString bothNameLogic = "("+ampNameLogic+")&&("+rctNameLogic+"::*)";
  vector<TString> ampNames;
  for (unsigned int i = 0; i < m_ampNames.size(); i++){
    vector<TString> categories;  categories.push_back(m_ampNames[i]);
    if (FSString::evalLogicalTString(bothNameLogic,categories))
      ampNames.push_back(m_ampNames[i]);
  }
  if (show){
    cout << "    AMPLITUDES:" << endl;
    for (unsigned int i = 0; i < ampNames.size(); i++){
      cout << "      (AMP " << i+1 << ")  " << ampNames[i] << endl;
    }
  }
  return ampNames;
}


void
FSAmpTools::showAmpNames(TString ampNameLogic, TString rctNameLogic){
  getAmpNames(ampNameLogic, rctNameLogic, true);
}


vector<TString>
FSAmpTools::getRctNames(TString rctNameLogic, bool show){
  if (rctNameLogic == "") rctNameLogic = "*";
  vector<TString> rctNames;
  for (unsigned int i = 0; i < m_rctNames.size(); i++){
    vector<TString> categories;  categories.push_back(m_rctNames[i]);
    if (FSString::evalLogicalTString(rctNameLogic,categories))
      rctNames.push_back(m_rctNames[i]);
  }
  if (show){
    cout << "    REACTIONS:" << endl;
    for (unsigned int i = 0; i < rctNames.size(); i++){
      cout << "      (RCT " << i+1 << ")  " << rctNames[i] << endl;
    }
  }
  return rctNames;
}


void
FSAmpTools::showRctNames(TString rctNameLogic){
  getRctNames(rctNameLogic, true);
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
  if (show) getAmpWtNames("*","*",-1,true);
}


vector<TString> 
FSAmpTools::getAmpWtNames(TString ampWtNameLogic, TString rctNameLogic, int iEvent, bool show){
  if (ampWtNameLogic == "") ampWtNameLogic = "*";
  if (rctNameLogic == "") rctNameLogic = "*";
  vector<TString> ampWtNames1;
  for (unsigned int i = 0; i < m_ampWtNames.size(); i++){
    vector<TString> categories;  categories.push_back(m_ampWtNames[i]);
    if (FSString::evalLogicalTString(ampWtNameLogic,categories)) 
      ampWtNames1.push_back(m_ampWtNames[i]);
  }
  vector<TString> ampWtNames2;
  for (unsigned int i = 0; i < ampWtNames1.size(); i++){
    TString ampNameLogic = m_ampWtMap[ampWtNames1[i]];
    vector<TString> ampNames = getAmpNames(ampNameLogic, rctNameLogic);
    if (ampNames.size() != 0) ampWtNames2.push_back(ampWtNames1[i]);
  }
  if (show){
    cout << "SETS OF AMPLITUDES FOR WEIGHTS:" << endl;
    for (unsigned int i = 0; i < ampWtNames2.size(); i++){
      cout << "  (SET " << i+1 << ")  " << ampWtNames2[i] << "  " << m_ampWtMap[ampWtNames2[i]] << endl;
      showAmpNames(m_ampWtMap[ampWtNames2[i]],rctNameLogic);
      if (iEvent >= 0) cout << "      VALUE = " << getAmpWt(ampWtNames2[i],iEvent) << endl;
    }
  }
  return ampWtNames2;
}


void
FSAmpTools::showAmpWts(TString ampWtNameLogic, TString rctNameLogic, int iEvent){
  getAmpWtNames(ampWtNameLogic,rctNameLogic,iEvent,true);
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
FSAmpTools::makeAmpWts(TString fileName, TString treeName, TString reactionName, 
                       int numParticles, double numGeneratedMCEvents){

    // (0) set up an AmpToolsInterface
  if (!m_configInfo){
    cout << "no ConfigurationInfo object:  use setupFromConfigFile or setupFromFitResults" << endl;
    return;
  }
  if (m_ATI) delete m_ATI;
  m_ATI = new AmpToolsInterface(m_configInfo);
  m_loadedATIReaction = "";

    // (1) set up a DataReader
  fileName = FSSystem::makeAbsolutePathName(fileName);
    // FIX: loop over filenames
  vector<string> args;
  args.push_back(FSString::TString2string(fileName));
  args.push_back(FSString::TString2string(treeName));
  args.push_back(FSString::TString2string(FSString::int2TString(numParticles)));
    // FIX: search for an existing one before making a new one
  cout << "makeAmpWts: setting up data reader" << endl;
  FSAmpToolsDataReader* dataReader = new FSAmpToolsDataReader(args);
  cout << "makeAmpWts: data reader events = " << dataReader->numEvents() << endl;

    // (2) set up the AmpToolsInterface
  cout << "makeAmpWts: clearEvents in AmpToolsInterface" << endl;
  m_ATI->clearEvents();
  cout << "makeAmpWts: loadEvents in AmpToolsInterface" << endl;
  m_ATI->loadEvents(dataReader);
  cout << "makeAmpWts: AmpToolsInterface events = " << m_ATI->numEvents() << endl;
  cout << "makeAmpWts: processEvents in AmpToolsInterface" << endl;
  double maxIntensity = m_ATI->processEvents(FSString::TString2string(reactionName));
  cout << "makeAmpWts: maxIntensity = " << maxIntensity << endl;
  int numEvents = m_ATI->numEvents();
  double wtScale = 1.0;
  if (!m_parametersFromFit){
    double totIntensity = 0.0;
    for (int i = 0; i < numEvents; i++){
      totIntensity += m_ATI->intensity(i);
    }
    if (totIntensity <= 0.0){ cout << "Total intensity <= 0??  Skipping." << endl; return; }
    if (numGeneratedMCEvents <= 0) numGeneratedMCEvents = numEvents;
    wtScale = numGeneratedMCEvents/totIntensity;
  }
  if (m_parametersFromFit){
    if (numGeneratedMCEvents > 0) wtScale = 1.0/numGeneratedMCEvents;
  }

    // (3) set up the lists of amplitude names
  vector< pair< pair<TString,TString>, vector< vector<string> > > > 
    vpairsWts = groupAmpsForWts(reactionName,"*");
  cout << "makeAmpWts: calculate these weights..." << endl;
  showAmpWts("*", reactionName);

    // (4) set up a friend tree
  FSTree::addFriendTree("AmpWts");
  TString fileName_wt(fileName);  fileName_wt += ".AmpWts";
  TString treeName_wt(treeName);  treeName_wt += "_AmpWts";
  TFile* wtTFile = new TFile(fileName_wt,"recreate");  wtTFile->cd();
  TTree* wtTTree = new TTree(treeName_wt, treeName_wt);
  Double_t WTS[1000];
  for (unsigned int i = 0; i < vpairsWts.size(); i++){
    TString ampWtName = vpairsWts[i].first.first;
    wtTTree->Branch(ampWtName, &WTS[i], ampWtName+"/D");
  }

    // (5) loop over events and fill the friend tree
  cout << "makeAmpWts: filling the AmpWts friend tree" << endl;
  for (int iEvent = 0; iEvent < numEvents; iEvent++){
    if (iEvent%10000 == 0) cout << "makeAmpWts: event " << iEvent << endl;
    for (unsigned int i = 0; i < vpairsWts.size(); i++){
      if (vpairsWts[i].first.second == "IN") WTS[i] = calcINFromATI(iEvent, vpairsWts[i].second);
      if (vpairsWts[i].first.second == "RE") WTS[i] = calcREFromATI(iEvent, vpairsWts[i].second[0]);
      if (vpairsWts[i].first.second == "IM") WTS[i] = calcIMFromATI(iEvent, vpairsWts[i].second[0]);
      if (vpairsWts[i].first.second == "PH") WTS[i] = calcPHFromATI(iEvent, vpairsWts[i].second[0]);
      WTS[i] *= wtScale;
    }
    wtTTree->Fill();
  }

    // (6) write the friend tree
  cout << "makeAmpWts: writing the friend tree" << endl;
  wtTFile->cd();
  wtTTree->Write();
  delete wtTFile;
  gDirectory->cd();
  m_loadedATIReaction = reactionName;
  cout << "makeAmpWts: finished" << endl;
}



double
FSAmpTools::getAmpWt(TString ampWtName, int iEvent){
  if (!m_ATI){ cout << "getAmpWt: No ATI" << endl; return 0.0; }
  if (m_loadedATIReaction == ""){ cout << "getAmpWt: No loaded reaction" << endl; return 0.0; }
  vector< pair< pair<TString,TString>, vector< vector<string> > > > 
    vpairsWts = groupAmpsForWts(m_loadedATIReaction, ampWtName);
  if (vpairsWts.size() != 1){
    cout << "Can't find ampWt with name: " << ampWtName << " ... skipping ..." << endl;
    return 0.0;
  }
  if (vpairsWts[0].first.second == "IN") return calcINFromATI(iEvent, vpairsWts[0].second);
  if (vpairsWts[0].first.second == "RE") return calcREFromATI(iEvent, vpairsWts[0].second[0]);
  if (vpairsWts[0].first.second == "IM") return calcIMFromATI(iEvent, vpairsWts[0].second[0]);
  if (vpairsWts[0].first.second == "PH") return calcPHFromATI(iEvent, vpairsWts[0].second[0]);
  return 0.0;
}



  // vector< pair< pair<ampWtName,ampWtType>, vector< vector<ampName> > > > 
vector< pair< pair<TString,TString>, vector< vector<string> > > > 
FSAmpTools::groupAmpsForWts(TString reactionName, TString ampWtNameLogic){
  vector< pair< pair<TString,TString>, vector< vector<string> > > > vpairsWts;
  vector<TString> usedAmpWtNames;
  vector<TString> allAmpWtNames = getAmpWtNames(ampWtNameLogic, reactionName);
  for (unsigned int i = 0; i < allAmpWtNames.size(); i++){
    TString wtType = FSString::subString(allAmpWtNames[i],0,2);
      // only use amplitudes from this reaction
    vector<TString> ampNames = getAmpNames(m_ampWtMap[allAmpWtNames[i]],reactionName);
    if (ampNames.size() == 0) continue;
      // sort the amplitudes into sums and do some checks
    vector< vector<TString> > sortedAmps = sortAmpsIntoSums(ampNames);
    if (((wtType == "RE") || (wtType == "IM") || (wtType == "PH")) && sortedAmps.size() != 1){
      cout << "Can't use RE/IM/PH with amps from different sums -- skipping " 
           << allAmpWtNames[i] << endl; continue; }
      // pack everything up to use in the loop over events
    usedAmpWtNames.push_back(allAmpWtNames[i]);
    vector< vector<string> > sortedAmpsStrings = vvTString2string(sortedAmps);
    vpairsWts.push_back(pair< pair<TString,TString>, vector< vector<string> > >
      (pair<TString,TString>(allAmpWtNames[i],wtType),sortedAmpsStrings));
  }
  if (usedAmpWtNames.size() == 0){
    cout << "No weights to calculate -- skipping" << endl; vpairsWts.clear(); return vpairsWts;
  }
  if (usedAmpWtNames.size() > 1000){
    cout << "Current limit is 1000 weights -- skipping" << endl; vpairsWts.clear(); return vpairsWts;
  }
  return vpairsWts;
}


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
    AMP += m_ATI->scaledProductionAmplitude(ampNames[i]) *
           m_ATI->decayAmplitude(iEvent,ampNames[i]);
  }
  return real(AMP);
}

double
FSAmpTools::calcIMFromATI(int iEvent, const vector<string>& ampNames){
  complex<double> AMP(0.0,0.0);
  for (unsigned int i = 0; i < ampNames.size(); i++){
    AMP += m_ATI->scaledProductionAmplitude(ampNames[i]) *
           m_ATI->decayAmplitude(iEvent,ampNames[i]);
  }
  return imag(AMP);
}

double
FSAmpTools::calcPHFromATI(int iEvent, const vector<string>& ampNames){
  complex<double> AMP(0.0,0.0);
  for (unsigned int i = 0; i < ampNames.size(); i++){
    AMP += m_ATI->scaledProductionAmplitude(ampNames[i]) *
           m_ATI->decayAmplitude(iEvent,ampNames[i]);
  }
  return arg(AMP);
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

