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

