#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "FSBasic/FSControl.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSCut.h"


  // static member data

map< TString, FSCutInfo* > FSCut::m_cutCache;


      // ********************************************************
      // DEFINE A CUT
      // ********************************************************

void 
FSCut::defineCut(TString cutName, TString cut, 
               TString cutSideBand, double weight){
  cutName     = FSString::removeWhiteSpace(cutName);
  cut         = FSString::removeWhiteSpace(cut);
  cutSideBand = FSString::removeWhiteSpace(cutSideBand);
  if (cut == "") cut = "(1==1)";
  if (cutSideBand == "") cutSideBand = "(1==1)";
  if (m_cutCache[cutName]) delete m_cutCache[cutName];
  m_cutCache[cutName] = new FSCutInfo(cutName,cut,cutSideBand,weight);
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
  cutName = FSString::removeWhiteSpace(cutName);
  cout << "FSCut Information:" << endl;
  map< TString, FSCutInfo* >::iterator it = m_cutCache.begin();
  int cutCountAll = 0;
  int cutCountShown = 0;
  for (; it != m_cutCache.end(); it++){
    TString locName = it->first;  if (locName == "!!BAD_FSCUT!!") continue;
    cutCountAll++;
    FSCutInfo* cutInfo = m_cutCache[locName];
    if ((cutName == "") || FSString::compareTStrings(locName,cutName)){
      cutCountShown++;
      cout << "  (" << cutCountShown << ") "
           << " ********** FSCUT NUMBER " << cutCountAll << " **********" << endl;
      cout << "              name: " << locName << endl;
      cout << "            signal: " << cutInfo->m_cut << endl;
      cout << "          sideband: " << cutInfo->m_cutSideBand << endl;
      cout << "    sideband scale: " << cutInfo->m_weight << endl;
    }
  }
}


      // ********************************************************
      // EXPAND CUTS
      // ********************************************************

vector< pair<TString,double> > 
FSCut::expandCuts(TString cuts, bool showDetails){
  cuts = FSString::removeWhiteSpace(cuts);
  vector< pair<TString,double> > newCuts;
  vector<TString> skipCuts;

  if (FSControl::DEBUG){
    cout << "FSCut: expanding cut... " << endl;
    cout << "  " << cuts << endl;
  }

    // loop over all CUT and CUTSB markers
  TString CUTSKIPMARK ("CUTSKIP(");
  TString CUTMARK     ("CUT(");
  TString CUTSBMARK   ("CUTSB(");
  TString CUTWTMARK   ("CUTWT(");
  TString CUTSBWTMARK ("CUTSBWT(");
  while (cuts.Contains(CUTSKIPMARK) ||
         cuts.Contains(CUTMARK) ||
         cuts.Contains(CUTSBMARK) ||
         cuts.Contains(CUTWTMARK) ||
         cuts.Contains(CUTSBWTMARK)){

      // find a CUT(...) or a CUTSB(...)
    TString mark("");
         if (cuts.Contains(CUTSKIPMARK)){ mark = CUTSKIPMARK; }
    else if (cuts.Contains(CUTMARK))    { mark = CUTMARK; }
    else if (cuts.Contains(CUTSBMARK))  { mark = CUTSBMARK; }
    else if (cuts.Contains(CUTWTMARK))  { mark = CUTWTMARK; }
    else if (cuts.Contains(CUTSBWTMARK)){ mark = CUTSBWTMARK; }
    int index = cuts.Index(mark);
    int size = (mark).Length()+1;

      // make a list of the cuts within the parentheses
    TString cutsInParentheses = FSString::captureParentheses(cuts,index);
    size += cutsInParentheses.Length();
    vector<TString> spacers; spacers.push_back(","); spacers.push_back(";");
    vector<TString> cutList = FSString::parseTString(cutsInParentheses,spacers);

      // skip some cuts
    if (mark != CUTSKIPMARK){
      vector<TString> cutListTemp = cutList;  cutList.clear();
      for (unsigned int i = 0; i < cutListTemp.size(); i++){
        bool skip = false;
        for (unsigned int j = 0 ; j < skipCuts.size(); j++){ if (skipCuts[j] == cutListTemp[i]) skip = true; }
        if (!skip) cutList.push_back(cutListTemp[i]);
      }
    }

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
    if (mark == CUTSKIPMARK){ 
      for (unsigned int i = 0; i < cutList.size(); i++){ skipCuts.push_back(cutList[i]); } }
    else if (mark == CUTMARK)    { substitutes = makeCut(cutList); }
    else if (mark == CUTSBMARK)  { substitutes = makeCutSB(cutList); }
    else if (mark == CUTWTMARK)  { substitutes = makeCutWT(cutList); }
    else if (mark == CUTSBWTMARK){ substitutes = makeCutSBWT(cutList); }


      // replace the old cuts with the new
    if (substitutes.size() == 0) substitutes.push_back(pair<TString,double>("(1==1)",1));
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
    TString cutSig  = getCut(cutList[i])->m_cut;
    if (i == 0) cut += "(";
    if (i != 0) cut += "&&";
    cut += "(";
    cut += cutSig;
    cut += ")";
    if (i == cutList.size()-1) cut += ")";
  }
  if (cut == "") cut = "(1==1)";
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
      TString cutSig  = getCut(cutList[j])->m_cut;
      TString cutSide = getCut(cutList[j])->m_cutSideBand;
      if ((cutSig == "") || (cutSide == "")){
        cout << "FSCut Error:  found empty cuts" << endl; exit(1);
      }
      double cutWT    = getCut(cutList[j])->m_weight;
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
  if (newCuts.size() == 0) newCuts.push_back(pair<TString,double>("(1==1)",1.0));

  return newCuts;
}


vector< pair<TString,double> > 
FSCut::makeCutSBWT(vector<TString> cutList){
  vector< pair<TString,double> > newCuts;
  vector< pair<TString,double> > cutSb = makeCutSB(cutList);
  TString combinedCuts = "";
  for (unsigned int i = 0; i < cutSb.size(); i++){
    if (i == 0) combinedCuts += "(";
    if (i != 0) combinedCuts += "+";
    combinedCuts += "("+FSString::double2TString(cutSb[i].second,8)+")";
    combinedCuts += "*("+cutSb[i].first+")";
    if (i == cutSb.size()-1) combinedCuts += ")";
  }
  newCuts.push_back(pair<TString,double>(combinedCuts,1.0));
  return newCuts;
}

vector< pair<TString,double> > 
FSCut::makeCutWT(vector<TString> cutList){
  vector< pair<TString,double> > newCuts;
  vector< pair<TString,double> > cutSig = makeCut(cutList);
  vector< pair<TString,double> > cutSbWt = makeCutSBWT(cutList);
  if ((cutSig.size() != 1) || (cutSbWt.size() != 1)){
    cout << "FSCut ERROR:  internal problem" << endl; exit(0); }
  TString combinedCuts = "((";
  combinedCuts += cutSig[0].first;
  combinedCuts += ")+(-1)*(";
  combinedCuts += cutSbWt[0].first;
  combinedCuts += "))";
  newCuts.push_back(pair<TString,double>(combinedCuts,1.0));
  return newCuts;
}



FSCutInfo*
FSCut::getCut(TString cutName){
  if (m_cutCache.find(cutName) == m_cutCache.end()){
    TString badFSCut = "!!BAD_FSCUT!!";
    defineCut(badFSCut,badFSCut,badFSCut);
    cout << "FSCut ERROR: can't find cut with name = " << cutName << endl;
    return m_cutCache[badFSCut];
  }
  return m_cutCache[cutName];
}




  // ********************************************************
  // CLEAR GLOBAL CACHES
  // ********************************************************

void
FSCut::clearCuts(){
  if (FSControl::DEBUG) 
    cout << "FSCut: clearing cut cache" << endl;
  map< TString, FSCutInfo* >::iterator it = m_cutCache.begin();
  for (; it != m_cutCache.end(); it++){ if (it->second) delete it->second; }
  m_cutCache.clear();
  if (FSControl::DEBUG) 
    cout << "FSCut: done clearing cut cache" << endl;
}

