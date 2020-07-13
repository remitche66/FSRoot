#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>

#include "TROOT.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TChainElement.h"
#include "TObjArray.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "THStack.h"
#include "TLegend.h"
#include "FSBasic/FSCanvas.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSPhysics.h"
#include "FSBasic/FSCut.h"
#include "FSMode/FSModeString.h"
#include "FSMode/FSModeHistogram.h"

  // static member data
map<TString, map<TString, float> > FSModeHistogram::m_cacheComponentsMap;



  // ********************************************************
  // CREATE A HISTOGRAM FROM A TREE
  // ********************************************************

TH1F* 
FSModeHistogram::getTH1F(TString fileName, TString ntName, TString category,
                       TString variable, TString bounds,
                       TString cuts, double scale, bool TESTONLY){
  pair<TString, vector<TString> > 
    indices = getHistogramIndexTree(1,fileName,ntName,category,variable,bounds,cuts,scale);
  if (TESTONLY){ FSHistogram::printIndexInfo(indices.first,indices.second); return NULL; }
  TH1F* hist = FSHistogram::getFSHistogramInfo(indices.first,indices.second)->getTHNF().first;
  if (FSHistogram::m_USEDATAFRAME && FSHistogram::m_USEDATAFRAMENOW)
    FSHistogram::executeRDataFrame();
  return FSHistogram::getTH1F(hist);
}



TH2F* 
FSModeHistogram::getTH2F(TString fileName, TString ntName, TString category,
                       TString variable, TString bounds,
                       TString cuts, double scale, bool TESTONLY){
  pair<TString, vector<TString> > 
    indices = getHistogramIndexTree(2,fileName,ntName,category,variable,bounds,cuts,scale);
  if (TESTONLY){ FSHistogram::printIndexInfo(indices.first,indices.second); return NULL; }
  TH2F* hist = FSHistogram::getFSHistogramInfo(indices.first,indices.second)->getTHNF().second;
  if (FSHistogram::m_USEDATAFRAME && FSHistogram::m_USEDATAFRAMENOW)
    FSHistogram::executeRDataFrame();
  return FSHistogram::getTH2F(hist);
}


      // ********************************************************
      // CREATE A TREE IN THE SAME WAY AS A HISTOGRAM
      // ********************************************************


TTree*
FSModeHistogram::getTH1FContents(TString fileName, TString ntName, TString category, 
       TString variable, TString bounds, TString cuts, double scale,
       vector< pair<TString,TString> > extraTreeContents){
  pair<TString, vector<TString> > 
    indices = getHistogramIndexTree(1,fileName,ntName,category,variable,bounds,cuts,scale);
  return FSHistogram::getFSHistogramInfo(indices.first,indices.second)
                                        ->getTHNFContents(extraTreeContents);
}

TTree*
FSModeHistogram::getTH2FContents(TString fileName, TString ntName, TString category, 
       TString variable, TString bounds, TString cuts, double scale,
       vector< pair<TString,TString> > extraTreeContents){
  pair<TString, vector<TString> > 
    indices = getHistogramIndexTree(2,fileName,ntName,category,variable,bounds,cuts,scale);
  return FSHistogram::getFSHistogramInfo(indices.first,indices.second)
                                        ->getTHNFContents(extraTreeContents);
}



  // ********************************************************
  // DRAW HISTOGRAMS SEPARATELY FOR EACH MODE
  // ********************************************************


void
FSModeHistogram::drawTH1F(TString fileName, TString ntName, TString category,
                        TString variable, TString bounds,
                        TString cuts,     TString options,
                        double scale, TCanvas* c1){
  drawTHNF(1,fileName,ntName,category,variable,bounds,cuts,options,scale,c1);
}

void
FSModeHistogram::drawTH2F(TString fileName, TString ntName, TString category,
                        TString variable, TString bounds,
                        TString cuts,     TString options,
                        double scale, TCanvas* c1){
  drawTHNF(2,fileName,ntName,category,variable,bounds,cuts,options,scale,c1);
}


void
FSModeHistogram::drawTHNF(int dimension,
                       TString fileName, TString ntName, TString category,
                       TString variable, TString bounds,
                       TString cuts,     TString options,
                       double scale, TCanvas* c1){

  vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);

  if (!c1){
    c1 = new TCanvas("cDrawModeHistogram","cDrawModeHistogram",600,600);
    FSCanvas::divideGPad(modeVector.size());
  }


  // loop over all modes in this category

  for (unsigned int i = 0; i < modeVector.size(); i++){

    c1->cd(i+1);

    TString category_i = modeVector[i]->modeString("CODE=MODECODE");
    TString title_i = FSModeString::rootSymbols(modeVector[i]->modeDescription());

    TH1F* hist1d = NULL;
    TH2F* hist2d = NULL;

    if (dimension == 1){
      hist1d = getTH1F(fileName,ntName,category_i,
                       variable,bounds,cuts,scale);
      hist1d->SetTitle(title_i);
      hist1d->Draw(options);
    }
    if (dimension == 2){
      hist2d = getTH2F(fileName,ntName,category_i,
                       variable,bounds,cuts,scale);
      hist2d->SetTitle(title_i);
      hist2d->Draw(options);
    }

  }

  c1->cd();
  c1->Update();

}


  // ********************************************************
  // DECONSTRUCT A HISTOGRAM INTO ITS MC COMPONENTS
  //   (using MCDecayCode1, MCDecayCode2, and MCExtras)
  // ********************************************************


vector< pair<TString,float> >
FSModeHistogram::getMCComponentsAndSizes(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts,
                                double scale, bool moreInfo, bool show){
    // initial setup

  vector< pair<TString,float> > components;
  map<TString,float> mcComponentsMap;

    // create an index for caching

  TString index = getHistogramIndexTree(1,fileName,ntName,category,
                                       variable,bounds,cuts,scale).first;
  if (moreInfo) index += "{-MI-}";

    // search the cache for this mode

  map<TString, map<TString,float> >::const_iterator 
    mapItr = m_cacheComponentsMap.find(index);
  if (mapItr != m_cacheComponentsMap.end()){
    mcComponentsMap = m_cacheComponentsMap[index];
  }
  else{

      // create a new tree with MC information

    vector< pair<TString,TString> > extraTreeContents;
    extraTreeContents.push_back(pair<TString,TString>("MCDecayCode1","MCDecayCode1"));
    extraTreeContents.push_back(pair<TString,TString>("MCDecayCode2","MCDecayCode2"));
    extraTreeContents.push_back(pair<TString,TString>("MCExtras","MCExtras"));
    if (moreInfo){
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle1","MCDecayParticle1"));
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle2","MCDecayParticle2"));
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle3","MCDecayParticle3"));
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle4","MCDecayParticle4"));
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle5","MCDecayParticle5"));
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle6","MCDecayParticle6"));
    }
      // first check to make sure the branches exist
    vector<TString> branches = FSTree::getBranchNamesFromTree(fileName,ntName,"MC*");
    for (unsigned int i = 0; i < extraTreeContents.size(); i++){
      bool found = false;
      for (unsigned int j = 0; j < branches.size(); j++){
        if (branches[j] == extraTreeContents[i].first){ found = true; break; }
      }
      if (!found){
        cout << "FSModeHistogram::getMCComponentsAndSizes ERROR: variable " << extraTreeContents[i].first <<
                 " not found" << endl;
        return components;
      }
    }
    TTree* histTree = getTH1FContents(fileName,ntName,category,variable,bounds,cuts,scale,extraTreeContents);

      // set the branch addresses for the new tree

    Double_t dcode1 = 0.0, dcode2 = 0.0, dextra = 0.0;
    Double_t dp1 = 0.0, dp2 = 0.0, dp3 = 0.0, 
             dp4 = 0.0, dp5 = 0.0, dp6 = 0.0;
    histTree->SetBranchAddress("MCDecayCode1",&dcode1);
    histTree->SetBranchAddress("MCDecayCode2",&dcode2);
    histTree->SetBranchAddress("MCExtras",&dextra);
    if (moreInfo){
      histTree->SetBranchAddress("MCDecayParticle1",&dp1);
      histTree->SetBranchAddress("MCDecayParticle2",&dp2);
      histTree->SetBranchAddress("MCDecayParticle3",&dp3);
      histTree->SetBranchAddress("MCDecayParticle4",&dp4);
      histTree->SetBranchAddress("MCDecayParticle5",&dp5);
      histTree->SetBranchAddress("MCDecayParticle6",&dp6);
    }

      // loop over the new tree and count MC components

    for (int ientry = 0; ientry < histTree->GetEntries(); ientry++){
      histTree->GetEntry(ientry);
      TString modeString = FSString::int2TString((int)dextra) + "_" 
                           + FSModeInfo((int)dcode1,(int)dcode2).modeString();
      if (moreInfo){
        modeString += ":";
        modeString += (int)dp1;  modeString += "_";
        modeString += (int)dp2;  modeString += "_";
        modeString += (int)dp3;  modeString += "_";
        modeString += (int)dp4;  modeString += "_";
        modeString += (int)dp5;  modeString += "_";
        modeString += (int)dp6;
      }
      map<TString,float>::const_iterator mapItrx = mcComponentsMap.find(modeString);
      if (mapItrx == mcComponentsMap.end()){
        mcComponentsMap[modeString] = 0.0;
      }
      mcComponentsMap[modeString] += scale;
    }
    for (map<TString,float>::iterator itr = mcComponentsMap.begin(); itr != mcComponentsMap.end(); itr++){
      if (histTree->GetEntries() > 0) itr->second *= 1.0/histTree->GetEntries(); 
    }

      // cache the new mcComponentsMap

    m_cacheComponentsMap[index] = mcComponentsMap;

  }

    // fill the components vector (ordered from most to least populated)

  for (map<TString,float>::iterator mapItr1 = mcComponentsMap.begin();
       mapItr1 != mcComponentsMap.end(); mapItr1++){
    float max = 0;
    TString maxMode("");
    for (map<TString,float>::iterator mapItr2 = mcComponentsMap.begin();
      mapItr2 != mcComponentsMap.end(); mapItr2++){
      bool found = false;
      for (unsigned int i = 0; i < components.size(); i++){
        if (mapItr2->first == components[i].first) found = true; 
      }
      if ((!found) && (mapItr2->second >= max)){
        max = mapItr2->second;
        maxMode = mapItr2->first;
      }
    }
    components.push_back(pair<TString,float>(maxMode,max));
  }


    // print the results

  if (show){
    cout << "****************" << endl;
    cout << "MC Components:" << endl;
    cout << "****************" << endl;
    for (unsigned int i = 0; i < components.size(); i++){
      cout << formatMCComponent(components[i].first,components[i].second) << endl;
    }
    cout << "****************" << endl;
  }

  return components;

}

vector<TString>
FSModeHistogram::getMCComponents(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts,
                                double scale, bool moreInfo, bool show){
  vector<TString> components;
  vector< pair<TString,float> > componentsAndSizes 
    = getMCComponentsAndSizes(fileName,ntName,category,variable,bounds,cuts,scale,moreInfo,show);
  for (unsigned int i = 0; i < componentsAndSizes.size(); i++){
    components.push_back(componentsAndSizes[i].first); }
  return components;
}


vector<TH1F*>
FSModeHistogram::getMCComponentsTH1F(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts,
                                double scale){
  vector<TH1F*> histograms;
  vector< pair<TString,float> > components
    = getMCComponentsAndSizes(fileName,ntName,category,variable,bounds,cuts,scale,false,true);
  if (components.size() == 0) return histograms;
  for (unsigned int i = 0; i < components.size(); i++){
    double fraction = 100*components[i].second;
    if (fraction < 0.01 || i > 10) continue;
    TString mcCut(cuts);
    if (mcCut != "") mcCut += "&&";
    mcCut += getMCComponentCut(components[i].first);
    TH1F* hcomp = getTH1F(fileName,ntName,category,variable,bounds,mcCut,scale);
    histograms.push_back(hcomp);
  }
  return histograms;
}


TH1F*
FSModeHistogram::drawMCComponents(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts,
                                double scale, TCanvas* c1){

    // create the original histogram

  vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);
  if (modeVector.size() == 0 && category != ""){
    cout << "FSModeHistogram::drawMCComponents ERROR: no modes for category = " << category << endl;
    exit(0);
  }
  TH1F* htot = getTH1F(fileName,ntName,category,variable,bounds,cuts,scale);
  FSModeInfo* modeInfo = NULL; if (modeVector.size() > 0) modeInfo = modeVector[0];
  htot->SetTitle("");
  if (modeInfo) htot->SetTitle(FSModeString::rootSymbols(modeInfo->modeDescription()));
  if (modeVector.size() > 1) htot->SetTitle("category = "+category);

    // make a new TCanvas if one isn't passed in

  if (!c1) c1 = new TCanvas("cDrawMCComponents","cDrawMCComponents",600,600);

    // get vectors of the MC components and the histograms

  vector< pair<TString,float> > components = 
    getMCComponentsAndSizes(fileName,ntName,category,variable,bounds,cuts,scale);
  vector<TH1F*> histograms =
    getMCComponentsTH1F(fileName,ntName,category,variable,bounds,cuts,scale);

    // make a stack of MC components

  THStack* stack = new THStack("sDrawMCComponents","sDrawMCComponents");
  TLegend* legend = new TLegend(0.7,0.5,1.0,1.0);
  for (unsigned int i = 0; i < histograms.size(); i++){
    TH1F* hcomp = histograms[i];
    if (i != 0) hcomp->SetFillColor(i+1);
    hcomp->SetLineColor(i+1);
    stack->Add(hcomp,"hist");
    TString legendString("");
    legendString += FSModeString::rootSymbols(formatMCComponent(components[i].first,components[i].second));
    legend->AddEntry(hcomp,legendString,"F");
  }
  htot->Draw();
  if (histograms.size() != 0){
    stack->Draw("same");
    htot->Draw("same");
    legend->Draw("same");
  }
  return htot;
}

TString
FSModeHistogram::getMCComponentCut(TString component){
  TString mcCut("(1==1)");
  vector<TString> spacers; spacers.push_back(":"); spacers.push_back("_");
  vector<TString> parts = FSString::parseTString(component,spacers);
  if (parts.size() < 3) return mcCut;
  TString mcExtras = FSString::int2TString(FSString::TString2int(parts[0]));
  TString mcCode2  = FSString::int2TString(FSString::TString2int(parts[1]));
  TString mcCode1  = FSString::int2TString(FSString::TString2int(parts[2]));
  mcCut  = "((MCExtras==";
  mcCut += FSString::TString2int(mcExtras);
  mcCut += ")&&(MCDecayCode2==";
  mcCut += FSString::TString2int(mcCode2);
  mcCut += ")&&(MCDecayCode1==";
  mcCut += FSString::TString2int(mcCode1);
  mcCut += "))";
  return mcCut;
}


TString
FSModeHistogram::formatMCComponent(TString component, float fraction){
  TString output("");
  if (fraction >= 0.0){
    output += "(";
    output += FSString::double2TString(100*fraction,-2,false,true);
    output += ")";
  }
  vector<TString> parts1 = FSString::parseTString(component,":");
  vector<TString> parts2;  if (parts1.size() >= 1) parts2 = FSString::parseTString(parts1[0],"_");
  vector<TString> parts3;  if (parts1.size() == 2) parts3 = FSString::parseTString(parts1[1],"_");
  if (parts1.size() == 0 || parts1.size() > 2 || parts2.size() != 3){
    if (output != "") output += " ";
    output += component;
    return output;
  }
  TString finalState = FSModeInfo(parts1[0]).modeDescription();
  TString initialState("");
  for (unsigned int j = 0; j < parts3.size(); j++){
    if (parts3[j] == "0") continue;
    if (initialState != "") initialState += " ";
    initialState += FSPhysics::pdgName(FSString::TString2int(parts3[j]));
  }
  TString mcExtras = FSModeInfo::mcExtrasDescription(parts2[0]);
  if (initialState != ""){
    if (output != "") output += " ";
    output += initialState;
    output += " -> ";
  }
  output += finalState;
  if (mcExtras != ""){
    output += "(";
    output += mcExtras;
    output += ")";
  }
  return output;
}



void
FSModeHistogram::dumpComponentsCache(string cacheName){

    // open output file

  string compCacheName(cacheName);  compCacheName += ".cache.comp";
  ofstream compCache(compCacheName.c_str());

    // write to file

  for (map<TString, map<TString,float> >::const_iterator 
       mapItr = m_cacheComponentsMap.begin();
       mapItr != m_cacheComponentsMap.end(); mapItr++){
    compCache << mapItr->first << endl;
    map<TString,float> componentsMap = mapItr->second;
    compCache << componentsMap.size() << endl;
    for (map<TString,float>::const_iterator
         map2Itr = componentsMap.begin();
         map2Itr != componentsMap.end(); map2Itr++){
      compCache << map2Itr->first << endl;
      compCache << map2Itr->second << endl;
    }
  }

    // close file

  compCache.close();

}

void
FSModeHistogram::readComponentsCache(string cacheName){

    // open output file

  string compCacheName(cacheName);  compCacheName += ".cache.comp";
  ifstream compCache(compCacheName.c_str());

    // read from file

  TString index;
  while (compCache >> index){
    int num;
    compCache >> num;
    map<TString,float> components;
    for (int i = 0; i < num; i++){
      TString compname;
      float compvalue;
      compCache >> compname;
      compCache >> compvalue;
      components[compname] = compvalue;
    }
    m_cacheComponentsMap[index] = components;
  }

    // close file

  compCache.close();

}


  // ********************************************************
  // DUMP OR READ A HISTOGRAM CACHE TO/FROM A FILE
  // ********************************************************

void 
FSModeHistogram::dumpHistogramCache(string cacheName){
  FSHistogram::dumpHistogramCache(cacheName);
  dumpComponentsCache(cacheName);
}

void 
FSModeHistogram::readHistogramCache(string cacheName){
  FSHistogram::readHistogramCache(cacheName);
  readComponentsCache(cacheName);
}



  // ***********************************************
  // helper functions to interact with the FSHistogramInfo class
  // ***********************************************

vector<TString>
FSModeHistogram::expandHistogramIndexTree(TString inIndex){
    // expand FSCuts using the same method as FSHistogram
  vector<TString> expandedIndices = FSHistogram::expandHistogramIndexTree(inIndex);
    // if there are no modes, return
  if (!inIndex.Contains("{-CA-}")) return expandedIndices;
    // if there are modes, expand them
  vector<TString> expandedIndicesTemp = expandedIndices;
  expandedIndices.clear();
  for (unsigned int i = 0; i < expandedIndicesTemp.size(); i++){
    TString index = expandedIndicesTemp[i];
    map<TString,TString> indexMap = FSHistogram::parseHistogramIndex(index);
    if (indexMap.find("{-CA-}") == indexMap.end()){ 
      cout << "expandHistogramIndexTree ERROR (no category?)" << endl; exit(0); }
    TString category = indexMap["{-CA-}"];
    vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);
    if (modeVector.size() == 0){ 
      indexMap["{-CA-}"] = "!!NO_MODES!!";
      expandedIndices.push_back(FSHistogram::getHistogramIndex(indexMap));
      continue;
    }
    for (unsigned int iM = 0; iM < modeVector.size(); iM++){
      TString indexM = modeVector[iM]->modeString(index);
              indexM = modeVector[iM]->modeCuts(indexM);
      vector<TString> combos = modeVector[iM]->modeCombinatorics(indexM);
      for (unsigned int iC = 0; iC < combos.size(); iC++){
         // checking for duplicates (but this should also be in modeCombinatorics)
        bool usedIndex = false;
        for (unsigned int iE = 0; iE < expandedIndices.size(); iE++){
          if (expandedIndices[iE] == combos[iC]){ usedIndex = true; break; }
        }
        if (usedIndex) continue;
        map<TString,TString> comboMap = FSHistogram::parseHistogramIndex(combos[iC]);
        comboMap["{-CA-}"] = modeVector[iM]->modeString();
        expandedIndices.push_back(FSHistogram::getHistogramIndex(comboMap));
      }
    }
  }
  return expandedIndices;
}

pair<TString, vector<TString> >
FSModeHistogram::getHistogramIndexTree(int dimension,
                      TString fileName, TString ntName, TString category,
                      TString variable, TString bounds,
                      TString cuts, double scale){
  TString index;
  fileName = FSString::removeWhiteSpace(fileName);
  ntName   = FSString::removeWhiteSpace(ntName);
  category = FSString::removeWhiteSpace(category);
  variable = FSString::removeWhiteSpace(variable);
  bounds   = FSString::removeWhiteSpace(bounds);
  cuts     = FSString::removeWhiteSpace(cuts);
  index += "{-TP-}TREE";
  if (dimension == 1) index += "{-ND-}1D";
  if (dimension == 2) index += "{-ND-}2D";
  index += "{-FN-}";  index += fileName;
  index += "{-NT-}";  index += ntName;
  if (category != "" || FSModeCollection::modeVector().size() != 0){
  index += "{-CA-}";  index += category; }
  index += "{-VA-}";  index += FSTree::reorderVariable(variable);
  index += "{-BO-}";  index += bounds;
  index += "{-CU-}";  index += FSTree::reorderVariable(cuts);
  index += "{-SC-}";  index += FSString::double2TString(scale,8,true);
  vector<TString> subIndices = expandHistogramIndexTree(index);
  if (subIndices.size() == 1){ index = subIndices[0]; subIndices.clear(); }
  return pair<TString, vector<TString> > (index,subIndices);
}


