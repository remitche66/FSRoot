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

FSXYPoint::FSXYPoint(TString inName, double scale, FSXYPoint* xyp){
  clear();
  setNAME(inName);
  m_XVL  =         xyp->xValueLow();
  m_XVH  =         xyp->xValueHigh();
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


FSXYPoint::FSXYPoint(TString inName, TString operation, FSXYPoint* xyp1, FSXYPoint* xyp2){
  clear();
  if ((operation != "*") && (operation != "/") && (operation != "+") && (operation != "-")){
    cout << "FSXYPoint ERROR: bad operation: " << operation << endl;
    exit(0);
  }
  if ((operation == "/") && (xyp2->yValue() == 0.0)){
    cout << "FSXYPoint ERROR: divide by zero" << endl;
    exit(0);
  }
  setNAME(inName);
  m_XVL  = xyp1->xValueLow();
  m_XVH  = xyp1->xValueHigh();
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
  m_NAME = "";
  m_XVL = 0.0;
  m_XVH = 0.0;
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
  cout << "    name: " << m_NAME << endl;
  cout << "    categories: ";
  vector<TString> n_categories = categories();
  for (unsigned int j = 0; j < n_categories.size(); j++){
    if (j%4 == 0){  cout << endl;  cout << "        ";  }
    cout << n_categories[j] << "  ";
  }
  cout << endl;
  cout << "    xValue = " << xValue() 
                          << " + " << xErrorHigh()     << " - " << xErrorLow()
                          << " + " << xErrorSystHigh() << " - " << xErrorSystLow() << endl;
  cout << "    yValue = " << yValue() 
                          << " + " << yErrorHigh()     << " - " << yErrorLow()
                          << " + " << yErrorSystHigh() << " - " << yErrorSystLow() << endl;
  cout << endl;
}

void
FSXYPoint::setValuesFromString(TString sValues){
cout << "START HERE: " << sValues << endl;
  if (!setValuesFromMap(parseValuesFromString(sValues))){
    cout << "FSXYPoint ERROR: problem setting values from string: " << endl << "  " << sValues << endl;
    exit(0);
  }
}

void
FSXYPoint::setNAME(vector<TString> longerName){
  TString inName("");
  for (unsigned int i = 0; i < longerName.size(); i++){
    inName += longerName[i]; if (i != longerName.size()-1) inName += " "; }
  setNAME(inName);
}

bool
FSXYPoint::setValuesFromMap(map<TString, vector<TString> > mValues){
  if (!checkMap(mValues)){ return false; }
  if (mValues.find("NAME") != mValues.end()){ setNAME(mValues["NAME"]); }
  if (mValues.find("XV")   != mValues.end()){ if (!setXYV("X",mValues["XV"])) return false; }
  if (mValues.find("YV")   != mValues.end()){ if (!setXYV("Y",mValues["YV"])) return false; }
  if (mValues.find("XE")   != mValues.end()){ if (!setXYE("XE",  mValues["XE"]  )) return false; } 
  if (mValues.find("XEH")  != mValues.end()){ if (!setXYE("XEH", mValues["XEH"] )) return false; } 
  if (mValues.find("XEL")  != mValues.end()){ if (!setXYE("XEL", mValues["XEL"] )) return false; } 
  if (mValues.find("XES")  != mValues.end()){ if (!setXYE("XES", mValues["XES"] )) return false; } 
  if (mValues.find("XESH") != mValues.end()){ if (!setXYE("XESH",mValues["XESH"])) return false; } 
  if (mValues.find("XESL") != mValues.end()){ if (!setXYE("XESL",mValues["XESL"])) return false; } 
  if (mValues.find("YE")   != mValues.end()){ if (!setXYE("YE",  mValues["YE"]  )) return false; } 
  if (mValues.find("YEH")  != mValues.end()){ if (!setXYE("YEH", mValues["YEH"] )) return false; } 
  if (mValues.find("YEL")  != mValues.end()){ if (!setXYE("YEL", mValues["YEL"] )) return false; } 
  if (mValues.find("YES")  != mValues.end()){ if (!setXYE("YES", mValues["YES"] )) return false; } 
  if (mValues.find("YESH") != mValues.end()){ if (!setXYE("YESH",mValues["YESH"])) return false; } 
  if (mValues.find("YESL") != mValues.end()){ if (!setXYE("YESL",mValues["YESL"])) return false; } 
  if (mValues.find("XVL")  != mValues.end()){ setXVL(FSString::TString2double(mValues["XVL"][0])); }
  if (mValues.find("XVH")  != mValues.end()){ setXVH(FSString::TString2double(mValues["XVH"][0])); }
  if (mValues.find("XL")   != mValues.end()){ setXL(mValues["XL"][0]); }
  if (mValues.find("CAT")  != mValues.end()){ addCategories(mValues["CAT"]); }
  return true;
}


bool
FSXYPoint::checkKey(TString key){
  if (key == "NAME") return true;
  if (key == "XV")   return true;
  if (key == "XVL")  return true;
  if (key == "XVH")  return true;
  if (key == "XL")   return true;
  if (key == "XE")   return true;
  if (key == "XEL")  return true;
  if (key == "XEH")  return true;
  if (key == "XES")  return true;
  if (key == "XESL") return true;
  if (key == "XESH") return true;
  if (key == "YV")   return true;
  if (key == "YE")   return true;
  if (key == "YEL")  return true;
  if (key == "YEH")  return true;
  if (key == "YES")  return true;
  if (key == "YESL") return true;
  if (key == "YESH") return true;
  if (key == "CAT")  return true;
  return false;
}


bool
FSXYPoint::checkMap(map<TString, vector<TString> > mValues){
  if (mValues.size() == 0){
    cout << "FSXYPoint ERROR: empty map" << endl;
    return false;
  }
  for (map<TString, vector<TString> >::iterator it = mValues.begin(); it != mValues.end(); it++){
    TString key = it->first;
    vector<TString> values = it->second;
    if (!checkKey(key)){
      cout << "FSXYPoint ERROR: undefined key: " << key << endl;
      return false;
    }
    if (values.size() == 0){
      cout << "FSXYPoint ERROR: no values for key: " << key << endl;
      return false;
    }
    for (unsigned int i = 0; i < values.size(); i++){
      if (values[i].Length() == 0){
        cout << "FSXYPoint ERROR: empty values for key: " << key << endl;
        return false;
      }
    }
  }
  return true;
}


// input is a string like
//   "key1: value1a value2a  key2  :value2a value2b"
//     (using ":" to separate keys from values
//       and " " to separate everything else)
// returns map< KEY, vector<value> >

map<TString, vector<TString> >
FSXYPoint::parseValuesFromString(TString sValues){
  map<TString, vector<TString> > mValues;
  sValues = FSString::removeTabs(sValues);
  vector<TString> words = FSString::parseTString(sValues);
  words = FSString::parseTString(words,":",true);
  if ((words.size() < 3) || (words[1] != ":") || (words[0] == ":")) return mValues;
  TString key("");
  for (unsigned int i = 0; i < words.size(); i++){
    if (words[i] == ":"){
      key = words[i-1];  key.ToUpper();
    }
    else if ((key != "") && ((i == words.size()-1) || (words[i+1] != ":"))) {
      mValues[key].push_back(words[i]);
    }
  }
  return mValues;
}



bool
FSXYPoint::setXYV(TString XY, TString sVal){
  vector<TString> sVals;  sVals.push_back(sVal);
  return setXYV(XY,sVals);
}

bool
FSXYPoint::setXYV(TString XY, vector<TString> sVals){
  sVals = FSString::parseTString(sVals);
  vector<TString> spacers;  spacers.push_back("+"); spacers.push_back("-");
  sVals = FSString::parseTString(sVals,spacers,true);
  if ((sVals.size() == 0) || (sVals[0].Length() == 0)) return false;
  if (XY == "X") setXV(FSString::TString2double(sVals[0])); 
  if (XY == "Y") setYV(FSString::TString2double(sVals[0])); 
  if (sVals.size() == 1) return true;
  bool statp = false;  bool statm = false;
  for (unsigned int i = 1; i < sVals.size()-1; i++){
    if (i + 2 < sVals.size() && ((sVals[i] == "+" && sVals[i+1] == "-") || 
                                 (sVals[i] == "-" && sVals[i+1] == "+"))){
      if  (statp){ setXYE(XY+"ESH", sVals[i+2]); }
      if  (statm){ setXYE(XY+"ESL", sVals[i+2]); }
      if (!statp){ setXYE(XY+"EH",  sVals[i+2]);  statp = true; }
      if (!statm){ setXYE(XY+"EL",  sVals[i+2]);  statm = true; }
      i++;
    }
    else if (sVals[i] == "+"){
      if  (statp){ setXYE(XY+"ESH", sVals[i+1]); }
      if (!statp){ setXYE(XY+"EH",  sVals[i+1]);  statp = true; }
    }
    else if (sVals[i] == "-"){
      if  (statm){ setXYE(XY+"ESL", sVals[i+1]); }
      if (!statm){ setXYE(XY+"EL",  sVals[i+1]);  statm = true; }
    }
  }
  return true;
}


bool
FSXYPoint::setXYE (TString XYE, vector<TString> sVals){
  for (unsigned int i = 0; i < sVals.size(); i++){ if (!setXYE(XYE,sVals[i])) return false; }
  return true;
}

bool
FSXYPoint::setXYE (TString XYE, TString sVal){
  double dVal = FSString::TString2double(sVal);
  if (XYE.Contains("XE") && (sVal.Contains("r")||sVal.Contains("R"))){ dVal *= xValue(); }
  if (XYE.Contains("YE") && (sVal.Contains("r")||sVal.Contains("R"))){ dVal *= yValue(); }
  if (XYE == "XE"  ){ setXE  (dVal); return true; }
  if (XYE == "XEL" ){ setXEL (dVal); return true; }
  if (XYE == "XEH" ){ setXEH (dVal); return true; }
  if (XYE == "XES" ){ setXES (dVal); return true; }
  if (XYE == "XESL"){ setXESL(dVal); return true; }
  if (XYE == "XESH"){ setXESH(dVal); return true; }
  if (XYE == "YE"  ){ setYE  (dVal); return true; }
  if (XYE == "YEL" ){ setYEL (dVal); return true; }
  if (XYE == "YEH" ){ setYEH (dVal); return true; }
  if (XYE == "YES" ){ setYES (dVal); return true; }
  if (XYE == "YESL"){ setYESL(dVal); return true; }
  if (XYE == "YESH"){ setYESH(dVal); return true; }
  cout << "FSXYPoint ERROR: problem setting errors" << endl;
  return false;
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
  if (category == "") return;
  if (FSString::parseTString(category).size() != 1) return;
  if (hasCategory(category)) return;
  m_categories.push_back(category);
}

void
FSXYPoint::addCategories(vector<TString> inCategories){
  for (unsigned int i = 0; i < inCategories.size(); i++){ addCategory(inCategories[i]); }
}

