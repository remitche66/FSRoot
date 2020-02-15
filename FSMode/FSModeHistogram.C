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
map<TString, float> FSModeHistogram::m_mcComponentsMap;
map<TString, map<TString, float> > FSModeHistogram::m_cacheComponentsMap;



  // ********************************************************
  // CREATE A HISTOGRAM FROM A TREE
  // ********************************************************

TH1F* 
FSModeHistogram::getTH1F(TString fileName, TString ntName, TString category,
                       TString variable, TString bounds,
                       TString cuts,     TString options,
                       float scale){
  return getTHNF(1,fileName,ntName,category,
                             variable,bounds,cuts,options,scale,NULL).first;
}


TH2F* 
FSModeHistogram::getTH2F(TString fileName, TString ntName, TString category,
                       TString variable, TString bounds,
                       TString cuts,     TString options,
                       float scale){
  return getTHNF(2,fileName,ntName,category,
                             variable,bounds,cuts,options,scale,NULL).second;
}


pair<TH1F*,TH2F*> 
FSModeHistogram::getTHNF(int dimension,
                       TString fileName, TString ntName, TString category,
                       TString variable, TString bounds,
                       TString cuts,     TString options,
                       float scale,      TTree* histTree,
                       vector< pair<TString,TString> > extraTreeContents){


  vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);
  if (modeVector.size() == 0){
    cout << "FSModeHistogram:  there are no modes associated with this category..." << endl;
    cout << "                ... returning NULL" << endl;
  }

    // expand "cuts" using FSCut and check for multidimensional sidebands
    //   note: this section of code is the same as FSHistogram...
    //           it could be organized better, probably


  vector< pair<TString,double> > fsCuts = FSCut::expandCuts(cuts);
  if (fsCuts.size() == 1){ cuts = fsCuts[0].first; scale *= fsCuts[0].second; }


    // make histograms using multidimensional sidebands

  if (fsCuts.size() > 1){

      // loop over sideband cuts and add to a running total

    TH1F* hist1d = NULL;
    TH2F* hist2d = NULL;
    for (unsigned int i = 0; i < fsCuts.size(); i++){
      TString cuts_i = fsCuts[i].first;
      double scale_i = scale * fsCuts[i].second;
      pair<TH1F*,TH2F*> histPair = FSModeHistogram::getTHNF(dimension, fileName, ntName,
                       category, variable, bounds, cuts_i, options, scale_i, histTree,
                       extraTreeContents);
      if (dimension == 1){
        TH1F* hi = histPair.first;
        hist1d = FSHistogram::addTH1F("FSMODECUTTOTAL",hi);
      }
      if (dimension == 2){
        TH2F* hi = histPair.second;
        hist2d = FSHistogram::addTH2F("FSMODECUTTOTAL",hi);
      }
    }

      // clear the add cache and return new histograms

    pair<TH1F*,TH2F*> newHists = FSHistogram::addTempHistToCache(hist1d,hist2d);
    FSHistogram::clearAddCache("FSMODECUTTOTAL");
    return newHists;

  }


  // loop over all modes in this category

  TH1F* hist1d = NULL;
  TH2F* hist2d = NULL;

  for (unsigned int i = 0; i < modeVector.size(); i++){
    if (!FSControl::QUIET){cout << endl; modeVector[i]->display(i+1);}

    TString fileName_i = modeVector[i]->modeString(fileName);
    TString ntName_i   = modeVector[i]->modeString(ntName);
    TString variable_i = modeVector[i]->modeString(variable);
    TString cuts_i("");  if (cuts != "") cuts_i = modeVector[i]->modeString(cuts);
                                         cuts_i = modeVector[i]->modeCuts(cuts_i);

    vector<TString> indices;
    vector<TString> combinatorics = 
      modeVector[i]->modeCombinatorics((variable_i+" "+cuts_i),FSControl::DEBUG);

      // loop over all combinatorics within this mode

    for (unsigned int j = 0; j < combinatorics.size(); j++){

      TString variable_j("");
      TString cuts_j("");
      vector<TString> parts = FSString::parseTString(combinatorics[j]," ");
      if (parts.size() >= 1) variable_j = parts[0];
      if (parts.size() >= 2) cuts_j     = parts[1];

        // make histograms and add to the running total

      if (dimension == 1){
        TString index = FSHistogram::getTH1FIndex(fileName_i,ntName_i,variable_j,
                                                         bounds,cuts_j,options,scale);
        bool usedIndex = false;
        for (unsigned int iindex = 0; iindex < indices.size(); iindex++){
          if (indices[iindex] == index){ usedIndex = true; break; }
        }
        if (usedIndex) continue;
        indices.push_back(index);
        TH1F* hij = FSHistogram::getTHNF(dimension,fileName_i,ntName_i,variable_j,
                                              bounds,cuts_j,options,scale,histTree,
                                              extraTreeContents).first;
        hist1d = FSHistogram::addTH1F("MODEHISTOGRAMTOTAL",hij);
      }

      if (dimension == 2){
        TString index = FSHistogram::getTH2FIndex(fileName_i,ntName_i,variable_j,
                                                         bounds,cuts_j,options,scale);
        bool usedIndex = false;
        for (unsigned int iindex = 0; iindex < indices.size(); iindex++){
          if (indices[iindex] == index){ usedIndex = true; break; }
        }
        if (usedIndex) continue;
        indices.push_back(index);
        TH2F* hij = FSHistogram::getTHNF(dimension,fileName_i,ntName_i,variable_j,
                                              bounds,cuts_j,options,scale,histTree,
                                              extraTreeContents).second;
        hist2d = FSHistogram::addTH2F("MODEHISTOGRAMTOTAL",hij);
      }

    }
  }

    // make copies of the resulting totals and return

  pair<TH1F*,TH2F*> newHists = FSHistogram::addTempHistToCache(hist1d,hist2d);
  FSHistogram::clearAddCache("MODEHISTOGRAMTOTAL");
  return newHists;

}


      // ********************************************************
      // CREATE A TREE IN THE SAME WAY AS A HISTOGRAM
      // ********************************************************


TTree*
FSModeHistogram::getTH1FContents(TString fileName, TString ntName, TString category, 
       TString variable, TString bounds, TString cuts, TString options, float scale,
       vector< pair<TString,TString> > extraTreeContents){
  TTree* histTree = FSHistogram::setTHNFContents(1,extraTreeContents);
  getTHNF(1,fileName,ntName,category,variable,bounds,cuts,options,scale,histTree,extraTreeContents);
  return histTree;
}

TTree*
FSModeHistogram::getTH2FContents(TString fileName, TString ntName, TString category, 
       TString variable, TString bounds, TString cuts, TString options, float scale,
       vector< pair<TString,TString> > extraTreeContents){
  TTree* histTree = FSHistogram::setTHNFContents(2,extraTreeContents);
  getTHNF(2,fileName,ntName,category,variable,bounds,cuts,options,scale,histTree,extraTreeContents);
  return histTree;
}



  // ********************************************************
  // DRAW HISTOGRAMS SEPARATELY FOR EACH MODE
  // ********************************************************


void
FSModeHistogram::drawTH1F(TString fileName, TString ntName, TString category,
                        TString variable, TString bounds,
                        TString cuts,     TString options,
                        float scale, TCanvas* c1){
  drawTHNF(1,fileName,ntName,category,variable,bounds,cuts,options,scale,c1);
}

void
FSModeHistogram::drawTH2F(TString fileName, TString ntName, TString category,
                        TString variable, TString bounds,
                        TString cuts,     TString options,
                        float scale, TCanvas* c1){
  drawTHNF(2,fileName,ntName,category,variable,bounds,cuts,options,scale,c1);
}


void
FSModeHistogram::drawTHNF(int dimension,
                       TString fileName, TString ntName, TString category,
                       TString variable, TString bounds,
                       TString cuts,     TString options,
                       float scale, TCanvas* c1){

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
                       variable,bounds,cuts,options,scale);
      hist1d->SetTitle(title_i);
      hist1d->Draw(options);
    }
    if (dimension == 2){
      hist2d = getTH2F(fileName,ntName,category_i,
                       variable,bounds,cuts,options,scale);
      hist2d->SetTitle(title_i);
      hist2d->Draw(options);
    }

  }

  c1->cd();
  c1->Update();

}


  // ********************************************************
  // DECONSTRUCT A HISTOGRAM INTO ITS MC COMPONENTS
  //   (using MCDecayCode1 and MCDecayCode2)
  // ********************************************************


vector<TString>
FSModeHistogram::getMCComponents(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts,
                                TString options, float scale, bool moreInfo){
    // initial setup

  vector<TString> components;
  m_mcComponentsMap.clear();

    // create an index for caching

  TString index;
  index += "(fn)";  index += fileName;
  index += "(nt)";  index += ntName;
  index += "(ca)";  index += category;
  index += "(va)";  index += variable;
  index += "(bo)";  index += bounds;
  index += "(cu)";  index += cuts;
  index += "(op)";  index += options;  index += (int) moreInfo;
  index += "(sc)";  index += FSString::double2TString(scale,8,true);

    // search the cache for this mode

  map<TString, map<TString,float> >::const_iterator 
    mapItr = m_cacheComponentsMap.find(index);
  if (mapItr != m_cacheComponentsMap.end()){
    m_mcComponentsMap = m_cacheComponentsMap[index];
  }
  else{

      // create a new tree with MC information

    vector< pair<TString,TString> > extraTreeContents;
    extraTreeContents.push_back(pair<TString,TString>("MCDecayCode1","MCDecayCode1"));
    extraTreeContents.push_back(pair<TString,TString>("MCDecayCode2","MCDecayCode2"));
    if (moreInfo){
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle1","MCDecayParticle1"));
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle2","MCDecayParticle2"));
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle3","MCDecayParticle3"));
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle4","MCDecayParticle4"));
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle5","MCDecayParticle5"));
      extraTreeContents.push_back(pair<TString,TString>("MCDecayParticle6","MCDecayParticle6"));
    }
    TTree* histTree = getTH1FContents(fileName,ntName,category,"1.234","(10,1.0,2.0)",cuts,options,scale,extraTreeContents);

      // set the branch addresses for the new tree

    Double_t dcode1 = 0.0, dcode2 = 0.0;
    Double_t dp1 = 0.0, dp2 = 0.0, dp3 = 0.0, 
             dp4 = 0.0, dp5 = 0.0, dp6 = 0.0;
    histTree->SetBranchAddress("MCDecayCode1",&dcode1);
    histTree->SetBranchAddress("MCDecayCode2",&dcode2);
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
      TString modeString = FSModeInfo((int)dcode1,(int)dcode2).modeString();
      if (moreInfo){
        modeString += ":";
        modeString += (int)dp1;  modeString += "_";
        modeString += (int)dp2;  modeString += "_";
        modeString += (int)dp3;  modeString += "_";
        modeString += (int)dp4;  modeString += "_";
        modeString += (int)dp5;  modeString += "_";
        modeString += (int)dp6;
      }
      map<TString,float>::const_iterator mapItrx = m_mcComponentsMap.find(modeString);
      if (mapItrx == m_mcComponentsMap.end()){
        m_mcComponentsMap[modeString] = 0.0;
      }
      m_mcComponentsMap[modeString] += scale;
    }

      // cache the new m_mcComponentsMap

    m_cacheComponentsMap[index] = m_mcComponentsMap;

  }

    // fill the components vector (ordered from most to least populated)

  for (map<TString,float>::iterator mapItr1 = m_mcComponentsMap.begin();
       mapItr1 != m_mcComponentsMap.end(); mapItr1++){
    float max = 0;
    TString maxMode("");
    for (map<TString,float>::iterator mapItr2 = m_mcComponentsMap.begin();
      mapItr2 != m_mcComponentsMap.end(); mapItr2++){
      bool found = false;
      for (unsigned int i = 0; i < components.size(); i++){
        if (mapItr2->first == components[i]) found = true; 
      }
      if ((!found) && (mapItr2->second >= max)){
        max = mapItr2->second;
        maxMode = mapItr2->first;
      }
    }
    components.push_back(maxMode);
  }


    // print the results

  if (!FSControl::QUIET){
    cout << "****************" << endl;
    cout << "MC Components:" << endl;
    cout << "****************" << endl;
    for (unsigned int i = 0; i < components.size(); i++){
      TString component(components[i]);
      TString mString(component);
      TString extra("");
      if (moreInfo){
        vector<TString> twoParts = FSString::parseTString(component,":");
        if (twoParts.size() != 2){cout << "PROBLEM with moreInfo in getMCComponents" << endl; exit(0);}
        mString = twoParts[0];
        vector<TString> extras = FSString::parseTString(twoParts[1],"_");
        for (unsigned int j = 0; j < extras.size(); j++){
          if (extras[j] == "0") continue;
          extra += FSPhysics::pdgName(FSString::TString2int(extras[j]));
          if (j != extras.size()-1) extra += " ";
        }
      }
      cout << m_mcComponentsMap[component]
           << "\t\t" << extra
           << "\t\t" << FSModeInfo(mString).modeDescription() << endl;
    }
    cout << "****************" << endl;
  }

  return components;

}

float
FSModeHistogram::getMCComponentSize(TString modeString){
  map<TString,float>::const_iterator mapItr = m_mcComponentsMap.find(modeString);
  if (mapItr != m_mcComponentsMap.end()) return mapItr->second;
  return 0.0;
}


TH1F*
FSModeHistogram::drawMCComponents(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts, TString options,
                                float scale, TCanvas* c1){


    // create the original histogram

  vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);
  if (modeVector.size() == 0){
    cout << "FSModeHistogram::drawMCComponents ERROR: no modes for category = " << category << endl;
    exit(0);
  }
  TH1F* htot = getTH1F(fileName,ntName,category,variable,bounds,cuts,options,scale);
  FSModeInfo* modeInfo = modeVector[0];
  htot->SetTitle(FSModeString::rootSymbols(modeInfo->modeDescription()));
  if (modeVector.size() > 1) htot->SetTitle("category = "+category);
  float totsize = htot->Integral(1,FSString::parseBoundsNBinsX(bounds));

    // make a new TCanvas if one isn't passed in

  if (!c1) c1 = new TCanvas("cDrawMCComponents","cDrawMCComponents",600,600);

    // get a vector of the MC components

  vector<TString> components = getMCComponents(fileName,ntName,category,
                                               variable,bounds,cuts,options,scale);

    // make a stack of MC components

  THStack* stack = new THStack("sDrawMCComponents","sDrawMCComponents");
  TLegend* legend = new TLegend(0.7,0.5,1.0,1.0);

  for (unsigned int i = 0; i < components.size(); i++){

    double fraction = 100*getMCComponentSize(components[i])/totsize;
    if (fraction < 0.01 || i > 10) continue;

    TString mcCut(cuts);
    if (mcCut != "") mcCut += "&&";
    mcCut += "((MCDecayCode1==";
    mcCut += FSModeInfo(components[i]).modeCode1();
    mcCut += ")&&(MCDecayCode2==";
    mcCut += FSModeInfo(components[i]).modeCode2();
    mcCut += "))";

    TH1F* hcomp = getTH1F(fileName,ntName,category,variable,bounds,mcCut,options,scale);
    if (i != 0) hcomp->SetFillColor(i+1);
    hcomp->SetLineColor(i+1);
    stack->Add(hcomp,"hist");

    TString legendString("");
    legendString += "(";
    legendString += FSString::double2TString(fraction,-2,false,true);
    legendString += ") ";
    legendString += FSModeString::rootSymbols(FSModeInfo(components[i]).modeDescription());
    legend->AddEntry(hcomp,legendString,"F");

  }
  htot->Draw();
  stack->Draw("same");
  htot->Draw("same");
  legend->Draw("same");


    // print the results

  if (!FSControl::QUIET){
    cout << "****************" << endl;
    cout << "MC Components:" << endl;
    cout << "****************" << endl;
    for (unsigned int i = 0; i < components.size(); i++){
      cout << m_mcComponentsMap[components[i]]
           << "\t\t" << components[i]
           << "\t\t" << FSModeInfo(components[i]).modeDescription() << endl;
    }
    cout << "****************" << endl;
  }

  return htot;

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
  // helper functions for histogram indices
  // ***********************************************

vector<TString>
FSModeHistogram::expandHistogramIndex(TString index){
    // expand FSCuts using the same method as FSHistogram
  vector<TString> expandedIndices = FSHistogram::expandHistogramIndex(index);
    // expand modes
  vector<TString> expandedIndicesTemp = expandedIndices;
  expandedIndices.clear();
  for (unsigned int i = 0; i < expandedIndicesTemp.size(); i++){
    TString index = expandedIndicesTemp[i];
    map<TString,TString> indexMap = FSHistogram::parseHistogramIndex(index);
    if (indexMap.find("{-CA-}") == indexMap.end())
      { expandedIndices.push_back(index); continue; }
    TString category = indexMap["{-CA-}"];
    vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);
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
        comboMap.erase("{-CA-}");
        expandedIndices.push_back(FSHistogram::getHistogramIndex(comboMap));
      }
    }
  }
  return expandedIndices;
}



void 
FSModeHistogram::testTH1F(TString fileName, TString ntName, TString category,
                                TString variable, TString bounds,
                                TString cuts, double scale){
  TString index = FSHistogram::getHistogramIndexTree(1,fileName,ntName,variable,
                                                 bounds,cuts,scale,true,category);
  vector<TString> indices = expandHistogramIndex(index);
  for (unsigned int i = 0; i < indices.size(); i++){ FSHistogram::printIndexInfo(indices[i]); }
}

void 
FSModeHistogram::testTH2F(TString fileName, TString ntName, TString category,
                                TString variable, TString bounds,
                                TString cuts, double scale){
  TString index = FSHistogram::getHistogramIndexTree(2,fileName,ntName,variable,
                                                 bounds,cuts,scale,true,category);
  vector<TString> indices = expandHistogramIndex(index);
  for (unsigned int i = 0; i < indices.size(); i++){ FSHistogram::printIndexInfo(indices[i]); }
}


