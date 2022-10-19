#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "FSBasic/FSControl.h"
#include "FSBasic/FSCanvas.h"
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
  cut         = FSString::removeWhiteSpace(cut);         if (cut == "") cut = "(1==1)";
  cutSideBand = FSString::removeWhiteSpace(cutSideBand); if (cutSideBand == "") cutSideBand = "(1==1)";
  if (m_cutCache.find(cutName) != m_cutCache.end() && m_cutCache[cutName]) delete m_cutCache[cutName];
  if (cutName == ""){ cout << "FSCut ERROR: no cut name" << endl; return; }
  if (!FSString::checkParentheses(cut)){ 
    cout << "FSCut ERROR: problem with parentheses in cut = " << cut << endl; return; }
  if (!FSString::checkParentheses(cutSideBand)){
    cout << "FSCut ERROR: problem with parentheses in sideband cut = " << cutSideBand << endl; return; }
  m_cutCache[cutName] = new FSCutInfo(cutName,cut,cutSideBand,weight);
  if (FSControl::DEBUG){
    cout << "FSCut: created cut..." << endl;
    display(cutName);
  }
}


void
FSCut::defineCut(TString cutName, TString cutVariable,
                            TString sigLow, TString sigHigh, 
                            TString sbLow1, TString sbHigh1,
                            TString sbLow2, TString sbHigh2, double weight){
  cutName     = FSString::removeWhiteSpace(cutName);
  cutVariable = FSString::removeWhiteSpace(cutVariable);
  sigLow      = FSString::removeWhiteSpace(sigLow);
  sigHigh     = FSString::removeWhiteSpace(sigHigh);
  sbLow1      = FSString::removeWhiteSpace(sbLow1);
  sbHigh1     = FSString::removeWhiteSpace(sbHigh1);
  sbLow2      = FSString::removeWhiteSpace(sbLow2);
  sbHigh2     = FSString::removeWhiteSpace(sbHigh2);
  if (m_cutCache.find(cutName) != m_cutCache.end() && m_cutCache[cutName]) delete m_cutCache[cutName];
  if (cutName == ""){ cout << "FSCut ERROR: no cut name" << endl; return; }
  if (cutVariable == "") { cout << "FSCut ERROR: no cut variable" << endl; return; }
  m_cutCache[cutName] = new FSCutInfo(cutName,cutVariable,
                     sigLow, sigHigh, sbLow1, sbHigh1, sbLow2, sbHigh2, weight);
  if (FSControl::DEBUG){
    cout << "FSCut: created cut..." << endl;
    display(cutName);
  }  
}


      // ********************************************************
      // VARY AND RESET CUTS
      // ********************************************************


void
FSCut::varyCut(TString cutName, TString cut, TString cutSideBand, double weight){
  cutName     = FSString::removeWhiteSpace(cutName);
  cut         = FSString::removeWhiteSpace(cut);         if (cut == "") cut = "(1==1)";
  cutSideBand = FSString::removeWhiteSpace(cutSideBand); if (cutSideBand == "") cutSideBand = "(1==1)";
  FSCutInfo* cutInfo = getCut(cutName);  
  if (!cutInfo){ cout << "FSCut::varyCut ERROR: no cut with name = " << cutName << endl; exit(0); }
  cutInfo->m_cut =         cut;
  cutInfo->m_cutSideBand = cutSideBand;
  cutInfo->m_weight =      weight;
}

void
FSCut::varyCut(TString cutName, TString cutVariable,
                            TString sigLow, TString sigHigh, 
                            TString sbLow1, TString sbHigh1,
                            TString sbLow2, TString sbHigh2, double weight){
  cutName     = FSString::removeWhiteSpace(cutName);
  cutVariable = FSString::removeWhiteSpace(cutVariable);
  sigLow      = FSString::removeWhiteSpace(sigLow);
  sigHigh     = FSString::removeWhiteSpace(sigHigh);
  sbLow1      = FSString::removeWhiteSpace(sbLow1);
  sbHigh1     = FSString::removeWhiteSpace(sbHigh1);
  sbLow2      = FSString::removeWhiteSpace(sbLow2);
  sbHigh2     = FSString::removeWhiteSpace(sbHigh2);
  FSCutInfo* cutInfo = getCut(cutName);  
  if (!cutInfo){ cout << "FSCut::varyCut ERROR: no cut with name = " << cutName << endl; exit(0); }
  cutInfo->m_cutVariable = cutVariable;
  cutInfo->m_sSigLow =      sigLow;
  cutInfo->m_sSigHigh =     sigHigh;
  cutInfo->m_sSbLow1 =      sbLow1;
  cutInfo->m_sSbHigh1 =     sbHigh1;
  cutInfo->m_sSbLow2 =      sbLow2;
  cutInfo->m_sSbHigh2 =     sbHigh2;
  cutInfo->m_weight =      weight; 
  cutInfo->makeCutStrings();
}


void
FSCut::resetCut(TString cutName){
  if (cutName == "") cutName = "*";
  map< TString, FSCutInfo* >::iterator it = m_cutCache.begin();
  for (; it != m_cutCache.end(); it++){
    if (!FSString::compareTStrings(it->first,cutName)) continue;
    FSCutInfo* cutInfo = it->second;  if (!cutInfo) continue;
    cutInfo->m_cutVariable = cutInfo->m0_cutVariable;
    cutInfo->m_cut =         cutInfo->m0_cut;        
    cutInfo->m_cutSideBand = cutInfo->m0_cutSideBand;
    cutInfo->m_weight =      cutInfo->m0_weight; 
    cutInfo->m_sSigLow =      cutInfo->m0_sSigLow;
    cutInfo->m_sSigHigh =     cutInfo->m0_sSigHigh;
    cutInfo->m_sSbLow1 =      cutInfo->m0_sSbLow1;
    cutInfo->m_sSbHigh1 =     cutInfo->m0_sSbHigh1;
    cutInfo->m_sSbLow2 =      cutInfo->m0_sSbLow2;
    cutInfo->m_sSbHigh2 =     cutInfo->m0_sSbHigh2;
    if (cutInfo->m_cutVariable != "") cutInfo->makeCutStrings();
  }
}


void
FSCutInfo::makeCutStrings(){
  TString cutSigLow (""); if (m_sSigLow  != "") cutSigLow  = "(("+m_cutVariable+")>="+m_sSigLow +")";
  TString cutSigHigh(""); if (m_sSigHigh != "") cutSigHigh = "(("+m_cutVariable+")<="+m_sSigHigh+")";
  TString cutSbLow1 (""); if (m_sSbLow1  != "") cutSbLow1  = "(("+m_cutVariable+")>="+m_sSbLow1 +")";
  TString cutSbHigh1(""); if (m_sSbHigh1 != "") cutSbHigh1 = "(("+m_cutVariable+")<="+m_sSbHigh1+")";
  TString cutSbLow2 (""); if (m_sSbLow2  != "") cutSbLow2  = "(("+m_cutVariable+")>="+m_sSbLow2 +")";
  TString cutSbHigh2(""); if (m_sSbHigh2 != "") cutSbHigh2 = "(("+m_cutVariable+")<="+m_sSbHigh2+")";
  TString cutSb1("");  TString cutSb2("");
  if (cutSigLow != ""){ m_cut  = cutSigLow; }  if (cutSigHigh != ""){ if (m_cut  != "") m_cut  += "&&"; m_cut  += cutSigHigh; }
  if (cutSbLow1 != ""){ cutSb1 = cutSbLow1; }  if (cutSbHigh1 != ""){ if (cutSb1 != "") cutSb1 += "&&"; cutSb1 += cutSbHigh1; }
  if (cutSbLow2 != ""){ cutSb2 = cutSbLow2; }  if (cutSbHigh2 != ""){ if (cutSb2 != "") cutSb2 += "&&"; cutSb2 += cutSbHigh2; }
  if (m_cut  != "") m_cut  = "("+m_cut +")";
  if (cutSb1 != "") cutSb1 = "("+cutSb1+")";
  if (cutSb2 != "") cutSb2 = "("+cutSb2+")";
  if (cutSb1 != ""){ m_cutSideBand = cutSb1; }  if (cutSb2 != ""){ if (m_cutSideBand != "") m_cutSideBand += "||"; m_cutSideBand += cutSb2; }
  if (m_cutSideBand != "") m_cutSideBand = "("+m_cutSideBand+")";  
  if (m_cut == "") m_cut = "(1==1)";
  if (m_cutSideBand == "") m_cutSideBand = "(1==1)";
}



      // ********************************************************
      // DRAW CUT ARROWS
      // ********************************************************


void
FSCut::drawCutArrows(TString cutName, int sigColor, int sbColor){
  FSCutInfo* cutInfo = getCut(cutName);  if (!cutInfo) return;
  if (cutInfo->m_sSigLow != "")  FSCanvas::drawCutArrow(FSString::TString2double(cutInfo->m_sSigLow),  sigColor, "|>", 0.02, 0.04);
  if (cutInfo->m_sSigHigh != "") FSCanvas::drawCutArrow(FSString::TString2double(cutInfo->m_sSigHigh), sigColor, "<|", 0.02, 0.04);
  if (cutInfo->m_sSbLow1 != "")  FSCanvas::drawCutArrow(FSString::TString2double(cutInfo->m_sSbLow1),  sbColor,  "|>", 0.02, 0.04);
  if (cutInfo->m_sSbHigh1 != "") FSCanvas::drawCutArrow(FSString::TString2double(cutInfo->m_sSbHigh1), sbColor,  "<|", 0.02, 0.04);
  if (cutInfo->m_sSbLow2 != "")  FSCanvas::drawCutArrow(FSString::TString2double(cutInfo->m_sSbLow2),  sbColor,  "|>", 0.02, 0.04);
  if (cutInfo->m_sSbHigh2 != "") FSCanvas::drawCutArrow(FSString::TString2double(cutInfo->m_sSbHigh2), sbColor,  "<|", 0.02, 0.04);
}


      // ********************************************************
      // DISPLAY CUTS
      // ********************************************************

void 
FSCut::display(TString cutName){
  cutName = FSString::removeWhiteSpace(cutName);
  if (cutName == "") cutName = "*";
  cout << "FSCut Information:" << endl;
  map< TString, FSCutInfo* >::iterator it = m_cutCache.begin();
  int cutCountAll = 0;
  int cutCountShown = 0;
  for (; it != m_cutCache.end(); it++){
    TString locName = it->first;
    cutCountAll++;
    FSCutInfo* cutInfo = it->second;
    if (FSString::compareTStrings(locName,cutName)){
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
  pair<TString,double> badCut = pair<TString,double>("!!BAD_FSCUT!!",1.0);
  vector< pair<TString,double> > badCuts;  badCuts.push_back(badCut);

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

      // check that the cuts exist
    for (unsigned int i = 0; i < cutList.size(); i++){
      if (!getCut(cutList[i])) return badCuts; }

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
        cout << "FSCut ERROR:  too many cut combinations" << endl;
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
        cout << "FSCut ERROR:  found empty cuts" << endl; exit(1);
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
    cout << "FSCut ERROR: can't find cut with name = " << cutName << endl;
    return NULL;
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

