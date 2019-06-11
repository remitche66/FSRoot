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

FSXYPoint::FSXYPoint(TString category, double scale, FSXYPoint* xyp){
  clear();
  addCategory(category);
  m_XV   =         xyp->xValue();
  m_XL   =         xyp->xLabel();
  m_XEL  =         xyp->xErrorLow();
  m_XEH  =         xyp->xErrorHigh();
  m_XESL =         xyp->xErrorSystLow();
  m_XESH =         xyp->xErrorSystHigh();
  m_YV   = scale * xyp->yValue();
  m_YEL  = scale * xyp->yErrorLow();
  m_YEH  = scale * xyp->yErrorHigh();
  m_YESL = scale * xyp->yErrorSystLow();
  m_YESH = scale * xyp->yErrorSystHigh();
}


FSXYPoint::FSXYPoint(TString category, TString operation, FSXYPoint* xyp1, FSXYPoint* xyp2){
  clear();
  if ((operation != "*") && (operation != "/") && (operation != "+") && (operation != "-")){
    cout << "FSXYPoint ERROR: bad operation: " << operation << endl;
    exit(0);
  }
  if ((operation == "/") && (xyp2->yValue() == 0.0)){
    cout << "FSXYPoint ERROR: divide by zero" << endl;
    exit(0);
  }
  addCategory(category);
  m_XV   = xyp1->xValue();
  m_XL   = xyp1->xLabel();
  m_XEL  = xyp1->xErrorLow();
  m_XEH  = xyp1->xErrorHigh();
  m_XESL = xyp1->xErrorSystLow();
  m_XESH = xyp1->xErrorSystHigh();
  double   v1 = xyp1->yValue();
  double  el1 = xyp1->yErrorLow();      double  eh1 = xyp1->yErrorHigh();
  double esl1 = xyp1->yErrorSystLow();  double esh1 = xyp1->yErrorSystHigh();
  double   v2 = xyp2->yValue();
  double  el2 = xyp2->yErrorLow();      double  eh2 = xyp2->yErrorHigh();
  double esl2 = xyp2->yErrorSystLow();  double esh2 = xyp2->yErrorSystHigh();
  if (operation == "*") m_YV = v1 * v2;
  if (operation == "/") m_YV = v1 / v2;
  if (operation == "+") m_YV = v1 + v2;
  if (operation == "-") m_YV = v1 - v2;
  if ((operation == "+") || (operation == "-")){
    m_YEL = sqrt(el1*el1 + el2*el2);
    m_YEH = sqrt(eh1*eh1 + eh2*eh2);
    m_YESL = sqrt(esl1*esl1 + esl2*esl2);
    m_YESH = sqrt(esh1*esh1 + esh2*esh2);
  }
  if (operation == "*"){
    m_YEL = sqrt(v2*v2*el1*el1 + v1*v1*el2*el2);
    m_YEH = sqrt(v2*v2*eh1*eh1 + v1*v1*eh2*eh2);
    m_YESL = sqrt(v2*v2*esl1*esl1 + v1*v1*esl2*esl2);
    m_YESH = sqrt(v2*v2*esh1*esh1 + v1*v1*esh2*esh2);
  }
  if (operation == "/"){
    m_YEL = sqrt(el1*el1/v2/v2 + el2*el2*v1*v1/v2/v2/v2/v2);
    m_YEH = sqrt(eh1*eh1/v2/v2 + eh2*eh2*v1*v1/v2/v2/v2/v2);
    m_YESL = sqrt(esl1*esl1/v2/v2 + esl2*esl2*v1*v1/v2/v2/v2/v2);
    m_YESH = sqrt(esh1*esh1/v2/v2 + esh2*esh2*v1*v1/v2/v2/v2/v2);
  }
}


void
FSXYPoint::clear(){
  m_XV = 0.0;
  m_XL = "";
  m_XEL = 0.0;
  m_XEH = 0.0;
  m_XESL = 0.0;
  m_XESH = 0.0;
  m_YV = 0.0;
  m_YEL = 0.0;
  m_YEH = 0.0;
  m_YESL = 0.0;
  m_YESH = 0.0;
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
  cout << "    xValue = " << m_XV << endl;
  cout << "    yValue = " << m_YV << endl;
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
    vector<TString> sVals = FSString::parseTString(sExps[i],":");
    if (sVals.size() != 2){
      cout << "FSXYPoint ERROR: problem parsing values from: " << sValues << endl;
      exit(0);
    }
    TString sVar = sVals[0]; sVar.ToUpper();
    TString sVal = sVals[1];
     double dVal = FSString::TString2double(sVal);
         if (sVar == "XV")  { setXV  (dVal); }
    else if (sVar == "XL")  { setXL  (sVal); }
    else if (sVar == "XE")  { setXE  (dVal); }
    else if (sVar == "XEL") { setXEL (dVal); }
    else if (sVar == "XEH") { setXEH (dVal); }
    else if (sVar == "XES") { setXES (dVal); }
    else if (sVar == "XESL"){ setXESL(dVal); }
    else if (sVar == "XESH"){ setXESH(dVal); }
    else if (sVar == "YV")  { setYV  (dVal); }
    else if (sVar == "YE")  { setYE  (dVal); }
    else if (sVar == "YEL") { setYEL (dVal); }
    else if (sVar == "YEH") { setYEH (dVal); }
    else if (sVar == "YES") { setYES (dVal); }
    else if (sVar == "YESL"){ setYESL(dVal); }
    else if (sVar == "YESH"){ setYESH(dVal); }
    else if (sVar == "CAT") { addCategory(sVal); }
    else{
      cout << "FSXYPoint ERROR: problem parsing variable named: " << sVar << endl;
      exit(0);
    }
  }
}

