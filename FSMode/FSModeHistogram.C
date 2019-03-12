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
                             variable,bounds,cuts,options,scale).first;
}


TH2F* 
FSModeHistogram::getTH2F(TString fileName, TString ntName, TString category,
                       TString variable, TString bounds,
                       TString cuts,     TString options,
                       float scale){
  return getTHNF(2,fileName,ntName,category,
                             variable,bounds,cuts,options,scale).second;
}


pair<TH1F*,TH2F*> 
FSModeHistogram::getTHNF(int dimension,
                       TString fileName, TString ntName, TString category,
                       TString variable, TString bounds,
                       TString cuts,     TString options,
                       float scale){


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
      if (dimension == 1){
        TH1F* hi = FSModeHistogram::getTH1F(fileName, ntName, category, variable, bounds,
                                        cuts_i, options, scale_i);
        hist1d = FSHistogram::addTH1F("FSMODECUTTOTAL",hi);
      }
      if (dimension == 2){
        TH2F* hi = FSModeHistogram::getTH2F(fileName, ntName, category, variable, bounds,
                                        cuts_i, options, scale_i);
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
        TH1F* hij = FSHistogram::getTH1F(fileName_i,ntName_i,variable_j,
                                                bounds,cuts_j,options,scale);
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
        TH2F* hij = FSHistogram::getTH2F(fileName_i,ntName_i,variable_j,
                                                bounds,cuts_j,options,scale);
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

      // extract FSModeInfo (only use the first mode from this category)
      //  and replace any special names

    vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);
    if (modeVector.size() == 0) return components;
    FSModeInfo* modeInfo = modeVector[0];
    fileName = modeInfo->modeString(fileName);
    ntName   = modeInfo->modeString(ntName);
    variable = modeInfo->modeString(variable);
    if (cuts != "") cuts = modeInfo->modeString(cuts);
                    cuts = modeInfo->modeCuts(cuts);

      // set up the original TChain

    TChain* nt = FSTree::getTChain(fileName,ntName);

      // loop over combinatorics

    vector<TString> indices;
    vector<TString> combinatorics = 
      modeInfo->modeCombinatorics((variable+" "+cuts),FSControl::DEBUG);
    for (unsigned int icombo = 0; icombo < combinatorics.size(); icombo++){

	// set up the variables and cuts for this combination

      TString variable_i("");
      TString cuts_i("");
      vector<TString> parts = FSString::parseTString(combinatorics[icombo]," ");
      if (parts.size() >= 1) variable_i = parts[0];
      if (parts.size() >= 2) cuts_i     = parts[1];
      variable_i = FSTree::expandVariable(variable_i);
      cuts_i = FSTree::expandVariable(cuts_i);

	// add cuts derived from bounds

      if (cuts_i != "") cuts_i += "&&";
      cuts_i += "(";
      cuts_i += variable_i;
      cuts_i += ">";
      cuts_i += FSString::double2TString(FSString::parseBoundsLowerX(bounds),8);
      cuts_i += ")&&(";
      cuts_i += variable_i;
      cuts_i += "<";
      cuts_i += FSString::double2TString(FSString::parseBoundsUpperX(bounds),8);
      cuts_i += ")";


        // make sure this combination isn't a duplicate

      TString checkIndex = FSHistogram::getTH1FIndex(fileName,ntName,variable_i,
                                                         bounds,cuts_i,options,scale);
      bool usedIndex = false;
      for (unsigned int iindex = 0; iindex < indices.size(); iindex++){
        if (indices[iindex] == checkIndex){ usedIndex = true; break; }
      }
      if (usedIndex) continue;
      indices.push_back(checkIndex);


	// set up a new TTree

      TFile* dummyTFile = new TFile("tempMCCompenents.root","recreate");
      dummyTFile->cd();
      TTree* nt2 = nt->CopyTree(cuts_i);
      Double_t dcode1 = 0.0, dcode2 = 0.0;
      Float_t fcode1 = 0.0, fcode2 = 0.0;
      Double_t dp1 = 0.0, dp2 = 0.0, dp3 = 0.0, 
               dp4 = 0.0, dp5 = 0.0, dp6 = 0.0;
      if (ntName.Contains("StandardDecayProc")){
        nt2->SetBranchAddress("MCDecayCode1",&fcode1);
        nt2->SetBranchAddress("MCDecayCode2",&fcode2);
      }
      else{
        nt2->SetBranchAddress("MCDecayCode1",&dcode1);
        nt2->SetBranchAddress("MCDecayCode2",&dcode2);
        if (moreInfo){
          nt2->SetBranchAddress("MCDecayParticle1",&dp1);
          nt2->SetBranchAddress("MCDecayParticle2",&dp2);
          nt2->SetBranchAddress("MCDecayParticle3",&dp3);
          nt2->SetBranchAddress("MCDecayParticle4",&dp4);
          nt2->SetBranchAddress("MCDecayParticle5",&dp5);
          nt2->SetBranchAddress("MCDecayParticle6",&dp6);
        }
      }

	// loop over the TTree and count MC components

      for (int ientry = 0; ientry < nt2->GetEntries(); ientry++){
	nt2->GetEntry(ientry);
        int code1;
        int code2;
        if (ntName.Contains("StandardDecayProc")){
          code1 = (int)fcode1;
          code2 = (int)fcode2;
        }
        else{
          code1 = (int)dcode1;
          code2 = (int)dcode2;
        }

	TString modeString = FSModeInfo(code1,code2).modeString();
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

      dummyTFile->Close();
      delete dummyTFile;

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
        for (unsigned int i = 0; i < extras.size(); i++){
          if (extras[i] == "0") continue;
          extra += FSPhysics::pdgName(FSString::TString2int(extras[i]));
          if (i != extras.size()-1) extra += " ";
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
  if (modeVector.size() != 1){
    cout << "FSModeHistogram:  you can only draw MC components for one mode at a time..." << endl;
    cout << "                ... there are " << modeVector.size() << " modes associated" << endl;
    cout << "                    with category " << category << endl << endl;
    FSModeCollection::display(category);
    return (TH1F*) NULL;
  }
  TH1F* htot = getTH1F(fileName,ntName,category,variable,bounds,cuts,options,scale);
  FSModeInfo* modeInfo = modeVector[0];
  htot->SetTitle(FSModeString::rootSymbols(modeInfo->modeDescription()));
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

