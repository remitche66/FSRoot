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
#include "FSBasic/FSHistogram.h"


  // static member data

map< TString, TChain*> FSHistogram::m_chainCache;
map< TString, TFile*> FSHistogram::m_fileCache;
map< TString, pair<TH1F*,TH2F*> > FSHistogram::m_histogramCache;
map< TString, pair<TH1F*,TH2F*> > FSHistogram::m_addCache;

TString AND("&&");
TString OR("||");
TString NOT("!");


  // ********************************************************
  // GET A TCHAIN FROM THE CACHE OR CREATE A NEW ONE
  // ********************************************************

TChain*
FSHistogram::getTChain(TString fileName, TString ntName, 
                              bool addFilesIndividually){
  TChain* nt = NULL;

    // clear the chain cache if there is no chain caching

  if (!FSControl::CHAINCACHING) clearChainCache();

    // create an index for this chain and search for it

  TString index(fileName);  index += ":";  index += ntName;
  map<TString,TChain*>::const_iterator mapItr = m_chainCache.find(index);
  if (mapItr != m_chainCache.end()){
    if (FSControl::DEBUG) 
      cout << "FSHistogram: found TChain with index " << index << endl;
    nt = m_chainCache[index];
  }

    // create the chain from scratch if not found

  else{
    if (FSControl::DEBUG) 
      cout << "FSHistogram: creating new TChain with index " << index << endl;
    nt = new TChain(ntName);
    if (addFilesIndividually){
      vector<TString> fileList;
      {
        TChain ntTemp(ntName); 
        ntTemp.Add(fileName);
        TIter next(ntTemp.GetListOfFiles());
        while ( TChainElement* el = (TChainElement*) next()){
          fileList.push_back(el->GetTitle());
        }
      }
      for (unsigned int i = 0; i < fileList.size(); i++){
         TChain ntTemp(ntName);
         ntTemp.Add(fileList[i]);
         if ((ntTemp.GetEntries() > 0) && (ntTemp.GetNbranches() > 0)){
           if (FSControl::DEBUG) 
             cout << "FSHistogram: adding file to TChain " << fileList[i] << endl;
           nt->Add(fileList[i]);
        }
      }
    }
    else{
      nt->Add(fileName);
    }
    if ((nt->GetEntries() == 0) || (nt->GetNbranches() == 0)){
      if (FSControl::DEBUG) 
	cout << "FSHistogram: null TChain with index " << index << endl;
      delete nt;
      nt = NULL;
    }
    else{
      m_chainCache[index] = nt;
    }
  }


  if ((nt) && (FSControl::CHAINFRIEND != "")){

    TString friendFileName(fileName);
    friendFileName += ".";
    friendFileName += FSControl::CHAINFRIEND;

    TString friendNTName(ntName);
    friendNTName += "_";
    friendNTName += FSControl::CHAINFRIEND;

    nt->AddFriend(friendNTName,friendFileName);

  }


  return nt;

}


  // ********************************************************
  // GET A TFILE FROM THE CACHE OR CREATE A NEW ONE
  // ********************************************************

TFile*
FSHistogram::getTFile(TString fileName){
  TFile* tf;

    // clear the file cache if there is no file caching

  if (!FSControl::FILECACHING) clearFileCache();

    // create an index for this file and search for it

  TString index(fileName);
  map<TString,TFile*>::const_iterator mapItr = m_fileCache.find(index);
  if (mapItr != m_fileCache.end()){
    if (FSControl::DEBUG) 
      cout << "FSHistogram: found TFile with index " << index << endl;
    tf = m_fileCache[index];
  }

    // create the file from scratch if not found

  else{
    if (FSControl::DEBUG) 
      cout << "FSHistogram: creating new TFile with index " << index << endl;
    tf = new TFile(fileName);
    m_fileCache[index] = tf;
  }

  return tf;

}



  // ********************************************************
  // SKIM A TREE AND OUTPUT TO A DIFFERENT FILE
  // ********************************************************

void
FSHistogram::skimTree(TString fileNameInput, TString chainName, 
                               TString fileNameOutput, TString cuts, TString newChainName){


  // create an output file first (further set up later)

  TFile* file2 = new TFile(fileNameOutput,"recreate");


  // retrieve tree 1

  TChain* nt = getTChain(fileNameInput,chainName);


  // expand variable macros

  TString newCuts = expandVariable(cuts);

  // write out message

  if (FSControl::QUIET){
    cout << "creating " << fileNameOutput << endl;
  }
  if (!FSControl::QUIET){
    cout << endl;
    cout << "Copying File:" << endl;
    cout << "\t" << fileNameInput << endl;
    cout << "To File:" << endl;
    cout << "\t" << fileNameOutput << endl;
    cout << endl;
    cout << "\tOriginal selection criteria:" << endl;
    cout << cuts << endl;
    cout << endl;
    cout << "\tModified selection criteria:" << endl;
    cout << newCuts << endl;
    cout << endl;
    if (nt){
      cout << "\tNumber of entries to skim:" << endl;
      cout << nt->GetEntries() << endl;
      cout << endl;
    }
  }

  // check if there are entries to skim

  if ((!nt) || (nt->GetEntries() == 0) || (nt->GetNbranches() == 0)){
    cout << "Could not find any entries...  skipping..." << endl;
    return;
  }


  // set up the output file

  file2->cd();
  while (chainName.Contains("/")){
    TString dirName("");
    for (int i = 0; i < chainName.Index("/"); i++) { dirName += chainName[i]; }
    chainName.Replace(0,chainName.Index("/")+1,"");
    file2->mkdir(dirName);
    file2->cd(dirName);
  }


  // copy the tree with selection criteria

  TTree* tree2 = nt->CopyTree(newCuts);
  if (newChainName != "") tree2->SetName(newChainName);
  if (!FSControl::QUIET){
    cout << "\tNumber of entries kept:" << endl;
    cout << tree2->GetEntries() << endl;
    cout << endl;
    cout << "\tSkim Ratio:" << endl;
    cout << (float) tree2->GetEntries() / (float) nt->GetEntries() << endl;
    cout << endl;
  }


  // write the tree to file2

  tree2->AutoSave();
  //tree2->Write();
  file2->Close();

  clearChainCache();
  delete file2;

}



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
FSHistogram::makeAddName(){
  TString hname("ADDCACHE");
  hname += (m_addCache.size() + 1);
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
  return getTH1F(getHistogramGeneral(1,fileName,histName).first);
}

TH2F* 
FSHistogram::getTH2F(TString fileName, TString histName){
  return getTH2F(getHistogramGeneral(2,fileName,histName).second);
}

pair<TH1F*,TH2F*> 
FSHistogram::getHistogramGeneral(int dimension, TString fileName, 
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

  TFile* tf = getTFile(fileName);
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
  TH1F* hist = getHistogramGeneral(1,fileName,ntName,variable,bounds,cuts,options,scale).first;
  if (hist->GetSum() != 0.0) hist->Scale(scale*hist->GetEntries()/hist->GetSum());
  return getTH1F(hist);
} 

TH2F* 
FSHistogram::getTH2F(TString fileName, TString ntName,
                                   TString variable, TString bounds,
                                   TString cuts,     TString options,
                                   float scale){
  TH2F* hist = getHistogramGeneral(2,fileName,ntName,variable,bounds,cuts,options,scale).second;
  if (hist->GetSum() != 0.0) hist->Scale(scale*hist->GetEntries()/hist->GetSum());
  return getTH2F(hist);
} 


pair<TH1F*,TH2F*> 
FSHistogram::getHistogramGeneral(int dimension,
                                   TString fileName, TString ntName,
                                   TString variable, TString bounds,
                                   TString cuts,     TString options,
                                   float scale){


    // expand "variable" and "cuts" taking out things like MASS(xxxx)

  TString fullVariable = expandVariable(variable);
  TString fullCuts     = expandVariable(cuts);


    // create an index

  TString index = getHistogramIndexGeneral(dimension, fileName, ntName, 
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

  TChain* chain = getTChain(fileName,ntName);


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

  addHistogramToCache(index,hist1d,hist2d);
  return m_histogramCache[index];

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
    getHistogramGeneral(dim,FSString::string2TString(rootCacheName),name,index);
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
  return getHistogramIndexGeneral(1,fileName,ntName,variable,bounds,cuts,options,scale);
}

TString
FSHistogram::getTH2FIndex(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     TString options,
                                float scale){
  return getHistogramIndexGeneral(2,fileName,ntName,variable,bounds,cuts,options,scale);
}

TString
FSHistogram::getTH1FIndex(TH1F* hist1d){
  TH2F* hist2d = NULL;
  return getHistogramIndexGeneral(pair<TH1F*,TH2F*>(hist1d,hist2d));
}

TString
FSHistogram::getTH2FIndex(TH2F* hist2d){
  TH1F* hist1d = NULL;
  return getHistogramIndexGeneral(pair<TH1F*,TH2F*>(hist1d,hist2d));
}



TString
FSHistogram::getHistogramIndexGeneral(int dimension,
                                TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     TString options,
                                float scale){
  TString index;
  if (dimension == 1) index += "1D";
  if (dimension == 2) index += "2D";
  index += "(fn)";  index += fileName;
  index += "(nt)";  index += ntName;
  index += "(va)";  index += expandVariable(variable);
  index += "(bo)";  index += bounds;
  index += "(cu)";  index += expandVariable(cuts);
  //index += "(op)";  index += options;
  //index += "(sc)";  index += FSString::double2TString(scale,8,true);
  options = "";
  scale = 0.0;
  return index;
}


TString
FSHistogram::getHistogramIndexGeneral(pair<TH1F*,TH2F*> hist){
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
  return addHistogramGeneral(addName,hist,hist2d,scale).first;
}

TH2F*
FSHistogram::addTH2F(TString addName, TH2F* hist, float scale){
  TH1F* hist1d = NULL;
  return addHistogramGeneral(addName,hist1d,hist,scale).second;
}

pair<TH1F*,TH2F*>
FSHistogram::addHistogramGeneral(TString addName, TH1F* hist1d, TH2F* hist2d, 
                                       float scale){

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
  // EXPAND VARIABLE MACROS
  // ********************************************************

TString 
FSHistogram::expandVariable(TString variable){

  TString PREFIXMARK("");

  TString RECOILMASSMARK  ("RECOILMASS(");
  TString RECOILMASS2MARK ("RECOILMASS2(");
  TString MASSMARK        ("MASS(");
  TString MASS2MARK       ("MASS2(");
  TString MOMXMARK        ("MOMENTUMX(");
  TString MOMYMARK        ("MOMENTUMY(");
  TString MOMZMARK        ("MOMENTUMZ(");
  TString MOMRMARK        ("MOMENTUMR(");
  TString MOMMARK         ("MOMENTUM(");
  TString ENMARK          ("ENERGY(");
  TString DOTPRODUCT      ("DOTPRODUCT(");
  TString COSINE          ("COSINE(");

  while (variable.Contains(RECOILMASSMARK) ||
         variable.Contains(RECOILMASS2MARK) ||
         variable.Contains(MASSMARK) ||
         variable.Contains(MASS2MARK)||
         variable.Contains(MOMXMARK) ||
         variable.Contains(MOMYMARK) ||
         variable.Contains(MOMZMARK) ||
         variable.Contains(MOMRMARK) ||
         variable.Contains(MOMMARK)  ||
         variable.Contains(ENMARK)   ||
         variable.Contains(DOTPRODUCT) ||
         variable.Contains(COSINE)){

    int index = 0;
    int size = 0;
    bool hasprefixmark = false;
    TString EnN("");  TString EnM("");  TString En("");
    TString PxN("");  TString PxM("");	TString Px("");
    TString PyN("");  TString PyM("");	TString Py("");
    TString PzN("");  TString PzM("");	TString Pz("");
    TString mark("");
         if (variable.Contains(RECOILMASSMARK))  mark = RECOILMASSMARK;
    else if (variable.Contains(RECOILMASS2MARK)) mark = RECOILMASS2MARK;
    else if (variable.Contains(MASSMARK))        mark = MASSMARK;
    else if (variable.Contains(MASS2MARK))       mark = MASS2MARK;
    else if (variable.Contains(MOMXMARK))        mark = MOMXMARK;
    else if (variable.Contains(MOMYMARK))        mark = MOMYMARK;
    else if (variable.Contains(MOMZMARK))        mark = MOMZMARK;
    else if (variable.Contains(MOMRMARK))        mark = MOMRMARK;
    else if (variable.Contains(MOMMARK))         mark = MOMMARK;
    else if (variable.Contains(ENMARK))          mark = ENMARK;
    else if (variable.Contains(DOTPRODUCT))      mark = DOTPRODUCT;
    else if (variable.Contains(COSINE))          mark = COSINE;

    if (variable.Contains(mark) && 
             variable.Index(mark) >= 1 &&
             TString(variable[variable.Index(mark)-1]).IsAlpha()){
           PREFIXMARK = "";
         if (variable.Index(mark) >= 2 && 
             TString(variable[variable.Index(mark)-2]).IsAlpha())
           PREFIXMARK += variable[variable.Index(mark)-2];
           PREFIXMARK += variable[variable.Index(mark)-1];
	   index = variable.Index(PREFIXMARK+mark);
	   size = (PREFIXMARK+mark).Length()+1;
	   hasprefixmark = true;
    }
    else if (variable.Contains(mark)){
	   index = variable.Index(mark);
	   size = (mark).Length()+1;
    }

    vector<TString> pN;
    vector<TString> pM;
    bool usevectorM = false;
    TString pIndex("");
    for (int i = index+size-1; (variable[i] != ')' && i < variable.Length()); i++){
      size++;
      if (variable[i] == ',' || variable[i] == ';'){
        if (!usevectorM)  pN.push_back(pIndex);
        if  (usevectorM)  pM.push_back(pIndex);
        if (variable[i] == ';') usevectorM = true;
        pIndex = "";
      }
      else{
        pIndex += variable[i];
      }
    }
    if (pIndex.Length() > 0 && !usevectorM) pN.push_back(pIndex);
    if (pIndex.Length() > 0 &&  usevectorM) pM.push_back(pIndex);


    TString CROSSINGANGLE("-0.003");
    if (pN[0] == "B3BEAM") CROSSINGANGLE = "0.011";
    if (pN[0] == "CCBEAM") CROSSINGANGLE = "-0.003";
    unsigned int ipN = 0;
    if (pN[0] == "P1S")     pN[0] = "3.096916";
    if (pN[0] == "P2S")     pN[0] = "3.68609";
    if (pN[0] == "P3770")   pN[0] = "3.774";
    if (pN[0] == "Y1S")     pN[0] = "9.4603";
    if (pN[0] == "Y2S")     pN[0] = "10.02326";
    if (pN[0] == "Y3S")     pN[0] = "10.3552";
    if (pN[0] == "BEAM")    pN[0] = "(2.0*BeamEnergy)";
    if (pN[0] == "B3BEAM")  pN[0] = "(2.0*BeamEnergy)";
    if (pN[0] == "CCBEAM")  pN[0] = "(2.0*BeamEnergy)";
    if (pN[0].Contains(".")){
      EnN += pN[0];		             
      PxN += "1.0*sin("+CROSSINGANGLE+")*"+pN[0];
      PyN += "0.0";
      PzN += "0.0";
      ipN++;
    }
    //if (pN[0] == "GBEAM"){
    //  EnN += "(BeamEnergy+0.938)";		             
    //  PxN += "0.0";
    //  PyN += "0.0";
    //  PzN += "(BeamEnergy)";
    //  ipN++;
    //}
    if (pN[0] == "GBEAM"){
      EnN += "(EnPB+0.938272)";		             
      PxN += "PxPB";
      PyN += "PyPB";
      PzN += "PzPB";
      ipN++;
    }
    if (pN[0] == "RGBEAM"){
      EnN += "(REnPB+0.938272)";		             
      PxN += "RPxPB";
      PyN += "RPyPB";
      PzN += "RPzPB";
      ipN++;
    }


      // sort the pN vector
    for (unsigned int i = ipN; (pN.size() != 0) && (i < pN.size()-1); i++){
      for (unsigned int j = i+1; j < pN.size(); j++){
        if (FSString::TString2string(pN[j]) < FSString::TString2string(pN[i])){
          TString tmp = pN[i];
          pN[i] = pN[j];
          pN[j] = tmp;
        }
      }
    }

      // sort the pM vector
    for (unsigned int i = 0; (pM.size() != 0) && (i < pM.size()-1); i++){
      for (unsigned int j = i+1; j < pM.size(); j++){
        if (FSString::TString2string(pM[j]) < FSString::TString2string(pM[i])){
          TString tmp = pM[i];
          pM[i] = pM[j];
          pM[j] = tmp;
        }
      }
    }


    TString pre("");
    TString post("");
    TString operation("");
    if (hasprefixmark)   pre =  PREFIXMARK;
    if (pre == "TR") pre = "MCDecayParticle";
    for (unsigned int i = ipN; i < pN.size(); i++) { if (i == 0) operation = "";
                                                     if (i != 0) operation = "+";
                                                     EnN += operation+pre+"EnP"+pN[i]+post;
			  		             PxN += operation+pre+"PxP"+pN[i]+post;
					             PyN += operation+pre+"PyP"+pN[i]+post;
					             PzN += operation+pre+"PzP"+pN[i]+post; }
    for (unsigned int i = 0;   i < pM.size(); i++) { if (i == 0) operation = "";
                                                     if (i != 0) operation = "+";
                                                     EnM += operation+pre+"EnP"+pM[i]+post;
			  		             PxM += operation+pre+"PxP"+pM[i]+post;
					             PyM += operation+pre+"PyP"+pM[i]+post;
					             PzM += operation+pre+"PzP"+pM[i]+post; }


    TString substitute("");

         if (mark == RECOILMASSMARK){
           En = EnN + "-(" + EnM +")";
           Px = PxN + "-(" + PxM +")";
           Py = PyN + "-(" + PyM +")";
           Pz = PzN + "-(" + PzM +")";
	   //substitute += "(sqrt((" + En + ")*(" + En + ")-" +
           //                    "(" + Px + ")*(" + Px + ")-" +
           //                    "(" + Py + ")*(" + Py + ")-" +
           //                    "(" + Pz + ")*(" + Pz + ")))";
	   substitute += "(sqrt((" + En + ")**2-" +
                               "(" + Px + ")**2-" +
                               "(" + Py + ")**2-" +
                               "(" + Pz + ")**2))";
    }
    else if (mark == RECOILMASS2MARK){
           En = EnN + "-(" + EnM +")";
           Px = PxN + "-(" + PxM +")";
           Py = PyN + "-(" + PyM +")";
           Pz = PzN + "-(" + PzM +")";
	   //substitute += "((" + En + ")*(" + En + ")-" +
           //               "(" + Px + ")*(" + Px + ")-" +
           //               "(" + Py + ")*(" + Py + ")-" +
           //               "(" + Pz + ")*(" + Pz + "))";
	   substitute += "((" + En + ")**2-" +
                          "(" + Px + ")**2-" +
                          "(" + Py + ")**2-" +
                          "(" + Pz + ")**2)";
    }
    else if (mark == MASSMARK){
	   //substitute += "(sqrt((" + EnN + ")*(" + EnN + ")-" +
           //                    "(" + PxN + ")*(" + PxN + ")-" +
           //                    "(" + PyN + ")*(" + PyN + ")-" +
           //                    "(" + PzN + ")*(" + PzN + ")))";
	   substitute += "(sqrt((" + EnN + ")**2-" +
                               "(" + PxN + ")**2-" +
                               "(" + PyN + ")**2-" +
                               "(" + PzN + ")**2))";
    }
    else if (mark == MASS2MARK){
	   //substitute += "((" + EnN + ")*(" + EnN + ")-" +
           //               "(" + PxN + ")*(" + PxN + ")-" +
           //               "(" + PyN + ")*(" + PyN + ")-" +
           //               "(" + PzN + ")*(" + PzN + "))";
	   substitute += "((" + EnN + ")**2-" +
                          "(" + PxN + ")**2-" +
                          "(" + PyN + ")**2-" +
                          "(" + PzN + ")**2)";
    }
    else if (mark == MOMXMARK){
	   substitute += "(" + PxN + ")";
    }
    else if (mark == MOMYMARK){
	   substitute += "(" + PyN + ")";
    }
    else if (mark == MOMZMARK){
	   substitute += "(" + PzN + ")";
    }
    else if (mark == MOMRMARK){
	   substitute += "(sqrt((" + PxN + ")**2+" +
                               "(" + PyN + ")**2))";
    }
    else if (mark == MOMMARK){
	   //substitute += "(sqrt((" + PxN + ")*(" + PxN + ")+" +
           //                    "(" + PyN + ")*(" + PyN + ")+" +
           //                    "(" + PzN + ")*(" + PzN + ")))";
	   substitute += "(sqrt((" + PxN + ")**2+" +
                               "(" + PyN + ")**2+" +
                               "(" + PzN + ")**2))";
    }
    else if (mark == ENMARK){
	   substitute += "(" + EnN + ")";
    }
    else if (mark == DOTPRODUCT){
	   substitute += "((" + PxN + ")*(" + PxM + ")+" +
                          "(" + PyN + ")*(" + PyM + ")+" +
                          "(" + PzN + ")*(" + PzM + "))";
    }
    else if (mark == COSINE){
      if (pM.size()==0){
	   //substitute += "((" + PzN + ")/" +
           //         "(sqrt((" + PxN + ")*(" + PxN + ")+" +
           //               "(" + PyN + ")*(" + PyN + ")+" +
           //               "(" + PzN + ")*(" + PzN + "))))"; 
	   substitute += "((" + PzN + ")/" +
                    "(sqrt((" + PxN + ")**2+" +
                          "(" + PyN + ")**2+" +
                          "(" + PzN + ")**2)))"; 
      }
      else{
	   //substitute += "(((" + PxN + ")*(" + PxM + ")+" +
           //                "(" + PyN + ")*(" + PyM + ")+" +
           //                "(" + PzN + ")*(" + PzM + "))/" +
	   //                   "(sqrt((" + PxN + ")*(" + PxN + ")+" +
           //                         "(" + PyN + ")*(" + PyN + ")+" +
           //                         "(" + PzN + ")*(" + PzN + ")))/" + 
	   //                   "(sqrt((" + PxM + ")*(" + PxM + ")+" +
           //                         "(" + PyM + ")*(" + PyM + ")+" +
           //                         "(" + PzM + ")*(" + PzM + "))))";
	   substitute += "(((" + PxN + ")*(" + PxM + ")+" +
                           "(" + PyN + ")*(" + PyM + ")+" +
                           "(" + PzN + ")*(" + PzM + "))/" +
	                      "(sqrt((" + PxN + ")**2+" +
                                    "(" + PyN + ")**2+" +
                                    "(" + PzN + ")**2))/" + 
	                      "(sqrt((" + PxM + ")**2+" +
                                    "(" + PyM + ")**2+" +
                                    "(" + PzM + ")**2)))";
      }
    }

    variable.Replace(index,size,substitute);

  }

  return variable;

}




  // ********************************************************
  // CLEAR GLOBAL CACHES
  // ********************************************************

void
FSHistogram::clearChainCache(){
  if (FSControl::DEBUG) 
    cout << "FSHistogram: clearing chain cache" << endl;
  for (map<TString,TChain*>::iterator rmItr = m_chainCache.begin();
       rmItr != m_chainCache.end(); rmItr++){
    if (rmItr->second) delete rmItr->second;
  }
  m_chainCache.clear();
  if (FSControl::DEBUG) 
    cout << "FSHistogram: done clearing chain cache" << endl;
}

void
FSHistogram::clearFileCache(){
  if (FSControl::DEBUG) 
    cout << "FSHistogram: clearing file cache" << endl;
  for (map<TString,TFile*>::iterator rmItr = m_fileCache.begin();
       rmItr != m_fileCache.end(); rmItr++){
    if (rmItr->second) delete rmItr->second;
  }
  m_fileCache.clear();
  if (FSControl::DEBUG) 
    cout << "FSHistogram: done clearing file cache" << endl;
}

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

