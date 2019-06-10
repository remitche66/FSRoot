#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include "TChain.h"
#include "TString.h"
#include "FSBasic/FSString.h"
#include "FSData/FSXYPoint.h"


FSXYPoint::FSXYPoint(){
  clear();
}

void
FSXYPoint::clear(){
  m_xValue = 0.0;
  m_xValueLow = 0.0;
  m_xValueHigh = 0.0;
  m_xError = 0.0;
  m_xErrorLow = 0.0;
  m_xErrorHigh = 0.0;
  m_xErrorSyst = 0.0;
  m_xErrorSystLow = 0.0;
  m_xErrorSystHigh = 0.0;
  m_yValue = 0.0;
  m_yValueLow = 0.0;
  m_yValueHigh = 0.0;
  m_yError = 0.0;
  m_yErrorLow = 0.0;
  m_yErrorHigh = 0.0;
  m_yErrorSyst = 0.0;
  m_yErrorSystLow = 0.0;
  m_yErrorSystHigh = 0.0;
  m_categories.clear();
}


void
FSXYPoint::display(int counter){
  if (counter >= 0) cout << counter << ". ";
  cout << "FSXYPoint: " << endl;
  cout << "    categories: ";
  for (unsigned int j = 0; j < m_categories.size(); j++){
    if (j%4 == 0){  cout << endl;  cout << "        ";  }
    cout << m_categories[j] << "  ";
  }
  cout << endl;
  cout << "    xValue = " << m_xValue << endl;
  cout << "    yValue = " << m_yValue << endl;
  cout << endl;
}


bool
FSXYPoint::hasCategory(TString category){
  for (unsigned int i = 0; i < m_categories.size(); i++){
    if (m_categories[i] == category) return true;
  }
  return false;
}


void
FSXYPoint::addCategory(TString category){
  if (!hasCategory(category) && category != "")  m_categories.push_back(category);
}


void
FSXYPoint::setValuesFromString(TString sValues){
  vector<TString> sExps = FSString::parseTString(sValues);
  for (unsigned int i = 0; i < sExps.size(); i++){
    TString sExp = FSString::removeWhiteSpace(sExps[i]);
    vector<TString> sVals = FSString::parseTString(sExps[i],"=");
    if (sVals.size() != 2){ cout << "ERROR" << endl; exit(0); }
    TString sVar = sVals[0];
    TString sVal = sVals[1];  double dVal = FSString::TString2double(sVal);
         if (sVar == "xV"){ setXV(dVal); }
    else if (sVar == "xVL"){ setXVL(dVal); }
    else if (sVar == "xVH"){ setXVH(dVal); }
    else{ cout << "ERROR" << endl; exit(0); }
    cout << "sVar = " << sVar << endl;
    cout << "dVal = " << dVal << endl;
  }
}


void FSXYPoint::setXV(double val){ m_xValue = val; }
void FSXYPoint::setXVL(double val){ m_xValueLow = val; }
void FSXYPoint::setXVH(double val){ m_xValueHigh = val; }

