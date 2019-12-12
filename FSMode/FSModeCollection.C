#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include "TChain.h"
#include "TString.h"
#include "FSBasic/FSString.h"
#include "FSMode/FSModeInfo.h"
#include "FSMode/FSModeCollection.h"


  // static member data
map< pair<int,int>, FSModeInfo*> FSModeCollection::m_modeInfoMap;


  // *******************************************************
  // ADD MODES TO THE COLLECTION
  // *******************************************************

FSModeInfo*
FSModeCollection::addModeInfo(pair<int,int> mCode){
  pair<int,int> mCode2(mCode.second,mCode.first);
  if (modeInfo(mCode) == NULL) m_modeInfoMap[mCode2] = new FSModeInfo(mCode);
  return m_modeInfoMap[mCode2];
}

FSModeInfo*
FSModeCollection::addModeInfo(int mCode1, int mCode2){
  return addModeInfo(FSModeInfo(mCode1,mCode2).modeCode());
}

FSModeInfo*
FSModeCollection::addModeInfo(TString mString){
  FSModeInfo modeInfoTemp(mString);
  FSModeInfo* modeInfo = addModeInfo(modeInfoTemp.modeCode());
  modeInfo->addCategory(modeInfoTemp.categories());
  return modeInfo;
}


  // *******************************************************
  // REMOVE MODES FROM THE COLLECTION
  // *******************************************************

void
FSModeCollection::removeModeInfo(pair<int,int> mCode){
  FSModeInfo* mi = modeInfo(mCode);
  if (mi != NULL){
    delete mi;
    pair<int,int> mCode2(mCode.second,mCode.first);
    m_modeInfoMap.erase(mCode2);
  }
}

void
FSModeCollection::removeModeInfo(int mCode1, int mCode2){
  if (modeInfo(mCode1,mCode2) != NULL) removeModeInfo(FSModeInfo(mCode1,mCode2).modeCode());
}

void
FSModeCollection::removeModeInfo(TString mString){
  if (modeInfo(mString) != NULL) removeModeInfo(FSModeInfo(mString).modeCode());
}


  // *******************************************************
  // READ MODES FROM A FILE AND ADD TO THE COLLECTION
  // *******************************************************

map<TString, vector<TString> >
FSModeCollection::addModesFromFile(TString filename){

  ifstream infile(FSString::TString2string(filename).c_str());

  TString topparticle("");
  TString particle("");
  vector<TString> decays;
  map<TString, vector<TString> > decayMap;

  string sline;
  while(getline(infile,sline)){
    TString line = FSString::string2TString(sline);
    vector<TString> words = FSString::parseTString(line);

    if (words.size() > 0){

      if (words[0] == "Decay"){
        if (words.size() != 2){
          cout << "addModesFromFile: PROBLEM WITH DECAY LINE" << endl;
          cout << line << endl;
          cout << "quitting" << endl;
          return decayMap;
        }
        particle = words[1];
        if (topparticle == "") topparticle = particle;
      }

      else if ((words[0] == "Enddecay") && (particle != "")){
        decayMap[particle] = decays;
        particle = "";
        decays.clear();
      }

      else if (words[0] == "End"){
        break;
      }

      else if (words[0][0] == '#'){
        continue;
      }

      else if ((particle != "") && (line != "")){
        decays.push_back(line);
      }

    }

  }

  vector<TString> defns = FSString::expandDefinitions(topparticle,decayMap);
  for (unsigned int i = 0; i < defns.size(); i++){
    FSModeInfo* modeInfo = FSModeCollection::addModeInfo(defns[i]);
    modeInfo->addCategory(topparticle);
  }
  return decayMap;

}



  // *******************************************************
  // RETRIEVE MODES FROM THE COLLECTION
  // *******************************************************

FSModeInfo*
FSModeCollection::modeInfo(pair<int,int> mCode){
  pair<int,int> mCode2(mCode.second,mCode.first);
  map< pair<int,int>, FSModeInfo*>::const_iterator mapitr = m_modeInfoMap.find(mCode2);
  if (mapitr != m_modeInfoMap.end()) return m_modeInfoMap[mCode2];
  return NULL;
}

FSModeInfo*
FSModeCollection::modeInfo(int mCode1, int mCode2){
  return modeInfo(FSModeInfo(mCode1,mCode2).modeCode());
}

FSModeInfo*
FSModeCollection::modeInfo(TString mString){
  return modeInfo(FSModeInfo(mString).modeCode());
}

FSModeInfo*
FSModeCollection::modeInfoForCategory(TString category){
  vector<FSModeInfo*> mvec = modeVector(category);
  if (mvec.size() > 1){ 
    cout << "FSModeCollection::modeInfoForCategory WARNING: " << endl;
    cout << "   more than one mode for category = " << category << endl;
    cout << "   only returning the first" << endl;
    return mvec[0];
  }
  if (mvec.size() == 0){ 
    cout << "FSModeCollection::modeInfoForCategory WARNING: " << endl;
    cout << "   no modes for category = " << category << endl;
    cout << "   returning NULL" << endl;
    return NULL;
  }
  return mvec[0];
}


  // *******************************************************
  // DISPLAY MODES IN THE COLLECTION
  // *******************************************************

void 
FSModeCollection::display(TString category){
  vector<FSModeInfo*> mvec = modeVector(category);
  for (unsigned int i = 0; i < mvec.size(); i++){
    mvec[i]->display(i+1);
  }
}


  // *******************************************************
  // CLEAR THE COLLECTION
  // *******************************************************

void 
FSModeCollection::clear(){
  for (map< pair<int,int>, FSModeInfo*>::iterator mapitr = m_modeInfoMap.begin();
       mapitr != m_modeInfoMap.end(); mapitr++){
    if ((*mapitr).second) delete (*mapitr).second;
  }
  m_modeInfoMap.clear();
}



  // *******************************************************
  // FORM A VECTOR OF MODES FROM A SET OF CATEGORIES
  // *******************************************************

vector<FSModeInfo*>
FSModeCollection::modeVector(TString category){

    // copy the map into a vector
  vector<FSModeInfo*> mVectorOriginal;
  for (map< pair<int,int>, FSModeInfo*>::iterator mapitr = m_modeInfoMap.begin();
       mapitr != m_modeInfoMap.end(); mapitr++){
    mVectorOriginal.push_back((*mapitr).second);
  }

    // loop over modes and keep modes that pass the category logic
  vector<FSModeInfo*> mVectorSelected;
  for (unsigned int i = 0; i < mVectorOriginal.size(); i++){
    FSModeInfo* mInfo = mVectorOriginal[i];
    if (FSString::evalLogicalTString(category,mInfo->categories()))
      mVectorSelected.push_back(mInfo);
  }

  return mVectorSelected;
}


int 
FSModeCollection::modeVectorSize(TString category){
  return modeVector(category).size();
}

FSModeInfo*
FSModeCollection::modeVectorElement(TString category, unsigned int index){
  vector<FSModeInfo*> mVector = modeVector(category);
  FSModeInfo* mElement = NULL;
  if (index < mVector.size()) mElement = mVector[index];
  return mElement;
}


  // *******************************************************
  // TELL THE BES FSFILTER ALGORITHM WHICH MODES TO FIND
  // *******************************************************

void 
FSModeCollection::printBESModes(TString category, TString outputFile, int nstart, TString tag){
  TString line("FSFilter.FSMODECOUNTER = \""); line += tag; line += "MODESTRING\";";
  printStrings(category,line,outputFile,nstart);
}


  // *******************************************************
  // TELL THE CLEO STANDARDDECAYPROC WHICH MODES TO FIND
  // *******************************************************

void 
FSModeCollection::printCLEOModes(TString category, TString outputFile){
  TString line("param StandardDecayProc modeMODECOUNTER FSMODESTRING");
  printStrings(category,line,outputFile);
}


  // *******************************************************
  // TELL EVTGEN WHICH DECAYS TO GENERATE
  // *******************************************************

void
FSModeCollection::printEvtGen(TString category, TString outputFile){
  TString line("1.000  MODEDESCRIPTION  PHSP;");
  printStrings(category,line,outputFile);
}


  // *******************************************************
  // PRINT A LIST OF MODES TO A FILE
  // *******************************************************

void
FSModeCollection::printDescriptions(TString category, TString outputFile){
  TString line("(MODECOUNTER)  MODEDESCRIPTION");
  printStrings(category,line,outputFile);
}


  // *******************************************************
  // PRINT STRINGS WITH REPLACED KEYWORDS 
  //   (see FSModeInfo::modeString) TO A FILE
  // *******************************************************

void
FSModeCollection::printStrings(TString category, TString inputLine, TString outputFile, 
                             int counterStart, bool append){
  std::ios_base::openmode mode = ios::out;
  if (append) mode = ios::app;
  ofstream fout(FSString::TString2string(outputFile).c_str(),mode);
  vector<FSModeInfo*> mVector = modeVector(category);
  for (unsigned int i = 0; i < mVector.size(); i++){
    fout << mVector[i]->modeString(inputLine, i+counterStart) << endl;
  }
  fout.close();
}
