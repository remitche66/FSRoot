#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include "TChain.h"
#include "TString.h"
#include "FSData/FSDataSet.h"
#include "FSData/FSDataSetList.h"
#include "FSBasic/FSString.h"


vector<FSDataSet*> FSDataSetList::m_vectorDataSets;
map< TString, FSDataSet* > FSDataSetList::m_mapTempDataSets;


void
FSDataSetList::addDataSetsFromFile(TString fileName){
  clearTempDataSets();
  cout << "FSDataSetList: Reading data from file: " << fileName << endl;
  ifstream infile(FSString::TString2string(fileName).c_str());
  if (!infile.is_open()){
    cout << "FSDataSetList ERROR: cannot find file: " << fileName << endl;
    exit(0);
  }
  string sline;
  while(getline(infile,sline)){
    TString line = FSString::string2TString(sline);
    vector<TString> words = FSString::parseTString(line);
    if (words.size() == 0) continue;
    if (words[0][0] == '#') continue;
    if (words[0] == "data"){
      if (words.size() < 10){
        cout << "FSDataSetList ERROR: problem with data command" << endl;
        exit(0);
      }
      TString fsName(words[1]);
        if (getDataSetVector(fsName).size() > 0){
          cout << "FSDataSetList ERROR:  using the same data set name twice: " << fsName <<  endl;
          exit(1);
        }
      int fsRunStart(FSString::TString2int(words[2]));
      int fsRunEnd(FSString::TString2int(words[3]));
      double fsEcm(FSString::TString2double(words[4]));
      TString sEcmStatError(words[5]);
      TString sEcmSystError(words[6]);
        bool relativeErrorStat = false;
        bool relativeErrorSyst = false;
        while (sEcmStatError.Contains("R")){
          sEcmStatError.Replace(sEcmStatError.Index("R"),1,"");
          relativeErrorStat = true; }
        while (sEcmSystError.Contains("R")){
          sEcmSystError.Replace(sEcmSystError.Index("R"),1,"");
          relativeErrorSyst = true; }
        double fsEcmStatError(FSString::TString2double(sEcmStatError));
        double fsEcmSystError(FSString::TString2double(sEcmSystError));
        if (relativeErrorStat) fsEcmStatError = fsEcmStatError*fsEcm;
        if (relativeErrorSyst) fsEcmSystError = fsEcmSystError*fsEcm;
      double fsLum(FSString::TString2double(words[7]));
      TString sLumStatError(words[8]);
      TString sLumSystError(words[9]);
        relativeErrorStat = false;
        relativeErrorSyst = false;
        while (sLumStatError.Contains("R")){
          sLumStatError.Replace(sLumStatError.Index("R"),1,"");
          relativeErrorStat = true; }
        while (sLumSystError.Contains("R")){
          sLumSystError.Replace(sLumSystError.Index("R"),1,"");
          relativeErrorSyst = true; }
        double fsLumStatError(FSString::TString2double(sLumStatError));
        double fsLumSystError(FSString::TString2double(sLumSystError));
        if (relativeErrorStat) fsLumStatError = fsLumStatError*fsLum;
        if (relativeErrorSyst) fsLumSystError = fsLumSystError*fsLum;
      FSDataSet* fsd = new FSDataSet(fsName, fsRunStart, fsRunEnd,
                                     fsEcm, fsEcmStatError, fsEcmSystError,
                                     fsLum, fsLumStatError, fsLumSystError);
      fsd->display();
      m_vectorDataSets.push_back(fsd);
    }
    else if (words[0] == "category"){
      if (words.size() < 3){
        cout << "FSDataSetList ERROR: problem with cateory command" << endl;
        exit(0);
      }
      TString category = words[1];
      for (unsigned int i = 2; i < words.size(); i++){
        vector<FSDataSet*> fsdv = getDataSetVector(words[i]);
        for (unsigned int idata = 0; idata < fsdv.size(); idata++){
          fsdv[idata]->addCategory(category);
        }
      }
    }
    else{
      cout << "FSDataSetList ERROR: problem reading file " << fileName << endl;
      cout << "  line:" << endl << line << endl;  exit(1);
    }
  }
  if (m_vectorDataSets.size() == 0){
    cout << "FSDataSetList WARNING:  no points found in file: " << fileName << endl;
  }
}




FSDataSet*
FSDataSetList::getDataSet(TString category, double ecmGrouping){
  TString name = "(total)" + category;
  if (ecmGrouping > 0.0){
    name += "(ecmgroup)" + FSString::double2TString(ecmGrouping,8,true);
  }
  FSDataSet* fsd = findTempDataSet(name);
  if (fsd) return fsd;
  vector<FSDataSet*> fsdv = getDataSetVector(category,ecmGrouping);
  if (fsdv.size() == 0) return NULL;
  fsd = new FSDataSet(name);
  for (unsigned int i = 0; i < fsdv.size(); i++){
    fsd->addSubSet(fsdv[i]);
  }
  m_mapTempDataSets[name] = fsd;
  return fsd;
}



vector<FSDataSet*>
FSDataSetList::getDataSetVector(TString category, double ecmGrouping){
    // loop over data sets and keep those that pass the category logic
  vector<FSDataSet*> fsdVectorSelected;
  for (unsigned int i = 0; i < m_vectorDataSets.size(); i++){
    FSDataSet* fsd = m_vectorDataSets[i];
    if (FSString::evalLogicalTString(category,fsd->categories()))
      fsdVectorSelected.push_back(fsd);
  }
  if (ecmGrouping <=  0.0) return fsdVectorSelected;
  if (fsdVectorSelected.size() < 2) return fsdVectorSelected;
    // sort by ecm
  for (unsigned int i = 0; i < fsdVectorSelected.size()-1; i++){
  for (unsigned int j = i+1; j < fsdVectorSelected.size(); j++){
    double ecmi = fsdVectorSelected[i]->ecm();
    double ecmj = fsdVectorSelected[j]->ecm();
    TString namei = fsdVectorSelected[i]->name();
    TString namej = fsdVectorSelected[j]->name();
    if ((ecmi > ecmj) || 
        ((ecmi == ecmj) && (namei > namej))){
      FSDataSet* tempDataSet = fsdVectorSelected[i];
      fsdVectorSelected[i] = fsdVectorSelected[j];
      fsdVectorSelected[j] = tempDataSet;
    }
  }}
    // do the ecm grouping
  vector<FSDataSet*> fsdVectorGrouped;
  for (unsigned int i = 0; i < fsdVectorSelected.size(); i++){
    TString groupCat(fsdVectorSelected[i]->name());
    double ecmi = fsdVectorSelected[i]->ecm();
    int iskip = 0;
    for (unsigned int j = i+1; j < fsdVectorSelected.size(); j++){
      double ecmj = fsdVectorSelected[j]->ecm();
      if ((ecmj - ecmi) < ecmGrouping){
        groupCat += "," + fsdVectorSelected[j]->name();
        iskip++;
      }
      else{ break; }
    }
    FSDataSet* fsdGroup = getDataSet(groupCat);
    if (!fsdGroup){ cout << "FSDataSetList ERROR" << endl; exit(1); }
    vector<FSDataSet*> groupSubSets = fsdGroup->subSets();
    if (groupSubSets.size() == 1){
      fsdVectorGrouped.push_back(groupSubSets[0]);
    }
    else{
      fsdVectorGrouped.push_back(fsdGroup);
    }
    i += iskip;
  }
  // check for nearby points
  // FSDatSet* first, last
  // if gaps too big, error, ambiguity

  // use getDataSet to combine

 return fsdVectorGrouped;

}


void 
FSDataSetList::display(TString category, double ecmGrouping){
  FSDataSet* fsd = getDataSet(category,ecmGrouping);
  if (!fsd){
    cout << "No data sets found for category: " << category << endl;
    return;
  }
  vector<FSDataSet*> fsdv = fsd->subSets();
  cout << endl << "****** SUB DATA SETS ******" << endl << endl;
  for (unsigned int i = 0; i < fsdv.size(); i++){
    fsdv[i]->display(i+1);
  }
  cout << "****** TOTAL DATA SET ******" << endl << endl;
  fsd->display();
}



FSDataSet*
FSDataSetList::findTempDataSet(TString name){
  map<TString,FSDataSet*>::iterator it = m_mapTempDataSets.find(name);
  if (it != m_mapTempDataSets.end()) return (*it).second;
  FSDataSet* fsd = NULL;
  return fsd;
}


void
FSDataSetList::clearTempDataSets(){
  for (map<TString,FSDataSet*>::iterator mapitr = m_mapTempDataSets.begin();
       mapitr != m_mapTempDataSets.end(); mapitr++){
    if ((*mapitr).second) delete (*mapitr).second;
  }
  m_mapTempDataSets.clear();
}


void
FSDataSetList::clearDataSets(){
  clearTempDataSets();
  for (unsigned int i = 0; i < m_vectorDataSets.size(); i++){
    if (m_vectorDataSets[i]) delete m_vectorDataSets[i];
  }
  m_vectorDataSets.clear();
}
