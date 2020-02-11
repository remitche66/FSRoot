#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "TChain.h"
#include "TChainElement.h"
#include "TObjArray.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTreeFormula.h"
#include "TRandom.h"
#include "FSBasic/FSControl.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSTree.h"
#include "FSBasic/FSCut.h"
#include "FSBasic/FSHistogram.h"



  // static member data

map< TString, FSHistogramInfo* >  FSHistogram::m_histogramInfoCache;
map< TString, pair<TH1F*,TH2F*> > FSHistogram::m_tempCache;
map< TString, pair<TH1F*,TH2F*> > FSHistogram::m_addCache;
unsigned int FSHistogram::m_indexFSRootHistName = 0;

// useful global strings

TString FSAND("&&");
TString FSOR("||");
TString FSNOT("!");



  // ********************************************************
  // SET HISTOGRAM ATTRIBUTES
  // ********************************************************

TH1F*
FSHistogram::getTH1F(TH1F* hist){
  if (hist){
    if (hist->GetSumw2N() == 0) hist->Sumw2();
    hist->SetDirectory(0);
    hist->SetStats(0);
    if (hist->GetMinimum() >= 0){
      hist->SetMinimum(0.0);
      double height =  hist->GetBinContent(hist->GetMaximumBin())
                     + hist->GetBinError(hist->GetMaximumBin());
      hist->SetMaximum(height*1.05);
    }
    if (hist->GetMaximum() == 0.0) hist->SetMaximum(1.0);
    hist->SetTitleOffset(1.5,"X");
    hist->SetTitleOffset(1.8,"Y");
    hist->SetLineColor(kBlack);
  }
  return hist;
}

TH2F* 
FSHistogram::getTH2F(TH2F* hist){
  if (hist){
    if (hist->GetSumw2N() == 0) hist->Sumw2();
    hist->SetDirectory(0);
    hist->SetStats(0);
    if (hist->GetMinimum() >= 0){
      hist->SetMinimum(0.0);
      double height =  hist->GetBinContent(hist->GetMaximumBin());
      hist->SetMaximum(height*1.05);
    }
    if (hist->GetMaximum() == 0.0) hist->SetMaximum(1.0);
    hist->SetTitleOffset(1.5,"X");
    hist->SetTitleOffset(1.8,"Y");
  }
  return hist;
}


  // ********************************************************
  // RETRIEVE A HISTOGRAM FROM A FILE AND CACHE IT
  // ********************************************************

TH1F* 
FSHistogram::getTH1F(TString fileName, TString histName){
  //return getTH1F(getTHNF(1,fileName,histName).first);
  TString index = getHistogramIndex(1,fileName,histName);
  return getHistogramInfo(index)->getTHNF().first;
}

TH2F* 
FSHistogram::getTH2F(TString fileName, TString histName){
//  return getTH2F(getTHNF(2,fileName,histName).second);
  TString index = getHistogramIndex(2,fileName,histName);
  return getHistogramInfo(index)->getTHNF().second;
}



  // ********************************************************
  // CREATE A HISTOGRAM FROM A TREE AND CACHE IT
  // ********************************************************


TH1F* 
FSHistogram::getTH1F(TString fileName, TString ntName,
                                   TString variable, TString bounds,
                                   TString cuts, double scale){
  //TH1F* hist = getTHNF(1,fileName,ntName,variable,bounds,cuts,scale,NULL).first;
  //return getTH1F(hist);
  TString index = getHistogramIndex(1,fileName,ntName,variable,bounds,cuts,scale);
  return getHistogramInfo(index)->getTHNF().first;
} 

TH2F* 
FSHistogram::getTH2F(TString fileName, TString ntName,
                                   TString variable, TString bounds,
                                   TString cuts, double scale){
  //TH2F* hist = getTHNF(2,fileName,ntName,variable,bounds,cuts,scale,NULL).second;
  //return getTH2F(hist);
  TString index = getHistogramIndex(2,fileName,ntName,variable,bounds,cuts,scale);
  return getHistogramInfo(index)->getTHNF().second;
} 





      // ********************************************************
      // CREATE A TREE IN THE SAME WAY AS A HISTOGRAM
      // ********************************************************


TTree*
FSHistogram::getTH1FContents(TString fileName, TString ntName, TString variable, TString bounds, 
                             TString cuts, double scale,
                             vector< pair<TString,TString> > extraTreeContents){
  //TTree* histTree = setTHNFContents(1,extraTreeContents);
  //getTHNF(1,fileName,ntName,variable,bounds,cuts,scale,histTree,extraTreeContents);
  //return histTree;
  TString index = getHistogramIndex(1,fileName,ntName,variable,bounds,cuts,scale);
  return getHistogramInfo(index)->getTHNFContents(NULL,extraTreeContents);
}

TTree*
FSHistogram::getTH2FContents(TString fileName, TString ntName, TString variable, TString bounds, 
                             TString cuts, double scale,
                             vector< pair<TString,TString> > extraTreeContents){
  //TTree* histTree = setTHNFContents(2,extraTreeContents);
  //getTHNF(2,fileName,ntName,variable,bounds,cuts,scale,histTree,extraTreeContents);
  //return histTree;
  TString index = getHistogramIndex(2,fileName,ntName,variable,bounds,cuts,scale);
  return getHistogramInfo(index)->getTHNFContents(NULL,extraTreeContents);
}




  // ********************************************************
  // GIVE HISTOGRAMS IN THE CACHE A UNIQUE NAME (private)
  // ********************************************************

TString
FSHistogram::makeFSRootHistName(){
  TString hname("FSRootHist");
  hname += FSString::int2TString(++m_indexFSRootHistName,9);
  return hname;
}


  // ********************************************************
  // GET A HISTOGRAM FROM THE CACHE OR RETURN NULL (private)
  // ********************************************************



  // ********************************************************
  // ADD A HISTOGRAM TO THE CACHE (private)
  // ********************************************************


pair<TH1F*,TH2F*>
FSHistogram::addTempHistToCache(TH1F* hist1d, TH2F* hist2d){
  if (FSControl::DEBUG){
    cout << "FSHistogram: adding a histogram to the temporary cache" << endl;
    if (hist1d){ cout << "  hist1d exists with "; 
                 cout << hist1d->GetEntries() << " entries" << endl; }
    if (hist2d){ cout << "  hist2d exists with " << endl; 
                 cout << hist2d->GetEntries() << " entries" << endl; }
  }
  TH1F* hist1dcopy = NULL;
  TH2F* hist2dcopy = NULL;
  TString histName = makeFSRootHistName();
  if (FSControl::DEBUG){ cout << "  new name = " << histName << endl; }
  if (hist1d){
    hist1dcopy = new TH1F(*hist1d);
    hist1dcopy->SetName(histName);
    hist1dcopy = FSHistogram::getTH1F(hist1dcopy);
  }
  if (hist2d){
    hist2dcopy = new TH2F(*hist2d);
    hist2dcopy->SetName(histName);
    hist2dcopy = FSHistogram::getTH2F(hist2dcopy);
  }
  m_tempCache[histName] = pair<TH1F*,TH2F*>(hist1dcopy,hist2dcopy);
  if (FSControl::DEBUG){ cout << "   added it to the cache" << endl; }
  return pair<TH1F*,TH2F*>(hist1dcopy,hist2dcopy);
}




    // ********************************************
    //  CREATE HISTOGRAMS FROM FUNCTIONS
    // ********************************************

TH1F*
FSHistogram::getTH1FFormula(TString formula, TString bounds, int numRandomTrials){
  return getTHNFFormula(1,formula,bounds,numRandomTrials).first;
}

TH2F*
FSHistogram::getTH2FFormula(TString formula, TString bounds, int numRandomTrials){
  return getTHNFFormula(2,formula,bounds,numRandomTrials).second;
}

TH1F*
FSHistogram::getTH1FFormula(TF1* function, TString bounds, int numRandomTrials){
  return getTHNFFormula(1,function->GetFormula()->GetExpFormula(),bounds,numRandomTrials).first;
}

TH2F*
FSHistogram::getTH2FFormula(TF2* function, TString bounds, int numRandomTrials){
  return getTHNFFormula(2,function->GetFormula()->GetExpFormula(),bounds,numRandomTrials).second;
}


pair<TH1F*,TH2F*>
FSHistogram::getTHNFFormulaRandom(int dimension, TString formula, TString bounds, int numRandomTrials){
  int nbins = 10000;  if (dimension == 2) nbins = 1000;
  int nbinsx = 1; int nbinsy = 1;
  double lowx = 0.0; double lowy = 0.0;
  double highx = 10.0; double highy = 10.0;
  if (dimension >= 1) nbinsx = FSString::parseBoundsNBinsX(bounds);
  if (dimension == 2) nbinsy = FSString::parseBoundsNBinsY(bounds);
  if (dimension >= 1)   lowx = FSString::parseBoundsLowerX(bounds);
  if (dimension == 2)   lowy = FSString::parseBoundsLowerY(bounds);
  if (dimension >= 1)  highx = FSString::parseBoundsUpperX(bounds);
  if (dimension == 2)  highy = FSString::parseBoundsUpperY(bounds);
  TString newBounds("");
  if (dimension == 1) newBounds = FSString::makeBounds(nbins,lowx,highx);
  if (dimension == 2) newBounds = FSString::makeBounds(nbins,lowx,highx,nbins,lowy,highy);
  pair<TH1F*,TH2F*> histMax = getTHNFFormula(dimension,formula,newBounds,-1);
  double fMax = 0.0;
  if (dimension == 1){
    TH1F* hist = histMax.first;
    fMax = hist->GetBinContent(1);
    for (int ix = 1; ix <= nbins; ix++){
      if (hist->GetBinContent(ix) > fMax) fMax = hist->GetBinContent(ix);
    }
  }
  if (dimension == 2){
    TH2F* hist = histMax.second;
    fMax = hist->GetBinContent(1,1);
    for (int ix = 1; ix <= nbins; ix++){
    for (int iy = 1; iy <= nbins; iy++){
      if (hist->GetBinContent(ix,iy) > fMax) fMax = hist->GetBinContent(ix,iy);
    }}
  }
  formula = FSString::removeWhiteSpace(formula);
  formula = FSString::expandSUM(formula);
  TFormula rootFormula("FSRootTempFormula",formula);
  TH1F* hist1d = NULL;  TH2F* hist2d = NULL;
  if (dimension == 1) hist1d = new TH1F("FSRootTempHist","",nbinsx,lowx,highx);
  if (dimension == 2) hist2d = new TH2F("FSRootTempHist","",nbinsx,lowx,highx,nbinsy,lowy,highy);
  int numAccepted = 0;
  int numAttempts = 0;
  while ((numAccepted < numRandomTrials) && (numAttempts < numRandomTrials*100000)){
    numAttempts++;
    double x0 = 0.0; double y0 = 0.0; double f0 = 0.0;  double f = 0.0;
    if (dimension >= 1) x0 = gRandom->Uniform(lowx,highx);
    if (dimension == 2) y0 = gRandom->Uniform(lowy,highy);
    f0 = gRandom->Uniform(0.0,fMax);
    if (dimension == 1) f = rootFormula.Eval(x0);
    if (dimension == 2) f = rootFormula.Eval(x0,y0);
    if (f0 <= f){
      numAccepted++;
      if (dimension == 1) hist1d->Fill(x0);
      if (dimension == 2) hist2d->Fill(x0,y0);
    }
  }
  if (hist1d) hist1d = FSHistogram::getTH1F(hist1d);
  if (hist2d) hist2d = FSHistogram::getTH2F(hist2d);
  if (hist1d){ hist1d->SetTitle("Random from f(x) = "+formula); hist1d->SetXTitle("x"); }
  if (hist2d){ hist2d->SetTitle("Random from f(x,y) = "+formula); hist2d->SetXTitle("x"); hist2d->SetYTitle("y"); }
  addTempHistToCache(hist1d,hist2d);
  return pair<TH1F*,TH2F*>(hist1d,hist2d);
}


pair<TH1F*,TH2F*>
FSHistogram::getTHNFFormula(int dimension, TString formula, TString bounds, int numRandomTrials){
  if (numRandomTrials > 0) return getTHNFFormulaRandom(dimension,formula,bounds,numRandomTrials);
  TString index = FSString::int2TString(dimension) + "D" +
                  "(form)" + formula +
                  "(bounds)" + bounds;
  pair<TH1F*,TH2F*> histPair;// = getHistogramFromCache(index);
  if (histPair.first || histPair.second) return histPair;
  formula = FSString::removeWhiteSpace(formula);
  formula = FSString::expandSUM(formula);
  TFormula rootFormula("FSRootTempFormula",formula);
  int nbinsx = 1; int nbinsy = 1;
  double lowx = 0.0; double lowy = 0.0;
  double highx = 10.0; double highy = 10.0;
  if (dimension >= 1) nbinsx = FSString::parseBoundsNBinsX(bounds);
  if (dimension == 2) nbinsy = FSString::parseBoundsNBinsY(bounds);
  if (dimension >= 1)   lowx = FSString::parseBoundsLowerX(bounds);
  if (dimension == 2)   lowy = FSString::parseBoundsLowerY(bounds);
  if (dimension >= 1)  highx = FSString::parseBoundsUpperX(bounds);
  if (dimension == 2)  highy = FSString::parseBoundsUpperY(bounds);
  TH1F* hist1d = NULL;  TH2F* hist2d = NULL;
  if (dimension == 1) hist1d = new TH1F("FSRootTempHist","",nbinsx,lowx,highx);
  if (dimension == 2) hist2d = new TH2F("FSRootTempHist","",nbinsx,lowx,highx,nbinsy,lowy,highy);
  double x = 0.0; double y = 0.0;  double f = 0.0;
  for (int ix = 1; ix <= nbinsx; ix++){
    for (int iy = 1; iy <= nbinsy; iy++){
      if (dimension >= 1) x = lowx + (highx-lowx)*(ix-0.5)/nbinsx;
      if (dimension == 2) y = lowy + (highy-lowy)*(iy-0.5)/nbinsy;
      if (dimension == 1) f = rootFormula.Eval(x);
      if (dimension == 2) f = rootFormula.Eval(x,y);
      if (dimension == 1){ hist1d->SetBinContent(ix,   f); hist1d->SetBinError(ix,   0.0); }
      if (dimension == 2){ hist2d->SetBinContent(ix,iy,f); hist2d->SetBinError(ix,iy,0.0); }
    }
  }
  hist1d = FSHistogram::getTH1F(hist1d);
  hist2d = FSHistogram::getTH2F(hist2d);
  if (hist1d){ hist1d->SetTitle("f(x) = "+formula); hist1d->SetXTitle("x"); }
  if (hist2d){ hist2d->SetTitle("f(x,y) = "+formula); hist2d->SetXTitle("x"); hist2d->SetYTitle("y"); }
  //addHistogramToCache(index, hist1d, hist2d);
  return pair<TH1F*,TH2F*>(hist1d,hist2d);
}


  // ********************************************
  //  MAKE PROJECTIONS
  // ********************************************

TH1F*
FSHistogram::getTH1F(TH2F* hist2d, TString projectionAxis, bool function){
  TH1F* hist = NULL;
  if (projectionAxis == "x") projectionAxis = "X";
  if (projectionAxis == "y") projectionAxis = "Y";
  if ((projectionAxis != "X") && (projectionAxis != "Y")) return hist;
  int nbins1 = hist2d->GetNbinsX();
  int nbins2 = hist2d->GetNbinsY();
  double low1  = getLowX(hist2d);
  double high1 = getHighX(hist2d);
  double low2  = getLowY(hist2d);
  double high2 = getHighY(hist2d);
  if (projectionAxis == "Y"){
    int iswap = nbins1; nbins1 = nbins2; nbins2 = iswap;
    double dswap = low1; low1 = low2; low2 = dswap;
    dswap = high1; high1 = high2; high2 = dswap;
  }
  hist = new TH1F("histProject","",nbins1,low1,high1);
  for (int i1 = 1; i1 <= nbins1; i1++){
    double proj = 0.0;
    double eproj = 0.0;
    double eproj2 = 0.0;
    for (int i2 = 1; i2 <= nbins2; i2++){
      int ix = i1; int iy = i2;
      if (projectionAxis == "Y"){ ix = i2; iy = i1; }
      proj += hist2d->GetBinContent(ix,iy); 
      eproj = hist2d->GetBinError(ix,iy);
      eproj2 += (eproj*eproj);
    }
    eproj = sqrt(eproj2);
    if (function){ proj = proj/nbins2*(high2-low2); eproj = eproj/nbins2*(high2-low2); }
    hist->SetBinContent(i1,proj);
    hist->SetBinError(i1,eproj);
  }
  addTempHistToCache(hist,NULL);
  hist = getTH1F(hist);
  hist->SetTitle(hist2d->GetTitle());
  if (projectionAxis == "X"){ TAxis* axis = hist2d->GetXaxis(); hist->SetXTitle(axis->GetTitle()); }
  if (projectionAxis == "Y"){ TAxis* axis = hist2d->GetYaxis(); hist->SetXTitle(axis->GetTitle()); }
  return hist;
}

pair<double,double>
FSHistogram::integral(TH1F* hist, bool function){
  if (!hist) return pair<double,double> (0.0,0.0);
  int nbins = hist->GetNbinsX();
  double low  = getLowX(hist);
  double high = getHighX(hist);
  double total = 0.0;
  double etotal = 0.0;
  double etotal2 = 0.0;
  for (int i = 1; i <= nbins; i++){
    total += hist->GetBinContent(i);
    etotal = hist->GetBinError(i);
    etotal2 += etotal*etotal;
  }
  etotal = sqrt(etotal2);
  if (function){ total = total/nbins*(high-low); etotal = etotal/nbins*(high-low); }
  return pair<double,double> (total,etotal);
}

pair<double,double>
FSHistogram::integral(TH2F* hist, bool function){
  if (!hist) return pair<double,double> (0.0,0.0);
  TH1F* hist1d = getTH1F(hist,"X",function);
  return integral(hist1d,function);
}


  // ********************************************************
  // GIVE A SET OF HISTOGRAMS A COMMON MAXIMUM
  // ********************************************************

void 
FSHistogram::setHistogramMaxima(TH1F* h1, TH1F* h2, TH1F* h3, TH1F* h4, 
                                       TH1F* h5, TH1F* h6, TH1F* h7, TH1F* h8, 
                                       TH1F* h9, TH1F* h10){
  vector<TH1F*> histVector;
  if (h1)  histVector.push_back(h1);
  if (h2)  histVector.push_back(h2);
  if (h3)  histVector.push_back(h3);
  if (h4)  histVector.push_back(h4);
  if (h5)  histVector.push_back(h5);
  if (h6)  histVector.push_back(h6);
  if (h7)  histVector.push_back(h7);
  if (h8)  histVector.push_back(h8);
  if (h9)  histVector.push_back(h9);
  if (h10) histVector.push_back(h10);
  float max = -1.0e99;
  for (unsigned int i = 0; i < histVector.size(); i++){
    TH1F* hist = histVector[i];
    for (int j = 1; j <= hist->GetNbinsX(); j++){
      float max0 = hist->GetBinContent(j) + hist->GetBinError(j);
      if (max0 > max) max = max0;
    }
  }
  for (unsigned int i = 0; i < histVector.size(); i++){
    TH1F* hist = histVector[i];
    if (max > 0) hist->SetMaximum(max*1.05);
    if (max < 0) hist->SetMaximum(max*0.9);
    //hist->SetMaximum(max);
  }
}

  // ********************************************************
  // GIVE A SET OF HISTOGRAMS A COMMON MINIMUM
  // ********************************************************

void 
FSHistogram::setHistogramMinima(TH1F* h1, TH1F* h2, TH1F* h3, TH1F* h4, 
                                       TH1F* h5, TH1F* h6, TH1F* h7, TH1F* h8, 
                                       TH1F* h9, TH1F* h10){
  vector<TH1F*> histVector;
  if (h1)  histVector.push_back(h1);
  if (h2)  histVector.push_back(h2);
  if (h3)  histVector.push_back(h3);
  if (h4)  histVector.push_back(h4);
  if (h5)  histVector.push_back(h5);
  if (h6)  histVector.push_back(h6);
  if (h7)  histVector.push_back(h7);
  if (h8)  histVector.push_back(h8);
  if (h9)  histVector.push_back(h9);
  if (h10) histVector.push_back(h10);
  float min = 1.0e99;
  for (unsigned int i = 0; i < histVector.size(); i++){
    TH1F* hist = histVector[i];
    for (int j = 1; j <= hist->GetNbinsX(); j++){
      float min0 = hist->GetBinContent(j) - hist->GetBinError(j);
      if (min0 < min) min = min0;
    }
  }
  for (unsigned int i = 0; i < histVector.size(); i++){
    TH1F* hist = histVector[i];
    if (min > 0) hist->SetMinimum(min*0.9);
    if (min < 0) hist->SetMinimum(min*1.1);
  }
}


      // ********************************************************
      // GET USEFUL INFORMATION ABOUT A HISTOGRAM
      // ********************************************************

int
FSHistogram::getNBins(TH1F* hist){
  return hist->GetNbinsX();
}

int
FSHistogram::getNBinsX(TH1F* hist){
  return hist->GetNbinsX();
}

int
FSHistogram::getNBinsX(TH2F* hist){
  return hist->GetNbinsX();
}

int
FSHistogram::getNBinsY(TH2F* hist){
  return hist->GetNbinsY();
}

double
FSHistogram::getLow  (TH1F* hist){
  return hist->GetBinLowEdge(1);
}

double
FSHistogram::getLowX (TH1F* hist){
  return hist->GetBinLowEdge(1);
}

double
FSHistogram::getLowX (TH2F* hist){
  TAxis* axis = hist->GetXaxis();
  return axis->GetBinLowEdge(1);
}

double
FSHistogram::getLowY (TH2F* hist){
  TAxis* axis = hist->GetYaxis();
  return axis->GetBinLowEdge(1);
}

double
FSHistogram::getHigh (TH1F* hist){
  return hist->GetBinLowEdge(hist->GetNbinsX())+hist->GetBinWidth(hist->GetNbinsX());
}

double
FSHistogram::getHighX(TH1F* hist){
  return hist->GetBinLowEdge(hist->GetNbinsX())+hist->GetBinWidth(hist->GetNbinsX());
}

double
FSHistogram::getHighX(TH2F* hist){
  TAxis* axis = hist->GetXaxis();
  return axis->GetBinLowEdge(hist->GetNbinsX())+axis->GetBinWidth(hist->GetNbinsX());
}

double
FSHistogram::getHighY(TH2F* hist){
  TAxis* axis = hist->GetYaxis();
  return axis->GetBinLowEdge(hist->GetNbinsY())+axis->GetBinWidth(hist->GetNbinsY());
}


  // ********************************************************
  // DUMP THE HISTOGRAM CACHE
  // ********************************************************

void 
FSHistogram::dumpHistogramCache(string cacheName){

    // open output files

  string textCacheName(cacheName);  textCacheName += ".cache.dat";
  string rootCacheName(cacheName);  rootCacheName += ".cache.root";
  TFile* rootCache = new TFile(rootCacheName.c_str(),"recreate");
  ofstream textCache(textCacheName.c_str());

    // write to files

  for (map<TString,FSHistogramInfo*>::iterator mapItr = m_histogramInfoCache.begin();
       mapItr != m_histogramInfoCache.end(); mapItr++){
    rootCache->cd();
    TString histIndex = mapItr->first;
    FSHistogramInfo* histInfo = mapItr->second;
    if (histInfo->m_expandedIndices.size() > 0) continue;
    pair<TH1F*,TH2F*> histPair = histInfo->getTHNF();
    if (histPair.first || histPair.second) 
      textCache << histIndex << endl;
    if (histPair.first) { 
      textCache << histPair.first->GetName() << endl;
      histPair.first->Write();
    }
    if (histPair.second) { 
      textCache << histPair.second->GetName() << endl;
      histPair.second->Write();
    }
  }

/*
  for (map<TString,pair<TH1F*,TH2F*> >::const_iterator mapItr = m_histogramCache.begin();
       mapItr != m_histogramCache.end(); mapItr++){
    rootCache->cd();
    textCache << mapItr->first << endl;
    if (mapItr->second.first) { 
      textCache << mapItr->second.first->GetName() << endl;
      mapItr->second.first->Write();
    }
    if (mapItr->second.second) { 
      textCache << mapItr->second.second->GetName() << endl;
      mapItr->second.second->Write();
    }
  }
*/

    // close files

  textCache.close();
  rootCache->Close();
  delete rootCache;

}



  // ********************************************************
  // READ THE HISTOGRAM CACHE
  // ********************************************************

void 
FSHistogram::readHistogramCache(string cacheName){

    // open input files

  string textCacheName(cacheName);  textCacheName += ".cache.dat";
  string rootCacheName(cacheName);  rootCacheName += ".cache.root";
  TFile* rootCache = new TFile(rootCacheName.c_str());
  ifstream textCache(textCacheName.c_str());

    // read histograms

  TString name;
  TString index;
  while (textCache >> index){
    textCache >> name;
    if (FSControl::DEBUG) cout << "FSHistogram: reading cache" << endl;
    if (FSControl::DEBUG) cout << "\tname  = " << name << endl;
    if (FSControl::DEBUG) cout << "\tindex = " << index << endl;
    int dim = 1;
    if (index.Contains("{-ND-}1D")) dim = 1;
    if (index.Contains("{-ND-}2D")) dim = 2;
    TString tempIndex = getHistogramIndex(dim,FSString::string2TString(rootCacheName),name);
    FSHistogramInfo* tempHistInfo = getHistogramInfo(tempIndex,false);
    pair<TH1F*,TH2F*> histPair = tempHistInfo->getTHNF();
    FSHistogramInfo* histInfo = getHistogramInfo(index);
    histInfo->m_hist1d = histPair.first;
    histInfo->m_hist2d = histPair.second;
    delete tempHistInfo;
/*
    int dim = 1;
    if (index.Index("2") == 0) dim = 2;
    getTHNF(dim,FSString::string2TString(rootCacheName),name,index);
*/
  }

    // close files

  textCache.close();
  rootCache->Close();
  delete rootCache;

}



  // ********************************************************
  // GET (OR CREATE) THE CACHING INDEX ASSOCIATED WITH A HISTOGRAM
  // ********************************************************



TString
FSHistogram::getHistogramIndex(int dimension,
                      TString fileName, TString ntName,
                      TString variable, TString bounds,
                      TString cuts, double scale, 
                      bool useCat, TString cat){
  TString index;
  fileName = FSString::removeWhiteSpace(fileName);
  ntName   = FSString::removeWhiteSpace(ntName);
  variable = FSString::removeWhiteSpace(variable);
  bounds   = FSString::removeWhiteSpace(bounds);
  cuts     = FSString::removeWhiteSpace(cuts);
  cat      = FSString::removeWhiteSpace(cat);
  if (dimension == 1) index += "{-ND-}1D";
  if (dimension == 2) index += "{-ND-}2D";
  index += "{-FN-}";  index += fileName;
  index += "{-NT-}";  index += ntName;
  if (useCat)
  index += "{-CA-}";  index += cat;
  index += "{-VA-}";  index += FSTree::expandVariable(variable);
  index += "{-BO-}";  index += bounds;
  index += "{-CU-}";  index += FSTree::expandVariable(cuts);
  index += "{-SC-}";  index += FSString::double2TString(scale,8,true);
  return index;
}

TString
FSHistogram::getHistogramIndex(int dimension,
                      TString fileName, TString histName){
  TString index;
  fileName = FSString::removeWhiteSpace(fileName);
  histName = FSString::removeWhiteSpace(histName);
  if (dimension == 1) index += "{-ND-}1D";
  if (dimension == 2) index += "{-ND-}2D";
  index += "{-FN-}";  index += fileName;
  index += "{-HN-}";  index += histName;
  return index;
}

TString
FSHistogram::getHistogramIndex(map<TString,TString> indexMap){
  TString index;
  if (indexMap.find("{-ND-}") != indexMap.end()){ index += "{-ND-}"; index += indexMap["{-ND-}"]; }
  if (indexMap.find("{-FN-}") != indexMap.end()){ index += "{-FN-}"; index += indexMap["{-FN-}"]; }
  if (indexMap.find("{-HN-}") != indexMap.end()){ index += "{-HN-}"; index += indexMap["{-HN-}"]; }
  if (indexMap.find("{-NT-}") != indexMap.end()){ index += "{-NT-}"; index += indexMap["{-NT-}"]; }
  if (indexMap.find("{-CA-}") != indexMap.end()){ index += "{-CA-}"; index += indexMap["{-CA-}"]; }
  if (indexMap.find("{-VA-}") != indexMap.end()){ index += "{-VA-}"; index += indexMap["{-VA-}"]; }
  if (indexMap.find("{-BO-}") != indexMap.end()){ index += "{-BO-}"; index += indexMap["{-BO-}"]; }
  if (indexMap.find("{-CU-}") != indexMap.end()){ index += "{-CU-}"; index += indexMap["{-CU-}"]; }
  if (indexMap.find("{-SC-}") != indexMap.end()){ index += "{-SC-}"; index += indexMap["{-SC-}"]; }
  return index;
}

map<TString,TString>
FSHistogram::parseHistogramIndex(TString index){
  index = FSString::removeWhiteSpace(index);
  vector<TString> spacers; 
  spacers.push_back("{-ND-}");
  spacers.push_back("{-FN-}");
  spacers.push_back("{-HN-}");
  spacers.push_back("{-NT-}");
  spacers.push_back("{-CA-}");
  spacers.push_back("{-VA-}");
  spacers.push_back("{-BO-}");
  spacers.push_back("{-CU-}");
  spacers.push_back("{-SC-}");
  map<TString,TString> mapWords = FSString::parseTStringToMap1(index,spacers);
  return mapWords;
}  

FSHistogramInfo*
FSHistogram::getHistogramInfo(TString index, bool cache, vector<TString> expandedIndices){
  if (m_histogramInfoCache.find(index) != m_histogramInfoCache.end())
    return m_histogramInfoCache[index];
  if (expandedIndices.size() == 0){ expandedIndices = expandIndices(index); }
  if (expandedIndices.size() == 0){ cout << "FSHistogram::getHistogramInfo ERROR" << endl; exit(0); }
  if (expandedIndices.size() == 1){ index = expandedIndices[0];  expandedIndices.clear(); }
  FSHistogramInfo* histInfo = new FSHistogramInfo(index,expandedIndices);
  if (cache) m_histogramInfoCache[index] = histInfo;
  return histInfo;
}

vector<TString>
FSHistogram::expandIndices(TString index){
  vector<TString> expandedIndices;

    // (1) expand "cuts" using FSCut and check for multidimensional sidebands

  map<TString,TString> indexMap = FSHistogram::parseHistogramIndex(index);
  if (indexMap.find("{-CU-}") != indexMap.end()){
    TString cuts = indexMap["{-CU-}"];  double scale = 1.0;
    if (indexMap.find("{-SC-}") != indexMap.end()) 
      scale = FSString::TString2double(indexMap["{-SC-}"]);
    vector< pair<TString,double> > fsCuts = FSCut::expandCuts(cuts);
    for (unsigned int i = 0; i < fsCuts.size(); i++){
      indexMap["{-CU-}"] = fsCuts[i].first;
      indexMap["{-SC-}"] = FSString::double2TString(fsCuts[i].second*scale,8,true);
      expandedIndices.push_back(FSHistogram::getHistogramIndex(indexMap));
    }
  }
  if (expandedIndices.size() == 0) expandedIndices.push_back(index);


    // (2) expand mode categories -- this needs to go elsewhere

/*
  vector<TString> expandedIndicesTemp = expandedIndices;
  expandedIndices.clear();
  for (unsigned int i = 0; i < expandedIndicesTemp.size(); i++){
    TString indexTemp = expandedIndicesTemp[i];
    map<TString,TString> indexTempMap = FSHistogram::parseHistogramIndex(indexTemp);
    if (indexTempMap.find("{-CA-}") == indexTempMap.end())
      { expandedIndices.push_back(indexTemp); continue; }
    TString category = indexTempMap["{-CA-}"];
    vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);
    for (unsigned int iM = 0; iM < modeVector.size(); iM++){
      indexTemp = modeVector[iM]->modeString(indexTemp);
      indexTemp = modeVector[iM]->modeCuts(indexTemp);
      vector<TString> combinatorics = modeVector[i]->modeCombinatorics(indexTemp);
      for (unsigned int iC = 0; iC < combinatorics.size(); iC++){
        bool usedIndex = false;
        for (unsigned int iE = 0; iE < expandedIndices.size(); iE++){
          if (expandedIndices[iE] == indexTemp){ usedIndex = true; break; }
        }
        if (!usedIndex) expandedIndices.push_back(indexTemp);
      }
    }
  }
*/

    // (3) replace strings like MASS(xxxx), etc.

  if (indexMap.find("{-VA-}") != indexMap.end()){
    for (unsigned int i = 0; i < expandedIndices.size(); i++){
      expandedIndices[i] = FSTree::expandVariable(expandedIndices[i]);
    }
  }

  return expandedIndices;
} 





/*
TString
FSHistogram::getIndex(TH1F* hist1d){
  TH2F* hist2d = NULL;  return getIndex(pair<TH1F*,TH2F*>(hist1d,hist2d));
}

TString
FSHistogram::getIndex(TH2F* hist2d){
  TH1F* hist1d = NULL;  return getIndex(pair<TH1F*,TH2F*>(hist1d,hist2d));
}

TString
FSHistogram::getIndex(pair<TH1F*,TH2F*> hist){
  for (map< TString, pair<TH1F*,TH2F*> >::const_iterator mItr = m_histogramCache.begin();
       mItr != m_histogramCache.end(); mItr++){
    if (mItr->second == hist) return mItr->first;
  }
  return TString("");
}
*/


  // ********************************************************
  // KEEP RUNNING HISTOGRAMS
  // ********************************************************

TH1F*
FSHistogram::addTH1F(TString addName, TH1F* hist, double scale){
  TH2F* hist2d = NULL;
  return addTHNF(addName,hist,hist2d,scale).first;
}

TH2F*
FSHistogram::addTH2F(TString addName, TH2F* hist, double scale){
  TH1F* hist1d = NULL;
  return addTHNF(addName,hist1d,hist,scale).second;
}

pair<TH1F*,TH2F*>
FSHistogram::addTHNF(TString addName, TH1F* hist1d, TH2F* hist2d, double scale){

    // the original histogram

  TH1F* hist1d0 = NULL;
  TH2F* hist2d0 = NULL;

    // first search for the original histogram in the cache

  map<TString, pair<TH1F*,TH2F*> >::const_iterator mapItr = m_addCache.find(addName);
  if (mapItr != m_addCache.end()){
    pair<TH1F*,TH2F*> histPair = m_addCache[addName];
    hist1d0 = histPair.first;
    hist2d0 = histPair.second;
  }

    // if found in cache, add hist to original and return

  if (hist1d0 || hist2d0){
    if (hist1d0) hist1d0->Add(hist1d,scale);
    if (hist2d0) hist2d0->Add(hist2d,scale);
    return pair<TH1F*,TH2F*>(hist1d0,hist2d0);
  }

    // if not found in cache, add hist to cache and return

  if (hist1d){ 
    hist1d0 = getTH1F(new TH1F(*hist1d));
    hist1d0->SetName(makeFSRootHistName());  
    hist1d0->Scale(scale); 
  }
  if (hist2d){
    hist2d0 = getTH2F(new TH2F(*hist2d));
    hist2d0->SetName(makeFSRootHistName());  
    hist2d0->Scale(scale); 
  }

  m_addCache[addName] = pair<TH1F*,TH2F*>(hist1d0,hist2d0);
  return m_addCache[addName];

}
  


  // ********************************************************
  // CLEAR GLOBAL CACHES
  // ********************************************************


void
FSHistogram::clearHistogramCache(){
  if (FSControl::DEBUG) 
    cout << "FSHistogram: clearing histogram cache" << endl;
  for (map<TString,FSHistogramInfo*>::iterator rmItr = m_histogramInfoCache.begin();
       rmItr != m_histogramInfoCache.end(); rmItr++){
    if (rmItr->second && rmItr->second->m_hist1d) delete rmItr->second->m_hist1d;
    if (rmItr->second && rmItr->second->m_hist2d) delete rmItr->second->m_hist2d;
    if (rmItr->second) delete rmItr->second; 
 }
  m_histogramInfoCache.clear();
  if (FSControl::DEBUG) 
    cout << "FSHistogram: done clearing histogram cache" << endl;
}

void
FSHistogram::clearTempHistCache(){
  if (FSControl::DEBUG) 
    cout << "FSHistogram: clearing temp histogram cache" << endl;
  for (map<TString,pair<TH1F*,TH2F*> >::iterator rmItr = m_tempCache.begin();
       rmItr != m_tempCache.end(); rmItr++){
    if (rmItr->second.first) delete rmItr->second.first;
    if (rmItr->second.second) delete rmItr->second.second;
  }
  m_tempCache.clear();
  if (FSControl::DEBUG) 
    cout << "FSHistogram: done clearing temp histogram cache" << endl;
}


void
FSHistogram::clearAddCache(TString addName){
  if (FSControl::DEBUG) 
    cout << "FSHistogram: clearing add cache " << addName << endl;
  if (addName != ""){
    map<TString,pair<TH1F*,TH2F*> >::iterator rmItr = m_addCache.find(addName);
    if (rmItr != m_addCache.end()){
      if (rmItr->second.first) delete rmItr->second.first;
      if (rmItr->second.second) delete rmItr->second.second;
      m_addCache.erase(rmItr);
    }
  }
  else{
    for (map<TString,pair<TH1F*,TH2F*> >::iterator rmItr = m_addCache.begin();
         rmItr != m_addCache.end(); rmItr++){
      if (rmItr->second.first) delete rmItr->second.first;
      if (rmItr->second.second) delete rmItr->second.second;
    }
    m_addCache.clear();
  }
  if (FSControl::DEBUG) 
    cout << "FSHistogram: done clearing add cache" << endl;
}







FSHistogramInfo::FSHistogramInfo(TString index, vector<TString> expandedIndices){
  m_index = index;
  m_expandedIndices = expandedIndices;
  m_histName = FSHistogram::makeFSRootHistName();
  m_hist1d = NULL;
  m_hist2d = NULL;
  for (unsigned int i = 0; i < expandedIndices.size(); i++){
    FSHistogram::getHistogramInfo(expandedIndices[i]);
  }
}




pair<TH1F*,TH2F*>
FSHistogramInfo::getTHNF(){

    // if the histogram is already created, return it
  if (m_hist1d || m_hist2d){
    cout << "----  FSHistogramInfo::getTHNF  FOUND  ---- " << endl;
    cout << "   index = " << m_index << endl;
    return pair<TH1F*,TH2F*> (m_hist1d,m_hist2d);
  }

  cout << "----  FSHistogramInfo::getTHNF  CREATING  ---- " << endl;
  cout << "   index = " << m_index << endl;

    // get information about this histogram from the index
  map<TString,TString> indexMap = FSHistogram::parseHistogramIndex(m_index);

    // if the histogram is coming from a file, get it and return
  if ((indexMap.find("{-ND-}") != indexMap.end()) &&
      (indexMap.find("{-FN-}") != indexMap.end()) &&
      (indexMap.find("{-HN-}") != indexMap.end())){
    int dimension = FSString::TString2int(indexMap["{-ND-}"]);
    TString fileName = indexMap["{-FN-}"];
    TString histName = indexMap["{-HN-}"];
    TFile* tf = FSTree::getTFile(fileName);  tf->cd();
    TH1F* hist1d0 = NULL;  TH2F* hist2d0 = NULL;
    if (dimension == 1) hist1d0 = (TH1F*) gDirectory->FindObjectAny(histName);
    if (dimension == 2) hist2d0 = (TH2F*) gDirectory->FindObjectAny(histName);
    if (hist1d0){ m_hist1d = new TH1F(*hist1d0); m_hist1d = FSHistogram::getTH1F(m_hist1d); }
    if (hist2d0){ m_hist2d = new TH2F(*hist2d0); m_hist2d = FSHistogram::getTH2F(m_hist2d); }
    return pair<TH1F*,TH2F*>(m_hist1d,m_hist2d);
  }

    // if the histogram is coming from a tree, get it and return
  if ((indexMap.find("{-ND-}") != indexMap.end()) &&
      (indexMap.find("{-FN-}") != indexMap.end()) &&
      (indexMap.find("{-NT-}") != indexMap.end()) &&
      (indexMap.find("{-VA-}") != indexMap.end()) &&
      (indexMap.find("{-BO-}") != indexMap.end()) &&
      (indexMap.find("{-CU-}") != indexMap.end()) &&
      (indexMap.find("{-SC-}") != indexMap.end())){

      // prepare to make a new histogram: get histogram properties
    int dimension = FSString::TString2int(indexMap["{-ND-}"]);
    TString fileName = indexMap["{-FN-}"];
    TString ntName = indexMap["{-NT-}"];
    TString var = indexMap["{-VA-}"]; 
    TString bounds = indexMap["{-BO-}"];
    TString cuts = indexMap["{-CU-}"];
    TString scale = indexMap["{-SC-}"]; 
    int nbins = FSString::parseBoundsNBinsX(bounds);
    double low = FSString::parseBoundsLowerX(bounds);
    double high = FSString::parseBoundsUpperX(bounds);
    int nbinsy = FSString::parseBoundsNBinsY(bounds);
    double lowy = FSString::parseBoundsLowerY(bounds);
    double highy = FSString::parseBoundsUpperY(bounds);

      // make an empty histogram
    if (dimension == 1) m_hist1d = new TH1F(m_histName,"",nbins,low,high);
    if (dimension == 2) m_hist2d = new TH2F(m_histName,"",nbins,low,high,nbinsy,lowy,highy); 

      // make a new fundamental histogram
    if (m_expandedIndices.size() == 0){

        // set up the TChain
      TChain* chain = FSTree::getTChain(fileName,ntName);
      if ((!chain) || (chain->GetEntries() == 0) || (chain->GetNbranches() == 0))
        return pair<TH1F*,TH2F*> (m_hist1d,m_hist2d);

        // if the TChain is okay, set up to make a new histogram
      if (m_hist1d){ delete m_hist1d; m_hist1d = NULL; }
      if (m_hist2d){ delete m_hist2d; m_hist2d = NULL; }
      TString hname = m_histName;
      TString hbounds(hname); hbounds += bounds;
      TString scaleTimesCuts("");
      if (cuts != "") scaleTimesCuts = scale+"*("+cuts+")";
      if (cuts == "") scaleTimesCuts = scale;

        // project the chain and get the results
      chain->Project(hbounds, var, scaleTimesCuts);
      if (dimension == 1) m_hist1d = (TH1F*) gDirectory->FindObject(hname); 
      if (dimension == 2) m_hist2d = (TH2F*) gDirectory->FindObject(hname); 
      return pair<TH1F*,TH2F*> (m_hist1d,m_hist2d);

    }

      // loop over the expanded indices and keep adding histograms
    for (unsigned int i = 0; i < m_expandedIndices.size(); i++){
      pair<TH1F*,TH2F*> histPair = FSHistogram::getHistogramInfo(m_expandedIndices[i])->getTHNF();
      if (histPair.first)  m_hist1d->Add(histPair.first);
      if (histPair.second) m_hist2d->Add(histPair.second);
    }

  }

  return pair<TH1F*,TH2F*> (m_hist1d,m_hist2d);
}



TTree*
FSHistogramInfo::getTHNFContents(TTree* histTree, vector< pair<TString,TString> > extraTreeContents){
  int dimension = 1;
  if (m_index.Contains("{-ND-}1D")) dimension = 1;
  if (m_index.Contains("{-ND-}2D")) dimension = 2;
  for (unsigned int i = 0; i < extraTreeContents.size(); i++){
    extraTreeContents[i].first  = FSString::removeWhiteSpace(extraTreeContents[i].first);
    extraTreeContents[i].second = FSString::removeWhiteSpace(extraTreeContents[i].second);
    extraTreeContents[i].second = FSTree::expandVariable(extraTreeContents[i].second);
  }

    // CASE 1: set up a new tree and then add contents

  if (!histTree){
    histTree = new TTree("HistContents", "HistContents");
    Double_t x;  if (dimension >= 1) histTree->Branch("x",  &x,  "x/D");
    Double_t y;  if (dimension == 2) histTree->Branch("y",  &y,  "y/D");
    Double_t wt; if (dimension >= 1) histTree->Branch("wt", &wt, "wt/D");
    vector<Double_t> tempDoubles;
    for (unsigned int i = 0; i < extraTreeContents.size(); i++){
      tempDoubles.push_back(0.0);
      TString contentName = extraTreeContents[i].first;
      histTree->Branch(contentName,&tempDoubles[i],contentName+"/D");
    }
    return getTHNFContents(histTree,extraTreeContents);
  }

    // CASE 2: add contents from lower FSHistogramInfo objects

  if (m_expandedIndices.size() > 0){
    for (unsigned int i = 0; i < m_expandedIndices.size(); i++){
      FSHistogramInfo* histInfo = FSHistogram::getHistogramInfo(m_expandedIndices[i]);
      histTree = histInfo->getTHNFContents(histTree,extraTreeContents);
    }
    return histTree;
  }

    // CASE 3: add contents to histTree

    // get information from the index
  map<TString,TString> indexMap = FSHistogram::parseHistogramIndex(m_index);
  TString fileName = indexMap["{-FN-}"];
  TString ntName = indexMap["{-NT-}"];
  TString variable = indexMap["{-VA-}"]; 
  TString bounds = indexMap["{-BO-}"];
  TString cuts = indexMap["{-CU-}"];
  double scale = FSString::TString2double(indexMap["{-SC-}"]); 

    // get the x and y variable names
  vector<TString> vars = FSString::parseTString(variable,":");
  if ((int)vars.size() != dimension){
    cout << "FSHistogram contents ERROR:  bad variable format" << endl;
    exit(0);
  }
  TString varX("");  if (dimension >= 1)  varX = vars[0];
  TString varY("");  if (dimension == 2){ varX = vars[1]; varY = vars[0]; }

    // set the addresses of the values to add to histTree
  Double_t x = 0.0;  if (dimension >= 1) histTree->SetBranchAddress("x",  &x);
  Double_t y = 0.0;  if (dimension == 2) histTree->SetBranchAddress("y",  &y);
  Double_t wt = 0.0; if (dimension >= 1) histTree->SetBranchAddress("wt", &wt);
  Double_t extraValues[200];
  if (extraTreeContents.size() > 200){
    cout << "FSHistogram::addTHNFContents ERROR: too many extra branches" << endl;
    exit(0);
  }
  for (unsigned int i = 0; i < extraTreeContents.size(); i++){
    TString contentName = extraTreeContents[i].first;
    histTree->SetBranchAddress(contentName,&extraValues[i]);
  }

    // get the old tree
  TTree* nt = FSTree::getTChain(fileName,ntName);

    // set up the TTreeFormula objects
  if (cuts == "") cuts = "(1==1)";
  TTreeFormula* cutsF = NULL; if (dimension >= 1) cutsF = new TTreeFormula("cutsF", cuts, nt);
  TTreeFormula* varXF = NULL; if (dimension >= 1) varXF = new TTreeFormula("varXF", varX, nt);
  TTreeFormula* varYF = NULL; if (dimension == 2) varYF = new TTreeFormula("varYF", varY, nt);
  vector<TTreeFormula*> varEF;
  for (unsigned int i = 0; i < extraTreeContents.size(); i++){
    TString varName("varEF"); varName += i;
    varEF.push_back(new TTreeFormula(varName,extraTreeContents[i].second,nt));
  }

    // get cut values according to the histogram bounds
  double xLow  = 0.0;  if (dimension >= 1)  xLow = FSString::parseBoundsLowerX(bounds);
  double xHigh = 0.0;  if (dimension >= 1) xHigh = FSString::parseBoundsUpperX(bounds);
  double yLow  = 0.0;  if (dimension == 2)  yLow = FSString::parseBoundsLowerY(bounds);
  double yHigh = 0.0;  if (dimension == 2) yHigh = FSString::parseBoundsUpperY(bounds);

    // loop over events
  unsigned int nEvents = nt->GetEntries();
  for (unsigned int i = 0; i < nEvents; i++){
    nt->GetEntry(i);
    if (!cutsF->EvalInstance()) continue;
    if (dimension >= 1) x = varXF->EvalInstance();
    if (dimension == 2) y = varYF->EvalInstance();
    if (dimension >= 1) wt = scale * cutsF->EvalInstance();
    for (unsigned int j = 0; j < varEF.size(); j++){
      extraValues[j] = varEF[j]->EvalInstance();
    }
    if ((dimension >= 1) && ((x < xLow) || (x > xHigh))) continue;      
    if ((dimension == 2) && ((y < yLow) || (y > yHigh))) continue;      
    histTree->Fill();  
  }

    // delete the TTreeFormula objects
  if (cutsF) delete cutsF;
  if (varXF) delete varXF;
  if (varYF) delete varYF;
  for (unsigned int i = 0; i < varEF.size(); i++){
    if (varEF[i]) delete varEF[i];
  }

  return histTree;
}











/*


pair<TH1F*,TH2F*> 
FSHistogram::getTHNF(int dimension, TString fileName, TString histName, TString index){

    // create an index (or use a given index -- used only when reading a cache)

  if (index == ""){
    if (dimension == 1) index += "{-ND-}1D";
    if (dimension == 2) index += "{-ND-}2D";
    index += "{-FN-}"; index += fileName;
    index += "{-HN-}"; index += histName;
  }

    // first search the cache and return if the histogram is found

  pair<TH1F*,TH2F*> histPair = getHistogramFromCache(index);
  if (histPair.first || histPair.second) return histPair;

    // if not found in cache, search the file

  TFile* tf = FSTree::getTFile(fileName);
  tf->cd();
  TH1F* hist1d0 = NULL;
  TH2F* hist2d0 = NULL;
  TH1F* hist1d = NULL;
  TH2F* hist2d = NULL;
  if (dimension == 1) hist1d0 = (TH1F*) gDirectory->FindObjectAny(histName);
  if (dimension == 2) hist2d0 = (TH2F*) gDirectory->FindObjectAny(histName);
  if (hist1d0) hist1d = new TH1F(*hist1d0);
  if (hist2d0) hist2d = new TH2F(*hist2d0);  

    // if found in file, add the histogram to the cache and return

  if (hist1d || hist2d){
    if (FSControl::DEBUG) 
      cout << "FSHistogram: found histogram in file " << index << endl;
    addHistogramToCache(index,hist1d,hist2d);
    return pair<TH1F*,TH2F*>(hist1d,hist2d);
  }

    // if not found in cache or file, return NULL

  return pair<TH1F*,TH2F*>(NULL,NULL);

}




pair<TH1F*,TH2F*> 
FSHistogram::getTHNF(int dimension,
                                   TString fileName, TString ntName,
                                   TString variable, TString bounds,
                                   TString cuts, double scale, TTree* histTree,
                                   vector< pair<TString,TString> > extraTreeContents){

    // remove white space

  variable = FSString::removeWhiteSpace(variable);
  bounds   = FSString::removeWhiteSpace(bounds);
  cuts     = FSString::removeWhiteSpace(cuts);
  for (unsigned int i = 0; i < extraTreeContents.size(); i++){
    extraTreeContents[i].first  = FSString::removeWhiteSpace(extraTreeContents[i].first);
    extraTreeContents[i].second = FSString::removeWhiteSpace(extraTreeContents[i].second);
  }

    // expand "cuts" using FSCut and check for multidimensional sidebands

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
      pair<TH1F*,TH2F*> histPair = FSHistogram::getTHNF(dimension, fileName, ntName, 
                                     variable, bounds, cuts_i, scale_i, histTree, extraTreeContents);
      if (dimension == 1){
        TH1F* hi = histPair.first;
        hist1d = FSHistogram::addTH1F("FSCUTTOTAL",hi);
      }
      if (dimension == 2){
        TH2F* hi = histPair.second;
        hist2d = FSHistogram::addTH2F("FSCUTTOTAL",hi);
      }
    }

      // clear the add cache and return new histograms

    pair<TH1F*,TH2F*> newHists = FSHistogram::addTempHistToCache(hist1d,hist2d);
    FSHistogram::clearAddCache("FSCUTTOTAL");
    return newHists;

  }


    // expand "variable" and "cuts" taking out things like MASS(xxxx)

  TString fullVariable = FSTree::expandVariable(variable);
  TString fullCuts     = FSTree::expandVariable(cuts);
  for (unsigned int i = 0; i < extraTreeContents.size(); i++){
    extraTreeContents[i].second = FSTree::expandVariable(extraTreeContents[i].second);
  }


    // create an index

  TString index = getHistogramIndex(dimension, fileName, ntName, 
                               fullVariable, bounds, fullCuts, scale);


    // print information to screen if DEBUG is set

  if (FSControl::DEBUG){
    cout << "***************************************" << endl;
    cout << " FSHistogram:  MAKING HISTOGRAM" << endl;
    cout << "***************************************" << endl;
    cout << "\tFILENAME    = " << fileName << endl;
    cout << "\tNTUPLE NAME = " << ntName << endl;
    cout << "\tVARIABLE    = " << variable << endl;
    cout << "\t  EXPANDED  = " << fullVariable << endl;
    cout << "\tBOUNDS      = " << bounds << endl;
    cout << "\tCUTS        = " << cuts << endl;
    cout << "\t  EXPANDED  = " << fullCuts << endl;
    cout << "\tSCALE       = " << scale << endl;
    cout << "\tFILL TREE?  = " << (bool) histTree << endl;
    cout << "\tINDEX       = " << index << endl;
    cout << "***************************************" << endl;
  }


    // first search the cache and return if the histogram is found

  if (!histTree){
    pair<TH1F*,TH2F*> histPair = getHistogramFromCache(index);
    if (histPair.first || histPair.second) return histPair;
  }
  else{
    cout << "FSHistogram:  MAKING TREE WITH HISTOGRAM CONTENTS" << endl;
  }


    // set up the chain

  TChain* chain = FSTree::getTChain(fileName,ntName);


    // if not found in cache, set up to make a new histogram

  TH1F* hist1d = NULL;
  TH2F* hist2d = NULL;
  TString hname = makeHistogramCacheName();
  TString hbounds(hname); hbounds += bounds;



    // use project to create a histogram if the chain is okay

  if ((chain) && (chain->GetEntries() > 0) && (chain->GetNbranches() > 0)){

    TString extraScale(FSString::double2TString(scale,8,true));
    TString scaleTimesCuts(fullCuts);
    if (scaleTimesCuts != "") scaleTimesCuts = "*("+scaleTimesCuts+")";
    scaleTimesCuts = extraScale+scaleTimesCuts;

    chain->Project(hbounds, fullVariable, scaleTimesCuts);

    if (dimension == 1) hist1d = (TH1F*) gDirectory->FindObject(hname); 
    if (dimension == 2) hist2d = (TH2F*) gDirectory->FindObject(hname); 

    if (histTree) histTree = addTHNFContents(histTree, dimension, fileName,
                      ntName,fullVariable,bounds,fullCuts,scale,extraTreeContents);

  }

    // otherwise create an empty histogram

  else{

    int nbins = FSString::parseBoundsNBinsX(bounds);
    double low = FSString::parseBoundsLowerX(bounds);
    double high = FSString::parseBoundsUpperX(bounds);
    int nbinsy = FSString::parseBoundsNBinsY(bounds);
    double lowy = FSString::parseBoundsLowerY(bounds);
    double highy = FSString::parseBoundsUpperY(bounds);

    if (dimension == 1) hist1d = new TH1F(hname,"",nbins,low,high);
    if (dimension == 2) hist2d = new TH2F(hname,"",nbins,low,high,nbinsy,lowy,highy); 

  }


    // add to the histogram cache and return

  addHistogramToCache(index,hist1d,hist2d);
  return pair<TH1F*,TH2F*>(hist1d,hist2d);

} 



TTree*
FSHistogram::setTHNFContents(int dimension,
                             vector< pair<TString,TString> > extraTreeContents){
  TTree* histTree = new TTree("HistContents", "HistContents");
  Double_t x;  if (dimension >= 1) histTree->Branch("x",  &x,  "x/D");
  Double_t y;  if (dimension == 2) histTree->Branch("y",  &y,  "y/D");
  Double_t wt; if (dimension >= 1) histTree->Branch("wt", &wt, "wt/D");
  vector<Double_t> tempDoubles;
  for (unsigned int i = 0; i < extraTreeContents.size(); i++){
    tempDoubles.push_back(0.0);
    TString contentName = extraTreeContents[i].first;
    histTree->Branch(contentName,&tempDoubles[i],contentName+"/D");
  }
  return histTree;
}

TTree*
FSHistogram::addTHNFContents(TTree* histTree, int dimension, 
                             TString fileName, TString ntName, TString variable, TString bounds, 
                             TString cuts, double scale,
                             vector< pair<TString,TString> > extraTreeContents){

    // get the x and y variable names

  vector<TString> vars = FSString::parseTString(variable,":");
  if ((int)vars.size() != dimension){
    cout << "FSHistogram contents ERROR:  bad variable format" << endl;
    exit(0);
  }
  TString varX("");  if (dimension >= 1)  varX = vars[0];
  TString varY("");  if (dimension == 2){ varX = vars[1]; varY = vars[0]; }

    // set the addresses of the values to add to histTree

  Double_t x = 0.0;  if (dimension >= 1) histTree->SetBranchAddress("x",  &x);
  Double_t y = 0.0;  if (dimension == 2) histTree->SetBranchAddress("y",  &y);
  Double_t wt = 0.0; if (dimension >= 1) histTree->SetBranchAddress("wt", &wt);
  Double_t extraValues[200];
  if (extraTreeContents.size() > 200){
    cout << "FSHistogram::addTHNFContents ERROR: too many extra branches" << endl;
    exit(0);
  }
  for (unsigned int i = 0; i < extraTreeContents.size(); i++){
    TString contentName = extraTreeContents[i].first;
    histTree->SetBranchAddress(contentName,&extraValues[i]);
  }

    // get the old tree

  TTree* nt = FSTree::getTChain(fileName,ntName);

    // set up the TTreeFormula objects

  if (cuts == "") cuts = "(1==1)";
  TTreeFormula* cutsF = NULL; if (dimension >= 1) cutsF = new TTreeFormula("cutsF", cuts, nt);
  TTreeFormula* varXF = NULL; if (dimension >= 1) varXF = new TTreeFormula("varXF", varX, nt);
  TTreeFormula* varYF = NULL; if (dimension == 2) varYF = new TTreeFormula("varYF", varY, nt);
  vector<TTreeFormula*> varEF;
  for (unsigned int i = 0; i < extraTreeContents.size(); i++){
    TString varName("varEF"); varName += i;
    varEF.push_back(new TTreeFormula(varName,extraTreeContents[i].second,nt));
  }

    // get cut values according to the histogram bounds

  double xLow  = 0.0;  if (dimension >= 1)  xLow = FSString::parseBoundsLowerX(bounds);
  double xHigh = 0.0;  if (dimension >= 1) xHigh = FSString::parseBoundsUpperX(bounds);
  double yLow  = 0.0;  if (dimension == 2)  yLow = FSString::parseBoundsLowerY(bounds);
  double yHigh = 0.0;  if (dimension == 2) yHigh = FSString::parseBoundsUpperY(bounds);

    // loop over events

  unsigned int nEvents = nt->GetEntries();
  for (unsigned int i = 0; i < nEvents; i++){
    nt->GetEntry(i);
    if (!cutsF->EvalInstance()) continue;
    if (dimension >= 1) x = varXF->EvalInstance();
    if (dimension == 2) y = varYF->EvalInstance();
    if (dimension >= 1) wt = scale * cutsF->EvalInstance();
    for (unsigned int j = 0; j < varEF.size(); j++){
      extraValues[j] = varEF[j]->EvalInstance();
    }
    if ((dimension >= 1) && ((x < xLow) || (x > xHigh))) continue;      
    if ((dimension == 2) && ((y < yLow) || (y > yHigh))) continue;      
    histTree->Fill();  
  }

    // delete the TTreeFormula objects

  if (cutsF) delete cutsF;
  if (varXF) delete varXF;
  if (varYF) delete varYF;
  for (unsigned int i = 0; i < varEF.size(); i++){
    if (varEF[i]) delete varEF[i];
  }

  return histTree;
}



pair<TH1F*,TH2F*>
FSHistogram::getHistogramFromCache(TString index){

    // return empty histograms if there is no histogram caching

  if (!FSControl::HISTOGRAMCACHING) return pair<TH1F*,TH2F*>(NULL,NULL);

    // search for the histogram

  if (FSControl::DEBUG) 
    cout << "FSHistogram: looking for histogram " << index << endl;
  map<TString, pair<TH1F*,TH2F*> >::const_iterator mapItr = m_histogramCache.find(index);
  if (mapItr != m_histogramCache.end()){
    pair<TH1F*,TH2F*> histPair = m_histogramCache[index];
    if ((FSControl::DEBUG || !FSControl::QUIET) && histPair.first) 
      cout << "FSHistogram:  FOUND 1D HIST... " << histPair.first->GetName() 
           << " \twith entries... " << histPair.first->GetEntries() << endl;
    if ((FSControl::DEBUG || !FSControl::QUIET) && histPair.second) 
      cout << "FSHistogram:  FOUND 2D HIST... " << histPair.second->GetName() 
           << " \twith entries... " << histPair.second->GetEntries() << endl;
    return histPair;
  }

    // return NULL if not found

  if (FSControl::DEBUG) 
    cout << "FSHistogram: could not find histogram " << index << endl;
  return pair<TH1F*,TH2F*>(NULL,NULL);

}

void
FSHistogram::addHistogramToCache(TString index, TH1F* hist1d, TH2F* hist2d){

    // give the histogram a name

  TString hname = makeHistogramCacheName();
  if (hist1d) hist1d->SetName(hname);
  if (hist2d) hist2d->SetName(hname);

    // set common properties

  if (hist1d) hist1d = getTH1F(hist1d);
  if (hist2d) hist2d = getTH2F(hist2d);

    // add the histogram to the cache (if it isn't already there)

  if (!FSControl::HISTOGRAMCACHING) return;
  map<TString, pair<TH1F*,TH2F*> >::const_iterator mapItr = m_histogramCache.find(index);
  if (mapItr == m_histogramCache.end()){
    if ((FSControl::DEBUG || !FSControl::QUIET) && hist1d) 
      cout << "FSHistogram:  CREATED 1D HIST... " << hist1d->GetName() 
           << " \twith entries... " << hist1d->GetEntries() << endl;
    if ((FSControl::DEBUG || !FSControl::QUIET) && hist2d) 
      cout << "FSHistogram:  CREATED 2D HIST... " << hist2d->GetName() 
           << " \twith entries... " << hist2d->GetEntries() << endl;
    m_histogramCache[index] = pair<TH1F*,TH2F*>(hist1d,hist2d);
  }

}

*/