#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include "TChain.h"
#include "TString.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSCut.h"
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
  if (m_modeInfoMap.find(mCode2) == m_modeInfoMap.end())
    m_modeInfoMap[mCode2] = new FSModeInfo(mCode);
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
  // READ MODES FROM A FILE AND ADD TO THE COLLECTION
  // *******************************************************

map<TString, vector<TString> >
FSModeCollection::addModesFromFile(TString filename){

  vector<TString> lines = FSString::readLinesFromFile(filename);

  TString topparticle("");
  TString particle("");
  vector<TString> decays;
  map<TString, vector<TString> > decayMap;

  for (unsigned int iL = 0; iL < lines.size(); iL++){
    TString line = lines[iL];
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
  // FORM A VECTOR OF MODES FROM A SET OF CATEGORIES
  // *******************************************************

vector<FSModeInfo*>
FSModeCollection::modeVector(TString category){
  vector<FSModeInfo*> mVector;
  for (map< pair<int,int>, FSModeInfo*>::iterator mapitr = m_modeInfoMap.begin();
       mapitr != m_modeInfoMap.end(); mapitr++){
    if ((*mapitr).second->hasCategory(category))
      mVector.push_back((*mapitr).second);
  }
  return mVector;
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
  // DEFINE MODE-SPECIFIC CUTS USING FSCUT
  // *******************************************************

void 
FSModeCollection::defineCuts(TString category, TString cutName, TString cut, 
                           TString cutSideBand, double weight){
  vector<FSModeInfo*> modes = modeVector(category);
  for (unsigned int i = 0; i < modes.size(); i++){
    FSModeInfo* mi = modes[i];
    FSCut::defineCut(mi->modeString(cutName),mi->modeString(cut),
                     mi->modeString(cutSideBand),weight);
  }
}


  // *******************************************************
  // PRINT COMBINATORICS TO THE SCREEN FOR TESTING
  // *******************************************************

void
FSModeCollection::testCombinatorics(TString category, TString testString, bool expandModeString){
  vector<FSModeInfo*> mVector = modeVector(category);
  if (mVector.size() == 0)
    cout << "FSModeCollection::testCombinatorics: no modes for this category" << endl;
  for (unsigned int i = 0; i < mVector.size(); i++){
    TString testString2 = testString;
    if (expandModeString) testString2 = mVector[i]->modeString(testString2);
    mVector[i]->modeCombinatorics(testString2,true);
  }
}


  // *******************************************************
  // PRINT STRINGS WITH REPLACED KEYWORDS 
  //   (see FSModeInfo::modeString) TO A FILE
  // *******************************************************

void
FSModeCollection::printStrings(TString category, TString inputLine, TString outputFile, 
                             int counterStart, bool append){
  vector<TString> mStrings = modeStrings(category,inputLine,counterStart);
  if (outputFile == ""){
    for (unsigned int i = 0; i < mStrings.size(); i++){
      cout << mStrings[i] << endl;
    }
  }
  std::ios_base::openmode mode = ios::out;
  if (append) mode = ios::app;
  ofstream fout(FSString::TString2string(outputFile).c_str(),mode);
  for (unsigned int i = 0; i < mStrings.size(); i++){
    fout << mStrings[i] << endl;
  }
  fout.close();
}


vector<TString>
FSModeCollection::modeStrings(TString category, TString inputLine, 
                             int counterStart, bool show){
  vector<FSModeInfo*> mVector = modeVector(category);
  vector<TString> mStrings;
  for (unsigned int i = 0; i < mVector.size(); i++){
    mStrings.push_back(mVector[i]->modeString(inputLine, i+counterStart));
  }
  if (show) printStrings(category,inputLine,"",counterStart);
  return mStrings;
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
FSModeCollection::clear(TString category){
  vector< pair<int,int> > eraseModes;
  for (map< pair<int,int>, FSModeInfo*>::iterator mapitr = m_modeInfoMap.begin();
       mapitr != m_modeInfoMap.end(); mapitr++){
    pair<int,int> modeCode = (*mapitr).first;
    FSModeInfo* modeInfo = (*mapitr).second;
    if (modeInfo->hasCategory(category)) eraseModes.push_back(modeCode);
  }
  for (unsigned int i = 0; i < eraseModes.size(); i++){
    if (m_modeInfoMap[eraseModes[i]]) delete m_modeInfoMap[eraseModes[i]];
    m_modeInfoMap.erase(eraseModes[i]);
  }
  if (category == "") m_modeInfoMap.clear();
}

