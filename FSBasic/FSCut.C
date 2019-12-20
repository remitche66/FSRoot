#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "FSBasic/FSControl.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSCut.h"


  // static member data

map< TString, pair< pair<TString,TString>, double > > FSCut::m_cutCache;


      // ********************************************************
      // DEFINE A CUT
      // ********************************************************

void 
FSCut::defineCut(TString cutName, TString cut, 
               TString cutSideBand, double weight){
  pair<TString,TString> cutPair(cut,cutSideBand);
  pair< pair<TString,TString>, double > cutPairWt(cutPair,weight);
  m_cutCache[cutName] = cutPairWt;
  if (FSControl::DEBUG){
    cout << "FSCut: created cut..." << endl;
    display(cutName);
  }
}


      // ********************************************************
      // DISPLAY CUTS
      // ********************************************************

void 
FSCut::display(TString cutName){
  if (cutName == ""){
    cout << "FSCut: LIST OF DEFINED CUTS" << endl;
    map< TString, pair< pair<TString,TString>, double > >::iterator it = m_cutCache.begin();
    for (; it != m_cutCache.end(); it++){ display(it->first); }
  }
  else{
    pair< pair<TString,TString>, double> cutInfo = getCut(cutName);
    cout << "  FSCut " << cutName << ":" << endl;
    cout << "    signal:   " << cutInfo.first.first << endl;
    cout << "    sideband: " << cutInfo.first.second << endl;
    cout << "    sideband scale: " << cutInfo.second << endl;
  }
}


      // ********************************************************
      // EXPAND CUTS
      // ********************************************************

vector< pair<TString,double> > 
FSCut::expandCuts(TString cuts, bool showDetails){
  cuts = FSString::removeWhiteSpace(cuts);
  vector< pair<TString,double> > newCuts;

  if (FSControl::DEBUG){
    cout << "FSCut: expanding cut... " << endl;
    cout << "  " << cuts << endl;
  }

    // loop over all CUT and CUTSB markers
  TString CUTMARK   ("CUT(");
  TString CUTSBMARK ("CUTSB(");
  while (cuts.Contains(CUTMARK) ||
         cuts.Contains(CUTSBMARK)){

      // find a CUT(...) or a CUTSB(...)
    TString mark("");
         if (cuts.Contains(CUTMARK))  { mark = CUTMARK; }
    else if (cuts.Contains(CUTSBMARK)){ mark = CUTSBMARK; }
    int index = cuts.Index(mark);
    int size = (mark).Length()+1;

      // make a list of the cuts within the parentheses
    TString cutsInParentheses = FSString::captureParentheses(cuts,index);
    size += cutsInParentheses.Length();
    vector<TString> spacers = {",",";"};
    vector<TString> cutList = FSString::parseTString(cutsInParentheses,spacers);

      // sort the cutList vector
    if (cutList.size() != 0){
      for (unsigned int i = 0; i < cutList.size(); i++){
      for (unsigned int j = i+1; j < cutList.size(); j++){
        if (FSString::TString2string(cutList[j]) < FSString::TString2string(cutList[i])){
          TString tmp = cutList[i]; cutList[i] = cutList[j]; cutList[j] = tmp; }
      }}
    }

    if (FSControl::DEBUG){
      cout << "FSCut: here is a list of cuts for mark = " << mark << endl;
      for (unsigned int i = 0; i < cutList.size(); i++){
        cout << "  " << cutList[i] << endl;
      }
    }

      // make the substitute cuts for this mark
    vector< pair<TString,double> > substitutes;
         if (mark == CUTMARK)  { substitutes = makeCut(cutList); }
    else if (mark == CUTSBMARK){ substitutes = makeCutSB(cutList); }


      // replace the old cuts with the new
    if (substitutes.size() > 0){
      double oldWT = 1.0;
      if (newCuts.size() > 1){
        cout << "FSCut Error:  too many cut combinations" << endl;
        exit(1);
      }
      if (newCuts.size() == 1) oldWT = newCuts[0].second;
      newCuts.clear();
      for (unsigned int i = 0; i < substitutes.size(); i++){
        TString cutCopy = cuts;
        cutCopy.Replace(index,size,substitutes[i].first);
        newCuts.push_back(pair<TString,double>(cutCopy,oldWT*substitutes[i].second));
      }
      cuts.Replace(index,size,substitutes[0].first);
    }

  }

  if (newCuts.size() == 0) newCuts.push_back(pair<TString,double>(cuts,1.0));

  if ((FSControl::DEBUG)||(showDetails)){
    cout << "FSCut: here is the list of cuts being returned..." << endl;
    for (unsigned int i = 0; i < newCuts.size(); i++){
      cout << "  " << newCuts[i].first << " " << newCuts[i].second << endl;
    }
  }

  return newCuts;

}


vector< pair<TString,double> > 
FSCut::makeCut(vector<TString> cutList){
  vector< pair<TString,double> > newCuts;
  TString cut("");
  for (unsigned int i = 0; i < cutList.size(); i++){ 
    TString cutSig  = getCut(cutList[i]).first.first;
    if (i == 0) cut += "(";
    if (i != 0) cut += "&&";
    cut += "(";
    cut += cutSig;
    cut += ")";
    if (i == cutList.size()-1) cut += ")";
  }
  newCuts.push_back(pair<TString,double>(cut,1.0));
  return newCuts;
}


vector< pair<TString,double> > 
FSCut::makeCutSB(vector<TString> cutList){
  vector< pair<TString,double> > newCuts;

  int n = cutList.size();
  vector< vector<int> > logicHelp;
  for (int i = 0; i < pow(2,n); i++){
    vector<int> binary;
    for (int d = 0; d < n; d++){
      binary.push_back((i/(int)pow(2,d))%(int)2);
    }
    if (i != 0) logicHelp.push_back(binary);
  }

  if (FSControl::DEBUG){
    cout << "FSCut: logic for making sideband cuts..." << endl;
    for (unsigned int i = 0; i < logicHelp.size(); i++){
      for (unsigned int j = 0; j < logicHelp[i].size(); j++){
        cout << "  " << logicHelp[i][j];
      }
      cout << endl;
    }
  }

  for (unsigned int i = 0; i < logicHelp.size(); i++){
    TString cut("");
    double wt = 1.0;
    for (unsigned int j = 0; j < logicHelp[i].size(); j++){
      TString cutSig  = getCut(cutList[j]).first.first;
      TString cutSide = getCut(cutList[j]).first.second;
      if ((cutSig == "") || (cutSide == "")){
        cout << "FSCut Error:  found empty cuts" << endl; exit(1);
      }
      double cutWT    = getCut(cutList[j]).second;
      if (j == 0) cut += "(";
      if (j != 0) cut += "&&";
      cut += "(";
      if (logicHelp[i][j] == 0) cut += cutSig;
      if (logicHelp[i][j] == 1) cut += cutSide;
      cut += ")";
      if (j == cutList.size()-1) cut += ")";
      if (logicHelp[i][j] == 1) wt *= -1*cutWT;
    }
    newCuts.push_back(pair<TString,double>(cut,-1*wt));
  }

  return newCuts;
}



pair< pair<TString,TString>, double> 
FSCut::getCut(TString cutName){
  if (m_cutCache.find(cutName) == m_cutCache.end()){
    cout << "FSCut Error: can't find cut with name = " << cutName << endl;
    exit(1);
  }
  return m_cutCache[cutName];
}




  // ********************************************************
  // CLEAR GLOBAL CACHES
  // ********************************************************

void
FSCut::clearCutCache(){
  if (FSControl::DEBUG) 
    cout << "FSCut: clearing cut cache" << endl;
  m_cutCache.clear();
  if (FSControl::DEBUG) 
    cout << "FSCut: done clearing cut cache" << endl;
}

