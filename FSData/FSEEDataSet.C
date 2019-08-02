#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include "TChain.h"
#include "TString.h"
#include "FSBasic/FSString.h"
#include "FSData/FSEEDataSet.h"

vector< pair<TString, pair<double,double> > > FSEEDataSet::m_vectorLumCategories;


FSEEDataSet::FSEEDataSet(){
  clearDS();
}

void
FSEEDataSet::setName(TString n_name){
  if (m_name != ""){
    cout << "FSEEDataSet::setName ERROR" << endl;
    exit(0);
  }
  m_name = n_name;
  addDSCategory(m_name,false);
}

void
FSEEDataSet::clearDS(){
  m_name = "";
  m_runStart.clear();
  m_runEnd.clear();
  m_ecm = -1.0;
  m_ecmStatError = 0.0;
  m_ecmSystError = 0.0;
  m_ecmLow = 1.0e10;
  m_ecmHigh = -1.0e10;
  m_lum = 0.0;
  m_lumStatError = 0.0;
  m_lumSystError = 0.0;
  m_subSets.clear();
  m_dsCategories.clear();
  m_lumCategories.clear();
}


FSEEDataSet::FSEEDataSet(TString  n_name,
                     int      n_runStart,
                     int      n_runEnd,
                     double   n_ecm,
                     double   n_ecmStatError,
                     double   n_ecmSystError,
                     double   n_lum,
                     double   n_lumStatError,
                     double   n_lumSystError,
                     vector<TString> n_extraDSCategories){
  clearDS();
  setName(n_name);
  m_runStart.push_back(n_runStart);
  m_runEnd.push_back(n_runEnd);
  m_ecm = n_ecm;
  m_ecmStatError = n_ecmStatError;
  m_ecmSystError = n_ecmSystError;
  m_ecmLow  = n_ecm - ecmError();
  m_ecmHigh = n_ecm + ecmError();
  m_lum = n_lum;
  m_lumStatError = n_lumStatError;
  m_lumSystError = n_lumSystError;
  if (m_lum <= 0.0){ 
    cout << "FSEEDataSet Error: no luminosity" << endl; 
    exit(1);
  }
  for (unsigned int i = 0; i < n_extraDSCategories.size(); i++){
    addDSCategory(n_extraDSCategories[i],false);
  }
  addLUMCategories();
}

FSEEDataSet::FSEEDataSet(TString  n_name,
                     double   n_ecm,
                     double   n_lum,
                     vector<TString> n_extraDSCategories){
  clearDS();
  setName(n_name);
  m_ecm = n_ecm;
  m_ecmLow  = n_ecm - ecmError();
  m_ecmHigh = n_ecm + ecmError();
  m_lum = n_lum;
  if (m_lum <= 0.0){ 
    cout << "FSEEDataSet Error: no luminosity" << endl; 
    exit(1);
  }
  for (unsigned int i = 0; i < n_extraDSCategories.size(); i++){
    addDSCategory(n_extraDSCategories[i],false);
  }
  addLUMCategories();
}


void
FSEEDataSet::addSubSet(FSEEDataSet* dataSet){
    // add to the run lists
  vector<int> n_runStart = dataSet->runStart();
  vector<int> n_runEnd   = dataSet->runEnd();
  for (unsigned int i = 0; i < n_runStart.size(); i++){
    m_runStart.push_back(n_runStart[i]);
    m_runEnd.push_back(n_runEnd[i]);
  }
      // try to shorten the run lists a bit
    if (m_runStart.size() > 1){
      vector<int> tempRunStart = m_runStart;
      vector<int> tempRunEnd = m_runEnd;
      m_runStart.clear();
      m_runEnd.clear();
      for (unsigned int i = 0; i < tempRunStart.size()-1; i++){
      for (unsigned int j = i+1; j < tempRunStart.size(); j++){
        if (tempRunStart[i] > tempRunStart[j]){
          int temp = tempRunStart[i];
          tempRunStart[i] = tempRunStart[j];
          tempRunStart[j] = temp;
          temp = tempRunEnd[i];
          tempRunEnd[i] = tempRunEnd[j];
          tempRunEnd[j] = temp;
        }
      }}
      int startRun = 0;
      for (unsigned int i = 0; i < tempRunStart.size(); i++){
        if ((i != tempRunStart.size()-1) && 
            (tempRunEnd[i]+1 == tempRunStart[i+1])){
          if (startRun == 0) startRun = tempRunStart[i];
        }
        else{
          if (startRun != 0){
            m_runStart.push_back(startRun);
            startRun = 0;
          }
          else{
            m_runStart.push_back(tempRunStart[i]);
          }
          m_runEnd.push_back(tempRunEnd[i]);
        }
      }
    }
    // add ecm info
  double ecmOld = m_ecm;
  double lumOld = m_lum;
  m_ecm = (ecmOld*lumOld + dataSet->ecm()*dataSet->lum()) /
          (lumOld + dataSet->lum());
  if (dataSet->ecmStatError() > m_ecmStatError) 
    m_ecmStatError = dataSet->ecmStatError(); 
  if (dataSet->ecmSystError() > m_ecmSystError) 
    m_ecmSystError = dataSet->ecmSystError(); 
  if (dataSet->ecmLow() < m_ecmLow)
    m_ecmLow = dataSet->ecmLow(); 
  if (dataSet->ecmHigh() > m_ecmHigh)
    m_ecmHigh = dataSet->ecmHigh(); 
    // add lum info
  double oldLumSystErrorRel = lumSystErrorRel();
  double newLumSystErrorRel = dataSet->lumSystErrorRel();
  double n_lumSystErrorRel = oldLumSystErrorRel;
  if (newLumSystErrorRel > oldLumSystErrorRel) 
    n_lumSystErrorRel = newLumSystErrorRel;
  m_lum += dataSet->lum();
  m_lumStatError = sqrt(m_lumStatError*m_lumStatError
        + dataSet->lumStatError()*dataSet->lumStatError());
  m_lumSystError = m_lum * n_lumSystErrorRel; 
    // add categories
  vector<TString> subCategories = dataSet->dsCategories();
  for (unsigned int i = 0; i < subCategories.size(); i++){
    addDSCategory(subCategories[i],false);
  }
    // done
  m_subSets.push_back(dataSet);
}


TString
FSEEDataSet::runCut(){
  TString cut("");
  for (unsigned int i = 0; i < m_runStart.size(); i++){
    if (i == 0) cut += "(";
    if (i > 0) cut += "||";
    cut += "((abs(Run)>=";   cut += FSString::int2TString(m_runStart[i]);
    cut += ")&&(abs(Run)<="; cut += FSString::int2TString(m_runEnd[i]); cut += "))";
    if (i == m_runStart.size()-1) cut += ")";
  }
  return cut;
}


void
FSEEDataSet::display(int counter){
  if (counter >= 0) cout << counter << ". ";
  cout << "FSEEDataSet: " << m_name << endl;
  cout << "    dsCategories: ";
  for (unsigned int j = 0; j < m_dsCategories.size(); j++){
    if (j%4 == 0){  cout << endl;  cout << "        ";  }
    cout << m_dsCategories[j] << "  ";
  }
  cout << endl;
  cout << "    lumCategories: ";
  for (unsigned int j = 0; j < m_lumCategories.size(); j++){
    if (j%4 == 0){  cout << endl;  cout << "        ";  }
    cout << m_lumCategories[j] << "  ";
  }
  cout << endl;
  cout << "    runs: " << endl;
  for (unsigned int i = 0; i < m_runStart.size(); i++){
    cout << "        " << m_runStart[i] << "  " << m_runEnd[i] << endl;
  }
  cout << "    ecm = " << m_ecm << endl;
  cout << "    ecm error1 = " << m_ecmStatError << endl;
  cout << "    ecm error2 = " << m_ecmSystError << endl;
  cout << "    lum = " << m_lum << endl;
  cout << "    lum error1 = " << m_lumStatError << endl;
  cout << "    lum error2 = " << m_lumSystError << endl;
  cout << "    subsets: " << endl;
  for (unsigned int i = 0; i < m_subSets.size(); i++){
    cout << "        " << m_subSets[i]->name() << endl;
  }
  cout << endl;
}


bool
FSEEDataSet::hasDSCategory(TString category){
  for (unsigned int i = 0; i < m_dsCategories.size(); i++){
    if (m_dsCategories[i] == category) return true;
  }
  return false;
}


void
FSEEDataSet::addDSCategory(TString category, bool propagateToSubsets){
  if (!hasDSCategory(category) && category != "")  m_dsCategories.push_back(category);
  if (propagateToSubsets){
    for (unsigned int i = 0; i < m_subSets.size(); i++){
      m_subSets[i]->addDSCategory(category,propagateToSubsets);
    }
  }
}


bool
FSEEDataSet::hasLUMCategory(TString category){
  for (unsigned int i = 0; i < m_lumCategories.size(); i++){
    if (m_lumCategories[i] == category) return true;
  }
  return false;
}


void
FSEEDataSet::addLUMCategory(TString category){
  if (!hasLUMCategory(category) && category != "")
    m_lumCategories.push_back(category);
}


void
FSEEDataSet::addLUMCategories(){
  for (unsigned int i = 0; i < m_vectorLumCategories.size(); i++){
    TString category = m_vectorLumCategories[i].first;
    pair<double,double> lumPair = m_vectorLumCategories[i].second;
    if (((lumPair.first  < 0.0) || (lum() > lumPair.first )) &&
        ((lumPair.second < 0.0) || (lum() < lumPair.second)))
      addLUMCategory(category);
  }
}

