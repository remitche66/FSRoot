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

void
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
          return;
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
  if (decayMap.find(topparticle) == decayMap.end()) return;
  vector<TString> modes = decayMap[topparticle];
  if (modes.size() == 0) return;
  bool done = false;
  while (!done){
    done = true;
    vector<TString> modesCopy = modes;  modes.clear();
    for (unsigned int i = 0; i < modesCopy.size(); i++){
      bool madeSubs = false;
      vector<TString> parts = FSString::parseTString(modesCopy[i]);
      for (unsigned int j = 0; j < parts.size(); j++){
        if (decayMap.find(parts[j]) != decayMap.end()){
          done = false;
          madeSubs = true;
          vector<TString> subs = decayMap[parts[j]];
          for (unsigned int k = 0; k < subs.size(); k++){
            modes.push_back(expandLine(modesCopy[i],parts[j],subs[k],decayMap));
          }
          break;
        }
      }
      if (!madeSubs) modes.push_back(modesCopy[i]);
    }
  }
  for (unsigned int i = 0; i < modes.size(); i++){
    FSModeInfo* modeInfo = FSModeCollection::addModeInfo(modes[i]);
    modeInfo->addCategory(topparticle);
  }
  return;
}


TString 
FSModeCollection::expandLine(TString line, TString word, TString definition, 
                   map<TString, vector<TString> >& decayMap){
  FSModeInfo miLine(line,false);
  FSModeInfo miDef(definition,false);
  vector<TString> catsLine = miLine.categories();
  vector<TString> catsDef  = miDef.categories();
  vector<TString> catsExtra;
  for (unsigned int i = 0; i < catsLine.size(); i++){
  for (unsigned int j = 0; j < catsDef.size(); j++){
    if (decayMap.find(catsLine[i]) == decayMap.end() && 
        decayMap.find(catsDef[j]) == decayMap.end())
      catsExtra.push_back(catsLine[i]+"__"+catsDef[j]);
  }}
  vector<TString> lineParts = FSString::parseTString(line);
  for (unsigned int i = 0; i < lineParts.size(); i++){
    if (lineParts[i] == word){ lineParts[i] = definition; break; }
  }
  for (unsigned int i = 0; i < catsExtra.size(); i++){
    lineParts.push_back(catsExtra[i]);
  }
  TString newLine = "";
  for (unsigned int i = 0; i < lineParts.size(); i++){
    newLine += lineParts[i]; newLine += " ";
  }
  return newLine;
}



  // *******************************************************
  // FORM A VECTOR OF MODES FROM A SET OF CATEGORIES
  // *******************************************************

vector<FSModeInfo*>
FSModeCollection::modeVector(TString category){
  vector<FSModeInfo*> mVector;
  int component = -1;  vector<TString> mainParts = FSString::parseTString(category,";");
  if (mainParts.size() == 2){ category = mainParts[0]; component = FSString::TString2int(mainParts[1]); }
  for (map< pair<int,int>, FSModeInfo*>::iterator mapitr = m_modeInfoMap.begin();
       mapitr != m_modeInfoMap.end(); mapitr++){
    if ((*mapitr).second->hasCategory(category))
      mVector.push_back((*mapitr).second);
  }
  if (component < 0) return mVector;
  if (component > 0 && component <= (int)mVector.size()){
    vector<FSModeInfo*> mVector2;
    mVector2.push_back(mVector[component-1]);
    return mVector2;
  }
  return vector<FSModeInfo*>();
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
FSModeCollection::display(TString category, bool showDetails){
  vector<FSModeInfo*> mvec = modeVector(category);
  for (unsigned int i = 0; i < mvec.size(); i++){
    mvec[i]->display(i+1, showDetails);
  }
}


  // *******************************************************
  // CLEAR THE COLLECTION
  // *******************************************************

void 
FSModeCollection::clear(TString category){
  vector<FSModeInfo*> eraseVector = modeVector(category);
  for (unsigned int i = 0; i < eraseVector.size(); i++){
    int code1 = eraseVector[i]->modeCode1();
    int code2 = eraseVector[i]->modeCode2();
    pair<int,int> mode2(code2,code1);
    if (m_modeInfoMap.find(mode2) != m_modeInfoMap.end()){
      if (m_modeInfoMap[mode2]) delete m_modeInfoMap[mode2];
      m_modeInfoMap.erase(mode2);
    }
  }
  if (category == "") m_modeInfoMap.clear();
}

