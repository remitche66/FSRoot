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
#include "FSBasic/FSControl.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSTree.h"
#include "FSBasic/FSCut.h"
#include "FSBasic/FSHistogram.h"


  // static member data

map< TString, pair<TH1F*,TH2F*> > FSHistogram::m_histogramCache;
map< TString, pair<TH1F*,TH2F*> > FSHistogram::m_tempCache;
map< TString, pair<TH1F*,TH2F*> > FSHistogram::m_addCache;
unsigned int FSHistogram::m_addCacheTotalSize = 0;

TString FSAND("&&");
TString FSOR("||");
TString FSNOT("!");


  // ********************************************************
  // GET A HISTOGRAM FROM THE CACHE OR RETURN NULL (private)
  // ********************************************************

pair<TH1F*,TH2F*>
FSHistogram::getHistogramFromCache(TString index){

    // clear the histogram cache if there is no histogram caching

  if (!FSControl::HISTOGRAMCACHING) clearHistogramCache();

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


  // ********************************************************
  // GIVE HISTOGRAMS IN THE CACHE A UNIQUE NAME (private)
  // ********************************************************

TString
FSHistogram::makeHistogramName(){
  TString hname("HISTCACHE");
  hname += (m_histogramCache.size() + 1);
  return hname;
}

TString
FSHistogram::makeTempHistName(){
  TString hname("TEMPHISTCACHE");
  hname += (m_tempCache.size() + 1);
  return hname;
}

TString
FSHistogram::makeAddName(){
  TString hname("ADDCACHE");
  hname += ++m_addCacheTotalSize; //(m_addCache.size() + 1);
  if (FSControl::DEBUG) { cout << "FSHistogram::makeAddName: new name = " << hname << endl; }
  return hname;
}



  // ********************************************************
  // ADD A HISTOGRAM TO THE CACHE (private)
  // ********************************************************

void
FSHistogram::addHistogramToCache(TString index, TH1F* hist1d, TH2F* hist2d){

    // give the histogram a name

  TString hname = makeHistogramName();
  if (hist1d) hist1d->SetName(hname);
  if (hist2d) hist2d->SetName(hname);

    // set common properties

  if (hist1d) hist1d = getTH1F(hist1d);
  if (hist2d) hist2d = getTH2F(hist2d);

    // add the histogram to the cache (if it isn't already there)

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
  TString histName = makeTempHistName();
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


  // ********************************************************
  // SET COMMON PROPERTIES OF HISTOGRAMS
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
  // RETRIEVE HISTOGRAMS FROM FILES
  // ********************************************************


TH1F* 
FSHistogram::getTH1F(TString fileName, TString histName){
  return getTH1F(getTHNF(1,fileName,histName).first);
}

TH2F* 
FSHistogram::getTH2F(TString fileName, TString histName){
  return getTH2F(getTHNF(2,fileName,histName).second);
}

pair<TH1F*,TH2F*> 
FSHistogram::getTHNF(int dimension, TString fileName, 
                                        TString histName, TString index){

    // create an index (or use the original index for readHistogramCache)

  if (index == ""){
    if (dimension == 1) index += "1D";
    if (dimension == 2) index += "2D";
    index += "(fn)"; index += fileName;
    index += "(hn)"; index += histName;
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
    return m_histogramCache[index];
  }

    // if not found in cache or file, return NULL

  return pair<TH1F*,TH2F*>(NULL,NULL);

}


  // ********************************************************
  // CREATE HISTOGRAMS FROM TREES
  // ********************************************************


TH1F* 
FSHistogram::getTH1F(TString fileName, TString ntName,
                                   TString variable, TString bounds,
                                   TString cuts,     TString options,
                                   float scale){
  TH1F* hist = getTHNF(1,fileName,ntName,variable,bounds,cuts,options,scale).first;
  return getTH1F(hist);
} 

TH2F* 
FSHistogram::getTH2F(TString fileName, TString ntName,
                                   TString variable, TString bounds,
                                   TString cuts,     TString options,
                                   float scale){
  TH2F* hist = getTHNF(2,fileName,ntName,variable,bounds,cuts,options,scale).second;
  return getTH2F(hist);
} 


pair<TH1F*,TH2F*> 
FSHistogram::getTHNF(int dimension,
                                   TString fileName, TString ntName,
                                   TString variable, TString bounds,
                                   TString cuts,     TString options,
                                   float scale){

    // remove white space

  variable = FSString::removeWhiteSpace(variable);
  bounds   = FSString::removeWhiteSpace(bounds);
  cuts     = FSString::removeWhiteSpace(cuts);


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
      if (dimension == 1){
        TH1F* hi = FSHistogram::getTH1F(fileName, ntName, variable, bounds,
                                        cuts_i, options, scale_i);
        hist1d = FSHistogram::addTH1F("FSCUTTOTAL",hi);
      }
      if (dimension == 2){
        TH2F* hi = FSHistogram::getTH2F(fileName, ntName, variable, bounds,
                                        cuts_i, options, scale_i);
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


    // create an index

  TString index = getTHNFIndex(dimension, fileName, ntName, 
                                           fullVariable, bounds, fullCuts, 
                                           options, scale);


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
    cout << "\tOPTIONS     = " << options << endl;
    cout << "\tSCALE       = " << scale << endl;
    cout << "\tINDEX       = " << index << endl;
    cout << "***************************************" << endl;
  }


    // first search the cache and return if the histogram is found

  pair<TH1F*,TH2F*> histPair = getHistogramFromCache(index);
  if (histPair.first || histPair.second) return histPair;


    // set up the chain

  TChain* chain = FSTree::getTChain(fileName,ntName);


    // if not found in cache, set up to make a new histogram

  TH1F* hist1d = NULL;
  TH2F* hist2d = NULL;
  TString hname = makeHistogramName();
  TString hbounds(hname); hbounds += bounds;



    // use project to create a histogram if the chain is okay

  if ((chain) && (chain->GetEntries() > 0) && (chain->GetNbranches() > 0)){

    chain->Project(hbounds, fullVariable, fullCuts, options);

    if (dimension == 1) hist1d = (TH1F*) gDirectory->FindObject(hname); 
    if (dimension == 2) hist2d = (TH2F*) gDirectory->FindObject(hname); 

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

  if (hist1d && hist1d->GetSum() != 0.0) hist1d->Scale(scale*hist1d->GetEntries()/hist1d->GetSum());
  if (hist2d && hist2d->GetSum() != 0.0) hist2d->Scale(scale*hist2d->GetEntries()/hist2d->GetSum());
  addHistogramToCache(index,hist1d,hist2d);
  return pair<TH1F*,TH2F*>(hist1d,hist2d);

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

double
FSHistogram::getLow  (TH1F* hist){
  return hist->GetBinLowEdge(1);
}

double
FSHistogram::getHigh (TH1F* hist){
  return hist->GetBinLowEdge(hist->GetNbinsX())+hist->GetBinWidth(hist->GetNbinsX());
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
    if (index.Index("2") == 0) dim = 2;
    getTHNF(dim,FSString::string2TString(rootCacheName),name,index);
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
FSHistogram::getTH1FIndex(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     TString options,
                                float scale){
  return getTHNFIndex(1,fileName,ntName,variable,bounds,cuts,options,scale);
}

TString
FSHistogram::getTH2FIndex(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     TString options,
                                float scale){
  return getTHNFIndex(2,fileName,ntName,variable,bounds,cuts,options,scale);
}

TString
FSHistogram::getTH1FIndex(TH1F* hist1d){
  TH2F* hist2d = NULL;
  return getTHNFIndex(pair<TH1F*,TH2F*>(hist1d,hist2d));
}

TString
FSHistogram::getTH2FIndex(TH2F* hist2d){
  TH1F* hist1d = NULL;
  return getTHNFIndex(pair<TH1F*,TH2F*>(hist1d,hist2d));
}



TString
FSHistogram::getTHNFIndex(int dimension,
                                TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     TString options,
                                float scale){
  TString index;
  variable = FSString::removeWhiteSpace(variable);
  bounds   = FSString::removeWhiteSpace(bounds);
  cuts     = FSString::removeWhiteSpace(cuts);
  if (dimension == 1) index += "1D";
  if (dimension == 2) index += "2D";
  index += "(fn)";  index += fileName;
  index += "(nt)";  index += ntName;
  index += "(va)";  index += FSTree::expandVariable(variable);
  index += "(bo)";  index += bounds;
  index += "(cu)";  index += FSTree::expandVariable(cuts);
  index += "(op)";  index += options;
  index += "(sc)";  index += FSString::double2TString(scale,8,true);
  //options = "";
  //scale = 0.0;
  return index;
}


TString
FSHistogram::getTHNFIndex(pair<TH1F*,TH2F*> hist){
  for (map< TString, pair<TH1F*,TH2F*> >::const_iterator mItr = m_histogramCache.begin();
       mItr != m_histogramCache.end(); mItr++){
    if (mItr->second == hist) return mItr->first;
  }
  return TString("");
}



  // ********************************************************
  // KEEP RUNNING HISTOGRAMS
  // ********************************************************

TH1F*
FSHistogram::addTH1F(TString addName, TH1F* hist, float scale){
  TH2F* hist2d = NULL;
  return addTHNF(addName,hist,hist2d,scale).first;
}

TH2F*
FSHistogram::addTH2F(TString addName, TH2F* hist, float scale){
  TH1F* hist1d = NULL;
  return addTHNF(addName,hist1d,hist,scale).second;
}

pair<TH1F*,TH2F*>
FSHistogram::addTHNF(TString addName, TH1F* hist1d, TH2F* hist2d, float scale){

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
    hist1d0->SetName(makeAddName());  
    hist1d0->Scale(scale); 
  }
  if (hist2d){
    hist2d0 = getTH2F(new TH2F(*hist2d));
    hist2d0->SetName(makeAddName());  
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
  for (map<TString,pair<TH1F*,TH2F*> >::iterator rmItr = m_histogramCache.begin();
       rmItr != m_histogramCache.end(); rmItr++){
    if (rmItr->second.first) delete rmItr->second.first;
    if (rmItr->second.second) delete rmItr->second.second;
  }
  m_histogramCache.clear();
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

