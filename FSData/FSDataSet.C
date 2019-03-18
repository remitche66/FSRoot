#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include "TChain.h"
#include "TString.h"
#include "FSBasic/FSString.h"
#include "FSData/FSDataSet.h"


FSDataSet::FSDataSet(TString  name){
  m_name = name;
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
  m_categories.clear();
}


FSDataSet::FSDataSet(TString  name,
                     int      runStart,
                     int      runEnd,
                     double   ecm,
                     double   ecmStatError,
                     double   ecmSystError,
                     double   lum,
                     double   lumStatError,
                     double   lumSystError){
  m_name = name;
  m_runStart.push_back(runStart);
  m_runEnd.push_back(runEnd);
  m_ecm = ecm;
  m_ecmStatError = ecmStatError;
  m_ecmSystError = ecmSystError;
  m_ecmLow  = ecm - ecmError();
  m_ecmHigh = ecm + ecmError();
  m_lum = lum;
  m_lumStatError = lumStatError;
  m_lumSystError = lumSystError;
  m_subSets.clear();
  if (m_lum <= 0.0){ 
    cout << "FSDataSet Error: no luminosity" << endl; 
    exit(1);
  }
  m_categories.clear();
  addCategory(m_name);
  //vector<TString> nameParts = FSString::parseTString(m_name,":");
  //for (unsigned int i = 0; i < nameParts.size(); i++){ addCategory(nameParts[i]); }  
}


void
FSDataSet::addSubSet(FSDataSet* dataSet){
    // add to the run lists
  vector<int> runStart = dataSet->runStart();
  vector<int> runEnd   = dataSet->runEnd();
  for (unsigned int i = 0; i < runStart.size(); i++){
    m_runStart.push_back(runStart[i]);
    m_runEnd.push_back(runEnd[i]);
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
  double lumSystErrorRel = oldLumSystErrorRel;
  if (newLumSystErrorRel > oldLumSystErrorRel) 
    lumSystErrorRel = newLumSystErrorRel;
  m_lum += dataSet->lum();
  m_lumStatError = sqrt(m_lumStatError*m_lumStatError
        + dataSet->lumStatError()*dataSet->lumStatError());
  m_lumSystError = m_lum * lumSystErrorRel; 
  m_subSets.push_back(dataSet);
}


TString
FSDataSet::runCut(){
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
FSDataSet::display(int counter){
  if (counter >= 0) cout << counter << ". ";
  cout << "FSDataSet: " << m_name << endl;
  cout << "    categories: ";
  for (unsigned int j = 0; j < m_categories.size(); j++){
    if (j%4 == 0){  cout << endl;  cout << "        ";  }
    cout << m_categories[j] << "  ";
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
FSDataSet::hasCategory(TString category){
  bool found = false;
  for (unsigned int i = 0; i < m_categories.size(); i++){
    if (m_categories[i] == category) found = true;
  }
  return found;
}


void
FSDataSet::addCategory(TString category){
  if (!hasCategory(category) && category != "") m_categories.push_back(category);
}



