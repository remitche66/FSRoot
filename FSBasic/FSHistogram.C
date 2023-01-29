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
#include "TMarker.h"
#include "FSBasic/FSControl.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSSystem.h"
#include "FSBasic/FSTree.h"
#include "FSBasic/FSCut.h"
#include "FSBasic/FSHistogram.h"


  // static member data

map< TString, FSHistogramInfo* >  FSHistogram::m_FSHistogramInfoCache;
unsigned int FSHistogram::m_indexFSRootHistName = 0;
unsigned int FSHistogram::m_indexFSRootTempName = 0;
bool FSHistogram::m_USEDATAFRAME = false;
bool FSHistogram::m_USEDATAFRAMENOW = false;
map< TString, pair<TString,TString> > FSHistogram::m_RDFVariableDefinitions;
unsigned int FSHistogram::m_RDFVariableCounter = 0;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
map< TString, ROOT::RDataFrame* > FSHistogram::m_RDataFrameCache;
#endif

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
    hist->SetLineColor(kBlack);
    setHistogramMaxMin(hist);
  }
  return hist;
}

TH2F* 
FSHistogram::getTH2F(TH2F* hist){
  if (hist){
    if (hist->GetSumw2N() == 0) hist->Sumw2();
    hist->SetDirectory(0);
    hist->SetStats(0);
    //hist->GetZaxis()->SetLabelSize(0.03);
    setHistogramMaxMin(hist);
  }
  return hist;
}


  // ********************************************************
  // RETRIEVE A HISTOGRAM FROM A FILE AND CACHE IT
  // ********************************************************


TH1F* 
FSHistogram::getTH1F(TString fileName, TString histName){
  TString index = getHistogramIndexFile(1,fileName,histName);
  return getTH1F(getFSHistogramInfo(index)->getTHNF().first);
}

TH2F* 
FSHistogram::getTH2F(TString fileName, TString histName){
  TString index = getHistogramIndexFile(2,fileName,histName);
  return getTH2F(getFSHistogramInfo(index)->getTHNF().second);
}


pair<TH1F*,TH2F*> 
FSHistogram::getTHNFBasicFile(TString index, TString& STATUS){

    // initial checks
  STATUS = checkIndex(index,"FILE");
  if (STATUS.Contains("!!")) return getTHNFBasicEmpty(index); 

    // adjust the input parameters
  map<TString,TString> mapIndex = parseHistogramIndex(index);
  int     dimension = FSString::TString2int(mapIndex["{-ND-}"]);
  TString fileName  = mapIndex["{-FN-}"];
  TString histName  = mapIndex["{-HN-}"];
  if (FSControl::DEBUG){
    cout << "FSHistogram::getTHNFBasicFile DEBUG: looking for histogram in file" << endl;
    printIndexInfo(index);
  }

    // find the histogram in a file 
  TH1F* hist1d  = NULL;  TH2F* hist2d  = NULL; 
  TFile file(fileName);  file.cd();
  if (dimension == 1) hist1d = (TH1F*) file.FindObjectAny(histName);
  if (dimension == 2) hist2d = (TH2F*) file.FindObjectAny(histName);
  if (hist1d) getTH1F(hist1d)->SetName(makeFSRootTempName());
  if (hist2d) getTH2F(hist2d)->SetName(makeFSRootTempName());
  if (!hist1d && !hist2d){ STATUS = "!!NO_HIST!!"; return getTHNFBasicEmpty(index); }
  return pair<TH1F*,TH2F*>(hist1d,hist2d);

}



  // ********************************************************
  // CREATE A HISTOGRAM FROM A TREE AND CACHE IT
  // ********************************************************


TH1F* 
FSHistogram::getTH1F(TString fileName, TString ntName,
                                   TString variable, TString bounds,
                                   TString cuts, double scale, bool TESTONLY){
  pair<TString, vector<TString> > 
    indices = getHistogramIndexTree(1,fileName,ntName,variable,bounds,cuts,scale);
  if (TESTONLY){ printIndexInfo(indices.first,indices.second); 
                 return getTHNFBasicEmpty(indices.first).first; }
  TH1F* hist = getFSHistogramInfo(indices.first,indices.second)->getTHNF().first;
  if (m_USEDATAFRAME && m_USEDATAFRAMENOW) executeRDataFrame();
  return getTH1F(hist);
} 

TH2F* 
FSHistogram::getTH2F(TString fileName, TString ntName,
                                   TString variable, TString bounds,
                                   TString cuts, double scale, bool TESTONLY){
  pair<TString, vector<TString> > 
    indices = getHistogramIndexTree(2,fileName,ntName,variable,bounds,cuts,scale);
  if (TESTONLY){ printIndexInfo(indices.first,indices.second);
                 return getTHNFBasicEmpty(indices.first).second; }
  TH2F* hist = getFSHistogramInfo(indices.first,indices.second)->getTHNF().second;
  if (m_USEDATAFRAME && m_USEDATAFRAMENOW) executeRDataFrame();
  return getTH2F(hist);
} 


pair<TH1F*,TH2F*> 
FSHistogram::getTHNFBasicTree(TString index, TString& STATUS){
 
    // initial checks
  STATUS = checkIndex(index,"TREE");
  if (STATUS.Contains("!!")) return getTHNFBasicEmpty(index); 

    // adjust the input parameters
  map<TString,TString> mapIndex = parseHistogramIndex(index);
  int     dimension = FSString::TString2int(mapIndex["{-ND-}"]);
  TString fileName  = mapIndex["{-FN-}"];
  TString ntName    = mapIndex["{-NT-}"];
  TString var0      = mapIndex["{-VA-}"];  TString variable = FSTree::expandVariable(var0);
    if (variable == ""){ STATUS = "!!NO_VAR2!!"; return getTHNFBasicEmpty(index); }
  TString bounds    = mapIndex["{-BO-}"];
  TString cuts      = mapIndex["{-CU-}"];  cuts = FSTree::expandVariable(cuts);
    int maxEntries = -1;
    if (cuts.Contains("MAXENTRIES=") || cuts.Contains("MAXEVENTS=")){
      pair<int,TString> pairMaxEntries = FSTree::processMaxEntries(cuts);
      maxEntries = pairMaxEntries.first;
      cuts = pairMaxEntries.second;
    }
  double  scale     = FSString::TString2double(mapIndex["{-SC-}"]);
  if (FSControl::DEBUG){
    cout << "FSHistogram::getTHNFBasicTree DEBUG: making histogram from tree" << endl;
    printIndexInfo(index);
  }

    // set up to make the histogram
  TH1F* hist1d = NULL; TH2F* hist2d = NULL;
  TString hname = makeFSRootTempName();
  TString hbounds(hname); hbounds += bounds;
  TString sScale(FSString::double2TString(scale,8,true));
  TString scaleTimesCuts(sScale);
  if (cuts != "") scaleTimesCuts += ("*("+cuts+")");

  // use project to create a histogram
  TString treeSTATUS;
  TChain* chain = FSTree::getTChain(fileName,ntName,treeSTATUS);
  if (treeSTATUS.Contains("!!")){ STATUS = treeSTATUS; return getTHNFBasicEmpty(index); } 
  TH1::AddDirectory(true); TH2::AddDirectory(true);
  if (maxEntries < 0)  chain->Project(hbounds, variable, scaleTimesCuts);
  if (maxEntries >= 0) chain->Project(hbounds, variable, scaleTimesCuts,"",maxEntries);
  if (dimension == 1) hist1d = (TH1F*) gDirectory->FindObject(hname); 
  if (dimension == 2) hist2d = (TH2F*) gDirectory->FindObject(hname); 
  if ((dimension == 1 && !hist1d) || (dimension == 2 && !hist2d)){
    cout << "FSHistogram::getTHNFBasicTree ERROR: problem finding histogram named: " << hname << endl;
    return getTHNFBasicEmpty(index);
  }

    // return the created histogram
  if (hist1d) getTH1F(hist1d)->SetName(hname);
  if (hist2d) getTH2F(hist2d)->SetName(hname);
  if (hist1d) hist1d->SetTitle(var0);
  if (hist2d) hist2d->SetTitle(var0);
  if (FSControl::DEBUG){
    cout << "FSHistogram::getTHNFBasicTree DEBUG: finished making histogram from tree" << endl;
    printIndexInfo(index);
  }
  return pair<TH1F*,TH2F*>(hist1d,hist2d);
} 


#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
pair< ROOT::RDF::RResultPtr<TH1D>, ROOT::RDF::RResultPtr<TH2D> >
FSHistogram::getTHNFBasicTreeRDF(TString index, TString& STATUS){
  pair< ROOT::RDF::RResultPtr<TH1D>, ROOT::RDF::RResultPtr<TH2D> > histPairRDF;

    // initial checks
  STATUS = checkIndex(index,"TREE");
  if (STATUS.Contains("!!")) return histPairRDF; 

    // adjust the input parameters
  map<TString,TString> mapIndex = parseHistogramIndex(index);
  int     dimension = FSString::TString2int(mapIndex["{-ND-}"]);
  TString fileName  = mapIndex["{-FN-}"];
  TString ntName    = mapIndex["{-NT-}"];
  TString variable  = mapIndex["{-VA-}"];  variable = FSTree::expandVariable(variable);
  TString bounds    = mapIndex["{-BO-}"];
  TString cuts      = mapIndex["{-CU-}"];  cuts = FSTree::expandVariable(cuts);
  double  scale     = FSString::TString2double(mapIndex["{-SC-}"]);
  if (cuts == "") cuts = "(1==1)";
  string sFN   = FSString::TString2string(fileName);
  string sNT   = FSString::TString2string(ntName);
  string sVARX = FSString::TString2string(variable);
  string sVARY = "";
  if (dimension == 2){ sVARX = FSString::TString2string(FSString::parseVariableX(variable));
                       sVARY = FSString::TString2string(FSString::parseVariableY(variable)); }
  string sCUTS = FSString::TString2string(cuts);
  int nbinsx = FSString::parseBoundsNBinsX(bounds);
  double xlow = FSString::parseBoundsLowerX(bounds);
  double xhigh = FSString::parseBoundsUpperX(bounds);
  int nbinsy = FSString::parseBoundsNBinsY(bounds);
  double ylow = FSString::parseBoundsLowerY(bounds);
  double yhigh = FSString::parseBoundsUpperY(bounds);
  TString rdfIndex = "{-FN-}"+fileName + "{-NT-}"+ntName;
  TString hname = makeFSRootTempName();
  if (FSControl::DEBUG){
    cout << "FSHistogram::getTHNFBasicTreeRDF DEBUG: setting up histogram" << endl;
    printIndexInfo(index);
  }

    // find the right RDataFrame object
  ROOT::RDataFrame* RDF;
  if (m_RDataFrameCache.find(rdfIndex) != m_RDataFrameCache.end()){
    RDF = m_RDataFrameCache[rdfIndex]; 
  }
  else {
/*
    vector<TString> allFiles = FSSystem::getAbsolutePaths(fileName);
    vector<string> rootFiles;
    for (unsigned int i = 0; i < allFiles.size(); i++){
      if (FSSystem::checkRootFormat(allFiles[i]))
        rootFiles.push_back(FSString::TString2string(allFiles[i]));
    }
    if (rootFiles.size() == 0){ STATUS = "!!NO_FILE!!";  return histPairRDF; } 
    vector<string> rootFiles2;
    for (unsigned int i = 0; i < rootFiles.size(); i++){
      TFile file(FSString::string2TString(rootFiles[i]));
      if (file.FindObjectAny(ntName))
        rootFiles2.push_back(FSString::TString2string(rootFiles[i]));
    }
    if (rootFiles2.size() == 0){ STATUS = "!!NO_TREE!!";  return histPairRDF; } 
    RDF = new ROOT::RDataFrame(sNT, rootFiles2);
*/
    TString treeSTATUS;  TTree* ntPointer = FSTree::getTChain(fileName,ntName,treeSTATUS);
    if (treeSTATUS.Contains("!!")){ STATUS = "!!NO_TREE!!"; return histPairRDF; }
    RDF = new ROOT::RDataFrame(*ntPointer);
    m_RDataFrameCache[rdfIndex] = RDF;
  }

    // find the name of the variable
  TString varNameX(""); string sVarNameX("");
  TString varNameY(""); string sVarNameY("");
  if (m_RDFVariableDefinitions.find(variable) != m_RDFVariableDefinitions.end()){
    varNameX = m_RDFVariableDefinitions[variable].first;
    varNameY = m_RDFVariableDefinitions[variable].second;
  }
  else{
    varNameX = "VARX" + FSString::int2TString(m_RDFVariableCounter++);
    varNameY = "VARY" + FSString::int2TString(m_RDFVariableCounter);
    m_RDFVariableDefinitions[variable] = pair<TString,TString>(varNameX,varNameY);
  }
  sVarNameX = FSString::TString2string(varNameX);
  sVarNameY = FSString::TString2string(varNameY);

    // set up the histogram
  if (dimension == 1){
    histPairRDF.first = RDF->Filter(sCUTS)
                            .Define(sVarNameX,sVARX)
                            .Histo1D({hname,hname,nbinsx,xlow,xhigh},sVarNameX);
  }
  if (dimension == 2){
    histPairRDF.second = RDF->Filter(sCUTS)
                            .Define(sVarNameX,sVARX).Define(sVarNameY,sVARY)
                            .Histo2D({hname,hname,nbinsx,xlow,xhigh,nbinsy,ylow,yhigh},sVarNameX,sVarNameY);
  }

  return histPairRDF;

}
#endif


pair<TH1F*,TH2F*> 
FSHistogram::getTHNFBasicEmpty(TString index){

    // adjust the input parameters
  map<TString,TString> mapIndex = parseHistogramIndex(index);
  int     dimension = FSString::TString2int(mapIndex["{-ND-}"]);
  TString bounds    = mapIndex["{-BO-}"];
  if (FSControl::DEBUG){
    cout << "FSHistogram::getTHNFBasicEmpty DEBUG: making empty histogram" << endl;
    printIndexInfo(index);
  }

    // basic checks
  if ((dimension != 1) && (dimension != 2)) dimension = 1;
  if (!FSString::checkBounds(dimension,bounds)) bounds = FSString::makeBounds(dimension);

    // set up to make the histogram
  TH1F* hist1d = NULL; TH2F* hist2d = NULL;
  int nbins = FSString::parseBoundsNBinsX(bounds);
  double low = FSString::parseBoundsLowerX(bounds);
  double high = FSString::parseBoundsUpperX(bounds);
  int nbinsy = FSString::parseBoundsNBinsY(bounds);
  double lowy = FSString::parseBoundsLowerY(bounds);
  double highy = FSString::parseBoundsUpperY(bounds);

    // make the histogram
  TString hname = makeFSRootTempName();
  if (dimension == 1) hist1d = new TH1F(hname,"",nbins,low,high);
  if (dimension == 2) hist2d = new TH2F(hname,"",nbins,low,high,nbinsy,lowy,highy);

    // return the histogram 
  if (hist1d){ getTH1F(hist1d)->SetName(hname); }
  if (hist2d){ getTH2F(hist2d)->SetName(hname); }
  return pair<TH1F*,TH2F*>(hist1d,hist2d);
} 




  // ********************************************************
  // CREATE A TREE IN THE SAME WAY AS A HISTOGRAM
  // ********************************************************


TTree*
FSHistogram::getTH1FContents(TString fileName, TString ntName, TString variable, TString bounds, 
                             TString cuts, double scale,
                             vector< pair<TString,TString> > extraTreeContents){
  pair<TString, vector<TString> > 
    indices = getHistogramIndexTree(1,fileName,ntName,variable,bounds,cuts,scale);
  return getFSHistogramInfo(indices.first,indices.second)->getTHNFContents(extraTreeContents);
}

TTree*
FSHistogram::getTH2FContents(TString fileName, TString ntName, TString variable, TString bounds, 
                             TString cuts, double scale,
                             vector< pair<TString,TString> > extraTreeContents){
  pair<TString, vector<TString> > 
    indices = getHistogramIndexTree(2,fileName,ntName,variable,bounds,cuts,scale);
  return getFSHistogramInfo(indices.first,indices.second)->getTHNFContents(extraTreeContents);
}


TTree* 
FSHistogram::getTHNFBasicContents(TTree* histTree, TString index,
                                vector< pair<TString,TString> > extraTreeContents, TString& STATUS){

    // initial checks
  STATUS = checkIndex(index,"TREE");
  if (STATUS.Contains("!!")) return histTree;

    // adjust the input parameters
  map<TString,TString> mapIndex = parseHistogramIndex(index);
  int     dimension = FSString::TString2int(mapIndex["{-ND-}"]);
  TString fileName  = mapIndex["{-FN-}"];
  TString ntName    = mapIndex["{-NT-}"];
  TString variable  = mapIndex["{-VA-}"];  variable = FSTree::expandVariable(variable);
  TString bounds    = mapIndex["{-BO-}"];
  TString cuts      = mapIndex["{-CU-}"];  cuts = FSTree::expandVariable(cuts);
  double  scale     = FSString::TString2double(mapIndex["{-SC-}"]);
  for (unsigned int i = 0; i < extraTreeContents.size(); i++){
    extraTreeContents[i].first  = FSString::removeWhiteSpace(extraTreeContents[i].first);
    extraTreeContents[i].second = FSString::removeWhiteSpace(extraTreeContents[i].second);
    extraTreeContents[i].second = FSTree::expandVariable(extraTreeContents[i].second);
  }
  if (FSControl::DEBUG){
    cout << "FSHistogram::getTHNFBasicContents DEBUG: making tree of histogram contents" << endl;
    printIndexInfo(index);
  }

    // create a new tree if one hasn't been passed in
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
  }

    // get the x and y variable names
  TString varX("");  if (dimension >= 1) varX = FSString::parseVariableX(variable);
  TString varY("");  if (dimension == 2) varY = FSString::parseVariableY(variable);

    // set the addresses of the values to add to histTree
  Double_t x = 0.0;  if (dimension >= 1) histTree->SetBranchAddress("x",  &x);
  Double_t y = 0.0;  if (dimension == 2) histTree->SetBranchAddress("y",  &y);
  Double_t wt = 0.0; if (dimension >= 1) histTree->SetBranchAddress("wt", &wt);
  Double_t extraValues[200];
  if (extraTreeContents.size() > 200){
    cout << "FSHistogram::getTHNFBasicContents ERROR: too many extra branches" << endl;
    exit(0);
  }
  for (unsigned int i = 0; i < extraTreeContents.size(); i++){
    TString contentName = extraTreeContents[i].first;
    histTree->SetBranchAddress(contentName,&extraValues[i]);
  }

    // get the old tree
  TString treeSTATUS;
  TTree* nt = FSTree::getTChain(fileName,ntName,treeSTATUS);
  if (treeSTATUS.Contains("!!")){ STATUS = treeSTATUS; return histTree; }

    // set up the TTreeFormula objects
  if (cuts == "") cuts = "(1==1)";
  TObjArray* formulas = new TObjArray();
  TTreeFormula* cutsF = NULL; 
  TTreeFormula* varXF = NULL; 
  TTreeFormula* varYF = NULL; 
  if (dimension >= 1){ cutsF = new TTreeFormula("cutsF", cuts, nt); formulas->Add(cutsF); }
  if (dimension >= 1){ varXF = new TTreeFormula("varXF", varX, nt); formulas->Add(varXF); }
  if (dimension == 2){ varYF = new TTreeFormula("varYF", varY, nt); formulas->Add(varYF); }
  vector<TTreeFormula*> varEF;
  for (unsigned int i = 0; i < extraTreeContents.size(); i++){
    TString varName("varEF"); varName += i;
    TTreeFormula* cutEXTRA = new TTreeFormula(varName,extraTreeContents[i].second,nt);
    formulas->Add(cutEXTRA);
    varEF.push_back(cutEXTRA);
  }
  nt->SetNotify(formulas);

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

    // delete the TTreeFormula objects and return
  if (cutsF) delete cutsF;
  if (varXF) delete varXF;
  if (varYF) delete varYF;
  for (unsigned int i = 0; i < varEF.size(); i++){
    if (varEF[i]) delete varEF[i];
  }
  nt->SetNotify(nullptr);
  delete formulas;
  return histTree;
}



  // ********************************************
  //  CREATE HISTOGRAMS FROM FUNCTIONS
  // ********************************************

TH1F*
FSHistogram::getTH1FFormula(TString formula, TString bounds, int numRandomTrials){
  TString index = getHistogramIndexFormula(1,formula,bounds);
  TH1F* hist = getFSHistogramInfo(index)->getTHNF().first;
  if (numRandomTrials > 0) hist = getTH1FRandom(hist,numRandomTrials);
  return getTH1F(hist);
}

TH2F*
FSHistogram::getTH2FFormula(TString formula, TString bounds, int numRandomTrials){
  TString index = getHistogramIndexFormula(2,formula,bounds);
  TH2F* hist = getFSHistogramInfo(index)->getTHNF().second;
  if (numRandomTrials > 0) hist = getTH2FRandom(hist,numRandomTrials);
  return getTH2F(hist);
}

TH1F*
FSHistogram::getTH1FFormula(TF1* function, TString bounds, int numRandomTrials){
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
  return getTH1FFormula(function->GetFormula()->GetExpFormula(),bounds,numRandomTrials);
#else
  cout << "getTH1Formula(" << function->GetName() << "," << bounds << ","
           << numRandomTrials << ")" << endl;
  cout << "ROOT version too old" << endl; exit(0);
#endif
}

TH2F*
FSHistogram::getTH2FFormula(TF2* function, TString bounds, int numRandomTrials){
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
  return getTH2FFormula(function->GetFormula()->GetExpFormula(),bounds,numRandomTrials);
#else
  cout << "getTH2Formula(" << function->GetName() << "," << bounds << ","
           << numRandomTrials << ")" << endl;
  cout << "ROOT version too old" << endl; exit(0);
#endif
}


pair<TH1F*,TH2F*>
FSHistogram::getTHNFBasicFormula(TString index, TString& STATUS){

    // initial checks
  STATUS = checkIndex(index,"FORMULA");
  if (STATUS.Contains("!!")) return getTHNFBasicEmpty(index); 

    // adjust the input parameters
  map<TString,TString> mapIndex = parseHistogramIndex(index);
  int     dimension = FSString::TString2int(mapIndex["{-ND-}"]);
  TString formula   = mapIndex["{-FO-}"];  formula  = FSString::expandSUM(formula);
  TString bounds    = mapIndex["{-BO-}"];
  TString histName = makeFSRootTempName();
  if (FSControl::DEBUG){
    cout << "FSHistogram::getTHNFBasicFormula DEBUG: making histogram from formula" << endl;
    printIndexInfo(index);
  }
  
    // set up to make the histogram
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
  pair<TH1F*,TH2F*> histPair = getTHNFBasicEmpty(index);
  TH1F* hist1d = histPair.first;  TH2F* hist2d = histPair.second;
  if (hist1d) hist1d->SetName(histName);  if (hist2d) hist2d->SetName(histName);

    // loop over bins and fill with the formula
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

    // return the histogram
  if (hist1d){ getTH1F(hist1d)->SetName(histName); }
  if (hist2d){ getTH2F(hist2d)->SetName(histName); }
  if (hist1d){ hist1d->SetTitle("f(x) = "+formula); hist1d->SetXTitle("x"); }
  if (hist2d){ hist2d->SetTitle("f(x,y) = "+formula); hist2d->SetXTitle("x"); hist2d->SetYTitle("y"); }
  return pair<TH1F*,TH2F*>(hist1d,hist2d);
}

void
FSHistogram::draw2dParametricFormula(TString xFormula, TString yFormula, TString bounds, int markerType, double markerSize){
  if (!FSString::checkBounds(1,bounds)) {cout << "problem with bounds: " << bounds << endl; return; }
  int    npts   = FSString::parseBoundsNBinsX(bounds);
  double lowpt  = FSString::parseBoundsLowerX(bounds);
  double highpt = FSString::parseBoundsUpperX(bounds);
  TFormula xF("xF",xFormula);
  TFormula yF("yF",yFormula);
  for (int i = 0; i < npts; i++){
    double x = lowpt + (i+0.5)*(highpt-lowpt)/npts;
    TMarker* marker = new TMarker(xF.Eval(x),yF.Eval(x),markerType);
    marker->SetMarkerSize(markerSize);
    marker->Draw();
  }
}


  // ********************************************
  //  MAKE PROJECTIONS, DO INTEGRALS, ETC.
  // ********************************************


TH1F*
FSHistogram::getTH1FRandom(TH1F* hist, int numRandomTrials){
  if (!hist) return NULL;
  TString rname(hist->GetName()); rname += "__"; rname += makeFSRootTempName();
  TH1F* rhist = getTH1F((TH1F*)hist->Clone(rname));  rhist->Reset();
  TAxis* axisX = rhist->GetXaxis(); 
  int nbinsX = axisX->GetNbins(); 
  double fMax = hist->GetBinContent(hist->GetMaximumBin());
  int numAccepted = 0;  //int numAttempts = 0;
  while (numAccepted < numRandomTrials){
    //numAttempts++;
    int iX = 1+(int)gRandom->Uniform(0.0,nbinsX);
    double f0 = gRandom->Uniform(0.0,fMax);
    double f = hist->GetBinContent(iX);
    if (f0 <= f){
      numAccepted++;
      rhist->Fill(axisX->GetBinCenter(iX));
    }
  }
  rhist = FSHistogram::getTH1F(rhist);
  TString title(hist->GetTitle());  rhist->SetTitle("Random from "+title);
  return rhist;
}

TH2F*
FSHistogram::getTH2FRandom(TH2F* hist, int numRandomTrials){
  if (!hist) return NULL;
  TString rname(hist->GetName()); rname += "__"; rname += makeFSRootTempName();
  TH2F* rhist = getTH2F((TH2F*)hist->Clone(rname));  rhist->Reset();
  TAxis* axisX = rhist->GetXaxis(); TAxis* axisY = rhist->GetYaxis();
  int nbinsX = axisX->GetNbins();   int nbinsY = axisX->GetNbins();
  double fMax = hist->GetBinContent(hist->GetMaximumBin());
  int numAccepted = 0;  //int numAttempts = 0;
  while (numAccepted < numRandomTrials){
    //numAttempts++;
    int iX = 1+(int)gRandom->Uniform(0.0,nbinsX); int iY = 1+(int)gRandom->Uniform(0.0,nbinsY);
    double f0 = gRandom->Uniform(0.0,fMax);
    double f = hist->GetBinContent(iX,iY);
    if (f0 <= f){
      numAccepted++;
      rhist->Fill(axisX->GetBinCenter(iX),axisY->GetBinCenter(iY));
    }
  }
  rhist = FSHistogram::getTH2F(rhist);
  TString title(hist->GetTitle());  rhist->SetTitle("Random from "+title);
  return rhist;
}


TH1F*
FSHistogram::getTH1F(TH2F* hist2d, TString projectionAxis, bool function,
                      double range1, double range2){
  TH1F* hist = NULL;
  if (!hist2d) return hist;
  projectionAxis.ToUpper();
  if ((projectionAxis != "X") && (projectionAxis != "Y")) return hist;
  TAxis* axis1 = hist2d->GetXaxis(); if (projectionAxis == "Y") axis1 = hist2d->GetYaxis();
  TAxis* axis2 = hist2d->GetYaxis(); if (projectionAxis == "Y") axis2 = hist2d->GetXaxis();
  int nbins1 = axis1->GetNbins();
  int nbins2 = axis2->GetNbins();
  double low1  = axis1->GetXmin();
  double high1 = axis1->GetXmax();
  if ((nbins1 == 0) || (nbins2 == 0)) return hist;
  if (low1 >= high1) return hist;
  hist = new TH1F("histProject","",nbins1,low1,high1);
  for (int i1 = 1; i1 <= nbins1; i1++){
    double proj = 0.0;
    double eproj = 0.0;
    double eproj2 = 0.0;
    for (int i2 = 1; i2 <= nbins2; i2++){
      double r0 = axis2->GetBinCenter(i2);
      if ((range2 > range1) && ((r0 < range1) || (r0 > range2))) continue;
      int ix = i1; int iy = i2;
      if (projectionAxis == "Y"){ ix = i2; iy = i1; }
      proj += hist2d->GetBinContent(ix,iy); 
      eproj = hist2d->GetBinError(ix,iy);
      eproj2 += (eproj*eproj);
    }
    eproj = sqrt(eproj2);
    if (function){ double w = axis2->GetBinWidth(1); proj *= w; eproj *= w; }
    hist->SetBinContent(i1,proj);
    hist->SetBinError(i1,eproj);
  }
  hist = getTH1F(hist);
  hist->SetTitle(hist2d->GetTitle());
  if (projectionAxis == "X"){ TAxis* axis = hist2d->GetXaxis(); hist->SetXTitle(axis->GetTitle()); }
  if (projectionAxis == "Y"){ TAxis* axis = hist2d->GetYaxis(); hist->SetXTitle(axis->GetTitle()); }
  return hist;
}

pair<double,double>
FSHistogram::integral(TH1F* hist, bool function, double x1, double x2){
  if (!hist) return pair<double,double> (0.0,0.0);
  if (hist->GetNbinsX() == 0) return pair<double,double> (0.0,0.0);
  if (hist->GetBinWidth(1) == 0) return pair<double,double> (0.0,0.0);
  int nbins = hist->GetNbinsX();
  double total = 0.0;
  double etotal = 0.0;
  double etotal2 = 0.0;
  for (int i = 1; i <= nbins; i++){
    double x0 = hist->GetBinCenter(i);
    if ((x2 > x1) && ((x0 < x1) || (x0 > x2))) continue;
    total += hist->GetBinContent(i);
    etotal = hist->GetBinError(i);
    etotal2 += etotal*etotal;
  }
  etotal = sqrt(etotal2);
  if (function){ double w = hist->GetBinWidth(1); total *= w; etotal *= w; }
  return pair<double,double> (total,etotal);
}

pair<double,double>
FSHistogram::integral(TH2F* hist, bool function, double x1, double x2, double y1, double y2){
  if (!hist) return pair<double,double> (0.0,0.0);
  TH1F* hist1d = getTH1F(hist,"X",function,y1,y2);
  pair<double,double> answer = integral(hist1d,function,x1,x2);
  delete hist1d;
  return answer;
}


  // ********************************************************
  // SET MAXIMA/MINIMA SO HISTOGRAMS CAN BE SHOWN TOGETHER
  // ********************************************************

void
FSHistogram::setHistogramMaxMin(TH1F* hist, bool zeroSuppression, TString MAXMIN, double xlow, double xhigh){
  vector<TH1F*> histVector;  histVector.push_back(hist);
  setHistogramMaxMin(histVector, zeroSuppression, MAXMIN, xlow, xhigh);
}

void
FSHistogram::setHistogramMaxMin(TH2F* hist, bool zeroSuppression, TString MAXMIN){
  vector<TH2F*> histVector;  histVector.push_back(hist);
  setHistogramMaxMin(histVector, zeroSuppression, MAXMIN);
}

void
FSHistogram::setHistogramMaxMin(vector<TH1F*> histVector, bool zeroSuppression, TString MAXMIN, double xlow, double xhigh){
  vector<TH1F*> histVector2;
  for (unsigned int i = 0; i < histVector.size(); i++){
    if (histVector[i] && histVector[i]->GetNbinsX() > 0)
      histVector2.push_back(histVector[i]); }
  if (histVector2.size() == 0) return;
  double min = histVector2[0]->GetBinContent(1);
  double max = min;
  for (unsigned int i = 0; i < histVector2.size(); i++){
    TH1F* hist = histVector2[i];
    for (int j = 1; j <= hist->GetNbinsX(); j++){
      if (xlow < xhigh && (hist->GetBinCenter(j) < xlow || hist->GetBinCenter(j) > xhigh)) continue;
      float max0 = hist->GetBinContent(j) + hist->GetBinError(j);
      if (max0 > max) max = max0;
      float min0 = hist->GetBinContent(j) - hist->GetBinError(j);
      if (min0 < 0.0 && fabs(min0)/fabs(hist->GetBinContent(j) + hist->GetBinError(j)) < 5.0e-7) min0 = 0.0;
      if (min0 < min) min = min0;
    }
  }
  if (min > 0 && !zeroSuppression) min = 0.0;
  if (max < 0 && !zeroSuppression) max = 0.0;
       if (min == 0.0 && max  > 0.0){ max = max*1.05; }
  else if (min  < 0.0 && max == 0.0){ min = min*1.05; }
  else if (min == 0.0 && max == 0.0){ max = 1.0; }
  else { double h = max - min; if (max == min) h = max;
         max = max + fabs(h)*0.05;  min = min - fabs(h)*0.05; }
  for (unsigned int i = 0; i < histVector2.size(); i++){
    TH1F* hist = histVector2[i];
    if (MAXMIN.Contains("MAX")) hist->SetMaximum(max);
    if (MAXMIN.Contains("MIN")) hist->SetMinimum(min);
  }
}

void
FSHistogram::setHistogramMaxMin(vector<TH2F*> histVector, bool zeroSuppression, TString MAXMIN){
  vector<TH2F*> histVector2;
  for (unsigned int i = 0; i < histVector.size(); i++){
    if (histVector[i] && histVector[i]->GetNbinsX() > 0 && histVector[i]->GetNbinsY() > 0)
      histVector2.push_back(histVector[i]); }
  if (histVector2.size() == 0) return;
  double min = histVector2[0]->GetBinContent(1,1);
  double max = min;
  for (unsigned int i = 0; i < histVector2.size(); i++){
    TH2F* hist = histVector2[i];
    for (int j = 1; j <= hist->GetNbinsX(); j++){
    for (int k = 1; k <= hist->GetNbinsY(); k++){
      float max0 = hist->GetBinContent(j,k);
      if (max0 > max) max = max0;
      float min0 = hist->GetBinContent(j,k);
      if (min0 < min) min = min0;
    }}
  }
  if (min > 0 && !zeroSuppression) min = 0.0;
  if (max < 0 && !zeroSuppression) max = 0.0;
       if (min == 0.0 && max  > 0.0){ max = max*1.05; }
  else if (min  < 0.0 && max == 0.0){ min = min*1.05; }
  else if (min == 0.0 && max == 0.0){ max = 1.0; }
  else { double h = max - min; if (max == min) h = max;
         max = max + fabs(h)*0.05;  min = min - fabs(h)*0.05; }
  for (unsigned int i = 0; i < histVector2.size(); i++){
    TH2F* hist = histVector2[i];
    if (MAXMIN.Contains("MAX")) hist->SetMaximum(max);
    if (MAXMIN.Contains("MIN")) hist->SetMinimum(min);
  }
}




  // ********************************************************
  // DUMP THE HISTOGRAM CACHE
  // ********************************************************

void 
FSHistogram::dumpHistogramCache(TString cacheName, TString selectIndex){

    // first sort (to make debugging easier)

  vector<FSHistogramInfo*> vecHistInfo;
  for (map<TString,FSHistogramInfo*>::iterator mpItr = m_FSHistogramInfoCache.begin();
       mpItr != m_FSHistogramInfoCache.end(); mpItr++){
    if ((mpItr->second) && (mpItr->second->getHistName() != "") &&
        FSString::compareTStrings(mpItr->second->m_index,selectIndex)) 
      vecHistInfo.push_back(mpItr->second);
  }
  if (vecHistInfo.size() == 0) return;
  for (unsigned int i = 0; i < vecHistInfo.size()-1; i++){
  for (unsigned int j = i+1; j < vecHistInfo.size(); j++){
    if (FSString::TString2string(vecHistInfo[j]->getHistName()) <
        FSString::TString2string(vecHistInfo[i]->getHistName())){
      FSHistogramInfo* temp = vecHistInfo[i];
      vecHistInfo[i] = vecHistInfo[j];
      vecHistInfo[j] = temp;
    }
  }}

    // open output files

  string textCacheName = FSString::TString2string(cacheName);  textCacheName += ".cache.dat";
  string rootCacheName = FSString::TString2string(cacheName);  rootCacheName += ".cache.root";
  TFile* rootCache = new TFile(rootCacheName.c_str(),"recreate");
  ofstream textCache(textCacheName.c_str());

    // write to files

  for (unsigned int i = 0; i < vecHistInfo.size(); i++){
    rootCache->cd();
    FSHistogramInfo* histInfo = vecHistInfo[i];
    TString histIndex = histInfo->m_index;
    if (!histInfo->basic()) continue;
    if (histInfo->waitingForEventLoop()) continue;
    if (histInfo->status().Contains("!!")) continue;
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

    // close files

  textCache.close();
  rootCache->Close();
  delete rootCache;

}



  // ********************************************************
  // READ THE HISTOGRAM CACHE
  // ********************************************************

void 
FSHistogram::readHistogramCache(TString cacheName, TString selectIndex){

  TString sCacheNameWithWildcards = cacheName;
  vector<TString> textCacheNames = FSSystem::getAbsolutePaths(sCacheNameWithWildcards+".cache.dat");
  vector<TString> rootCacheNames = FSSystem::getAbsolutePaths(sCacheNameWithWildcards+".cache.root");
  if ((textCacheNames.size() == 1) && (rootCacheNames.size() == 1)){
    cout << "READING CACHE: " << cacheName << endl;
    cout << "    root file = " << rootCacheNames[0] << endl;
    cout << "    data file = " << textCacheNames[0] << endl;
  }
  else{
    cout << "PREPARING TO READ CACHE: " << cacheName << endl;
    cout << "    ROOT FILES: " << endl;
    for (unsigned int i = 0; i < rootCacheNames.size(); i++){
      cout << "      (" << i+1 << ") " << rootCacheNames[i] << endl;
    }
    cout << "    DATA FILES: " << endl;
    for (unsigned int i = 0; i < textCacheNames.size(); i++){
      cout << "      (" << i+1 << ") " << textCacheNames[i] << endl;
    }
    if ((rootCacheNames.size() == 0) && (textCacheNames.size() == 0)){
      cout << "  No histogram cache found...  skipping" << endl;  return;
    }
    if (rootCacheNames.size() != textCacheNames.size()){
      cout << "  Problem with cache files...  skipping" << endl;  return;
    }
    for (unsigned int i = 0; i < textCacheNames.size(); i++){
      TString sCacheNameNoWildCards =
         FSString::subString(textCacheNames[i],0,textCacheNames[i].Length()-10);
      readHistogramCache(sCacheNameNoWildCards,selectIndex);
    }
  }

    // open input files

  TString textCacheName = textCacheNames[0];
  TString rootCacheName = rootCacheNames[0];
  TFile* rootCache = new TFile(rootCacheName);
  ifstream textCache(FSString::TString2string(textCacheName).c_str());

    // read histograms

  TString histName;
  TString fileName(rootCacheName);
  TString index;
  while (textCache >> index){
    textCache >> histName;
    if (!FSString::compareTStrings(index,selectIndex)) continue;
    if (FSControl::DEBUG) cout << "FSHistogram: reading cache" << endl;
    if (FSControl::DEBUG) cout << "\tname  = " << histName << endl;
    if (FSControl::DEBUG) cout << "\tindex = " << index << endl;
    int dim = 1;
    if (index.Contains("{-ND-}1D")) dim = 1;
    if (index.Contains("{-ND-}2D")) dim = 2;
    cout << "FSHistogram:  READ HISTOGRAM...          " << std::flush;
    if (getFSHistogramInfo(index)->getHistName() != ""){
      cout << "histogram already exists: " << getFSHistogramInfo(index)->getHistName() << endl;
      continue;
    }
    //TString cacheIndex(getHistogramIndexFile(dim,fileName,histName)); TString STATUS;
    //pair<TH1F*,TH2F*> histPair = getTHNFBasicFile(cacheIndex,STATUS);
    pair<TH1F*,TH2F*> histPair = pair<TH1F*,TH2F*>(NULL,NULL);
    if (dim == 1) histPair.first  = (TH1F*) rootCache->FindObjectAny(histName);
    if (dim == 2) histPair.second = (TH2F*) rootCache->FindObjectAny(histName);
    if (histPair.first)  getTH1F(histPair.first)->SetName(makeFSRootHistName());
    if (histPair.second) getTH2F(histPair.second)->SetName(makeFSRootHistName());
    getFSHistogramInfo(index)->m_histPair = histPair;
    cout << getFSHistogramInfo(index)->infoString() << endl;
  }

    // close files

  textCache.close();
  rootCache->Close();
  delete rootCache;

}


  // ********************************************************
  // CLEAR GLOBAL CACHES
  // ********************************************************


void
FSHistogram::clearHistogramCache(TString histName, TString selectIndex){
  if (FSControl::DEBUG) 
    cout << "FSHistogram: clearing histogram cache " << histName << endl;
  vector<TString> indices = getFSHistogramInfoCacheIndices(histName,false,selectIndex);
  for (unsigned int iIndex = 0; iIndex < indices.size(); iIndex++){
    FSHistogramInfo* hInfo = m_FSHistogramInfoCache[indices[iIndex]];
    m_FSHistogramInfoCache.erase(indices[iIndex]);
    if (!hInfo) continue;
    hInfo->m_basicHistograms.clear();
    if (hInfo->m_histPair.first)  delete hInfo->m_histPair.first;
    if (hInfo->m_histPair.second) delete hInfo->m_histPair.second;
    delete hInfo;
  }
  if (FSControl::DEBUG) 
    cout << "FSHistogram: done clearing histogram cache" << endl;
}

void
FSHistogram::clearHistogramCache(int histNumber, TString selectIndex){
  if (histNumber <= 0) clearHistogramCache("", selectIndex);
  if (histNumber > 0)  clearHistogramCache(makeFSRootHistName(histNumber), selectIndex);
}



void
FSHistogram::showHistogramCache(TString histName, bool showDetails, TString selectIndex){
  vector<TString> indices = getFSHistogramInfoCacheIndices(histName,true,selectIndex);
  cout << "-------------------------------------" << endl;
  cout << "-- CONTENTS OF THE HISTOGRAM CACHE --" << endl;
  cout << "-------------------------------------" << endl;
  if (indices.size() == 0) return;
  for (unsigned int i = 0; i < indices.size()-1; i++){
  for (unsigned int j = i+1; j < indices.size(); j++){
    if (FSString::TString2string(m_FSHistogramInfoCache[indices[j]]->getHistName()) <
        FSString::TString2string(m_FSHistogramInfoCache[indices[i]]->getHistName())) {
      TString temp = indices[i];
      indices[i]   = indices[j];
      indices[j]   = temp;
    }
  }}
  for (unsigned int i = 0; i < indices.size(); i++){
    m_FSHistogramInfoCache[indices[i]]->show(showDetails);
    if (!showDetails) cout << "-------------------------------------" << endl;
  }
}


void
FSHistogram::showHistogramCache(int histNumber, bool showDetails, TString selectIndex){
  if (histNumber <= 0) showHistogramCache("",showDetails,selectIndex);
  if (histNumber > 0)  showHistogramCache(makeFSRootHistName(histNumber),showDetails,selectIndex);
}


TString
FSHistogram::getHistogramInfo(TString histName, TString indexComponent, TString selectIndex){
  vector<TString> indices = getFSHistogramInfoCacheIndices(histName,true,selectIndex);
  if (indices.size() != 1) return TString("");
  if (indexComponent == "") return indices[0];
  if (indexComponent.Length() != 2) return TString("");
  indexComponent = "{-"+indexComponent+"-}";
  map<TString,TString> indexMap = parseHistogramIndex(indices[0]);
  if (indexMap.find(indexComponent) == indexMap.end()) return TString("");
  return indexMap[indexComponent];
}

TString
FSHistogram::getHistogramInfo(int histNumber, TString indexComponent, TString selectIndex){
  if (histNumber <= 0) return TString("");
  return getHistogramInfo(makeFSRootHistName(histNumber),indexComponent,selectIndex);
}


  // ********************************************************
  // GENERALIZED BASIC FUNCTIONS CREATED FROM INDICES
  // ********************************************************


pair<TH1F*,TH2F*>
FSHistogram::getTHNFBasicIndex(TString index, TString& STATUS){
  if (index.Contains("{-TP-}FILE"))    return getTHNFBasicFile(index,STATUS);
  if (index.Contains("{-TP-}TREE"))    return getTHNFBasicTree(index,STATUS);
  if (index.Contains("{-TP-}EMPTY"))   return getTHNFBasicEmpty(index);
  if (index.Contains("{-TP-}FORMULA")) return getTHNFBasicFormula(index,STATUS);
  return pair<TH1F*,TH2F*>(NULL,NULL);
}


  // *******************************************
  // INTERFACE TO THE FSHISTOGRAMINFO CLASS
  // *******************************************

vector<TString>
FSHistogram::expandHistogramIndexTree(TString index){
  vector<TString> expandedIndices;
  map<TString,TString> indexMap = FSHistogram::parseHistogramIndex(index);
    // expand "var" using FSCut
  if (indexMap.find("{-VA-}") != indexMap.end()){
    TString var = indexMap["{-VA-}"];
    vector< pair<TString,double> > fsCuts = FSCut::expandCuts(var);
    if (fsCuts.size() == 1) indexMap["{-VA-}"] = fsCuts[0].first;
  }
    // expand "cuts" using FSCut and check for multidimensional sidebands
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
  return expandedIndices;
}

FSHistogramInfo*
FSHistogram::getFSHistogramInfo(TString index, vector<TString> subIndices){
  if (FSHistogram::m_FSHistogramInfoCache.find(index) != 
      FSHistogram::m_FSHistogramInfoCache.end())
    return FSHistogram::m_FSHistogramInfoCache[index];
  FSHistogramInfo* histInfo = new FSHistogramInfo(index,subIndices);
  FSHistogram::m_FSHistogramInfoCache[index] = histInfo;
  return histInfo;
}


vector<TString>
FSHistogram::getFSHistogramInfoCacheIndices(TString histName, bool exactOnly, TString selectIndex){
  vector<TString> indices;
  for (map< TString, FSHistogramInfo* >::iterator mapItr = m_FSHistogramInfoCache.begin();
       mapItr != m_FSHistogramInfoCache.end(); mapItr++){
    if (!FSString::compareTStrings(mapItr->first,selectIndex)) continue;
    FSHistogramInfo* histInfo = mapItr->second;
    if (!histInfo) continue;
    if (histName == "" || histName == "*"){ indices.push_back(mapItr->first); continue; }
    if (histName == histInfo->getHistName()){ indices.push_back(mapItr->first); continue; }
    if (exactOnly) continue;
    for (unsigned int i = 0; i < histInfo->m_basicHistograms.size(); i++){
      if (!histInfo->m_basicHistograms[i]) continue;
      if (histName == histInfo->m_basicHistograms[i]->getHistName()){
        indices.push_back(mapItr->first); continue; }
    }
  }
  return indices;
}


  // ***********************************************
  // helper functions for histogram indices
  // ***********************************************

TString
FSHistogram::getHistogramIndex(map<TString,TString> indexMap){
  TString index;
  if (indexMap.find("{-TP-}") != indexMap.end()){ index += "{-TP-}"; index += indexMap["{-TP-}"]; }
  if (indexMap.find("{-ND-}") != indexMap.end()){ index += "{-ND-}"; index += indexMap["{-ND-}"]; }
  if (indexMap.find("{-FO-}") != indexMap.end()){ index += "{-FO-}"; index += indexMap["{-FO-}"]; }
  if (indexMap.find("{-FN-}") != indexMap.end()){ index += "{-FN-}"; index += indexMap["{-FN-}"]; }
  if (indexMap.find("{-NT-}") != indexMap.end()){ index += "{-NT-}"; index += indexMap["{-NT-}"]; }
  if (indexMap.find("{-CA-}") != indexMap.end()){ index += "{-CA-}"; index += indexMap["{-CA-}"]; }
  if (indexMap.find("{-VA-}") != indexMap.end()){ index += "{-VA-}"; index += indexMap["{-VA-}"]; }
  if (indexMap.find("{-BO-}") != indexMap.end()){ index += "{-BO-}"; index += indexMap["{-BO-}"]; }
  if (indexMap.find("{-CU-}") != indexMap.end()){ index += "{-CU-}"; index += indexMap["{-CU-}"]; }
  if (indexMap.find("{-SC-}") != indexMap.end()){ index += "{-SC-}"; index += indexMap["{-SC-}"]; }
  if (indexMap.find("{-HN-}") != indexMap.end()){ index += "{-HN-}"; index += indexMap["{-HN-}"]; }
  return index;
}

TString
FSHistogram::getHistogramIndexFile(int dimension, TString fileName, TString histName){
  TString index;
  index += "{-TP-}FILE";
  if (dimension == 1) index += "{-ND-}1D";
  if (dimension == 2) index += "{-ND-}2D";
  index += "{-FN-}";  index += fileName;
  index += "{-HN-}";  index += histName;
  return FSString::removeWhiteSpace(index);
}

TString
FSHistogram::getHistogramIndexFormula(int dimension, TString formula, TString bounds){
  TString index;
  index += "{-TP-}FORMULA";
  if (dimension == 1) index += "{-ND-}1D";
  if (dimension == 2) index += "{-ND-}2D";
  index += "{-FO-}";  index += formula;
  index += "{-BO-}";  index += bounds;
  return FSString::removeWhiteSpace(index);
}

TString
FSHistogram::getHistogramIndexEmpty(int dimension, TString bounds){
  TString index;
  index += "{-TP-}EMPTY";
  if (dimension == 1) index += "{-ND-}1D";
  if (dimension == 2) index += "{-ND-}2D";
  index += "{-BO-}";  index += bounds;
  return FSString::removeWhiteSpace(index);
}

pair<TString, vector<TString> >
FSHistogram::getHistogramIndexTree(int dimension,
                      TString fileName, TString ntName,
                      TString variable, TString bounds,
                      TString cuts, double scale){
  TString index;
  fileName = FSString::removeWhiteSpace(fileName);
  ntName   = FSString::removeWhiteSpace(ntName);
  variable = FSString::removeWhiteSpace(variable);
  bounds   = FSString::removeWhiteSpace(bounds);
  cuts     = FSString::removeWhiteSpace(cuts);
  if (ntName == "") ntName = FSTree::getTreeNameFromFile(fileName);
  index += "{-TP-}TREE";
  if (dimension == 1) index += "{-ND-}1D";
  if (dimension == 2) index += "{-ND-}2D";
  index += "{-FN-}";  index += fileName;
  index += "{-NT-}";  index += ntName;
  index += "{-VA-}";  index += FSTree::reorderVariable(variable);
  index += "{-BO-}";  index += bounds;
  index += "{-CU-}";  index += FSTree::reorderVariable(cuts);
  index += "{-SC-}";  index += FSString::double2TString(scale,8,true);
  vector<TString> subIndices = expandHistogramIndexTree(index);
  if (subIndices.size() == 1){ index = subIndices[0]; subIndices.clear(); }
  return pair<TString, vector<TString> > (index,subIndices);
}

map<TString,TString>
FSHistogram::parseHistogramIndex(TString index){
  index = FSString::removeWhiteSpace(index);
  vector<TString> spacers; 
  spacers.push_back("{-TP-}");
  spacers.push_back("{-ND-}");
  spacers.push_back("{-FN-}");
  spacers.push_back("{-HN-}");
  spacers.push_back("{-NT-}");
  spacers.push_back("{-CA-}");
  spacers.push_back("{-VA-}");
  spacers.push_back("{-BO-}");
  spacers.push_back("{-CU-}");
  spacers.push_back("{-SC-}");
  spacers.push_back("{-FO-}");
  map<TString,TString> mapWords = FSString::parseTStringToMap1(index,spacers);
  return mapWords;
}  


void
FSHistogram::printIndexInfo(TString index, vector<TString> subIndices){
  cout << "---------------------------------------------------------" << endl;
  cout << "----                PRIMARY HISTOGRAM                ----" << endl;
  cout << "----        (with number of components = "
       << FSString::int2TString(subIndices.size(),3) << ")        ----" << endl;
  cout << "---------------------------------------------------------" << endl;
  printIndexInfo(index);
  for (unsigned int i = 0; i < subIndices.size(); i++){
    cout << "---------------------------------------------------------" << endl;
    cout << "----             COMPONENT HISTOGRAM "
         << FSString::int2TString(i+1,3) << "             ----" << endl;
    cout << "---------------------------------------------------------" << endl;
    printIndexInfo(subIndices[i]);
  }
}

void
FSHistogram::printIndexInfo(TString index){
  map<TString,TString> iMap = parseHistogramIndex(index);
  cout << "-------------- HISTOGRAM INDEX INFORMATION --------------" << endl;
  cout << "\tINDEX       = " << index << endl;
  cout << "\tCACHE NAME  = ";
  if (m_FSHistogramInfoCache.find(index) != m_FSHistogramInfoCache.end()){
    cout << m_FSHistogramInfoCache[index]->infoString() << endl;
  }
  else{ cout << "NOT IN CACHE" << endl; }
  if (iMap.find("{-TP-}") != iMap.end()){ cout << "\tTYPE        = " << iMap["{-TP-}"] << endl; }
  if (iMap.find("{-ND-}") != iMap.end()){ cout << "\tDIMENSION   = " << iMap["{-ND-}"] << endl; }
  if (iMap.find("{-FN-}") != iMap.end()){ cout << "\tFILE NAME   = " << iMap["{-FN-}"] << endl; }
  if (iMap.find("{-HN-}") != iMap.end()){ cout << "\tHIST NAME   = " << iMap["{-HN-}"] << endl; }
  if (iMap.find("{-NT-}") != iMap.end()){ cout << "\tTREE NAME   = " << iMap["{-NT-}"] << endl; }
  if (iMap.find("{-CA-}") != iMap.end()){ cout << "\tCATEGORY    = " << iMap["{-CA-}"] << endl; }
  if (iMap.find("{-VA-}") != iMap.end()){ cout << "\tVARIABLE    = " << iMap["{-VA-}"] << endl; 
                   cout << "\t  EXPANDED  = " << FSTree::expandVariable(iMap["{-VA-}"]) << endl;}
  if (iMap.find("{-BO-}") != iMap.end()){ cout << "\tBOUNDS      = " << iMap["{-BO-}"] << endl; }
  if (iMap.find("{-CU-}") != iMap.end()){ cout << "\tCUTS        = " << iMap["{-CU-}"] << endl;
                   cout << "\t  EXPANDED  = " << FSTree::expandVariable(iMap["{-CU-}"]) << endl;}
  if (iMap.find("{-SC-}") != iMap.end()){ cout << "\tSCALE       = " << iMap["{-SC-}"] << endl; }
  if (iMap.find("{-FO-}") != iMap.end()){ cout << "\tFORMULA     = " << iMap["{-FO-}"] << endl;
                      cout << "\t  EXPANDED  = " << FSString::expandSUM(iMap["{-FO-}"]) << endl;}
  cout << "---------------------------------------------------------" << endl;
}

TString
FSHistogram::checkIndex(TString index, TString type){
  map<TString,TString> iMap = parseHistogramIndex(index);
    // checks on type (TP)
  if (type != "" && type != iMap["{-TP-}"]) return TString("!!BAD_TYPE!!");
  type = iMap["{-TP-}"];    if (type == "") return TString("!!NO_TYPE!!");
    // checks on dimension (ND)
  int dimension = FSString::TString2int(iMap["{-ND-}"]);
  if ((dimension != 1) && (dimension != 2)) return TString("!!BAD_DIM!!"); 
    // checks on bounds (BO)
  if (type == "TREE" || type == "FORMULA" || type == "EMPTY"){
    TString bounds = iMap["{-BO-}"];
    if (!FSString::checkBounds(dimension,bounds)) return TString("!!BAD_BOUNDS!!"); 
  }
    // checks on file (FN)
  if (type == "TREE" || type == "FILE"){
    TString fileName = iMap["{-FN-}"];
    if (FSSystem::getAbsolutePath(fileName) == "") return TString("!!NO_FILE!!");
  }
    // checks on tree (NT)
  if (type == "TREE"){
    TString treeName = iMap["{-NT-}"];
    if (treeName == "") return TString("!!NO_TREE!!");
  }
    // checks on category (CA)
  if (type == "TREE"){
    TString category = iMap["{-CA-}"];
    if (category == "!!NO_MODES!!") return TString("!!NO_MODES!!");
  }
    // checks on variable (VA)
  if (type == "TREE"){
    TString variable = iMap["{-VA-}"];
    if (variable == "") return TString("!!NO_VAR!!");
    if ((!FSString::checkParentheses(variable)) ||
        (!FSString::checkVariable(dimension,variable)))
      return TString("!!BAD_VAR!!");
  }
    // checks on cuts (CU)
  if (type == "TREE"){
    TString cuts = iMap["{-CU-}"];
    if (!FSString::checkParentheses(cuts)) return TString("!!BAD_CUTS!!");
    if (cuts.Contains("!!BAD_FSCUT!!")) return TString("!!BAD_FSCUT!!");
  }
    // checks on formula (FO)
  if (type == "FORMULA"){
    TString formula = iMap["{-FO-}"];
    if (!FSString::checkParentheses(formula)) return TString("!!BAD_FUNC!!");
  }
    // checks on scale (SC)
  if (type == "TREE"){
    TString scale = iMap["{-SC-}"];
    if (scale == "") return TString("!!NO_SCALE!!");
    scale = FSString::double2TString(FSString::TString2double(scale),3,true);
    if (scale == "0.00e+00") return TString("!!ZERO_SCALE!!");
  }
  return TString("OKAY");
}


TString
FSHistogram::makeFSRootHistName(int histNumber){
  TString hname("FSRootHist:");
  if (histNumber <= 0) histNumber = (++m_indexFSRootHistName);
  hname += FSString::int2TString(histNumber,6);
  return hname;
}

int
FSHistogram::getFSRootHistNumber(TString hName){
  return FSString::TString2int(hName);
}

TString
FSHistogram::makeFSRootTempName(){
  TString hname("FSRootTemp:");
  hname += FSString::int2TString(++m_indexFSRootTempName,6);
  return hname;
}


  // ********************************************************
  // ********************************************************
  // INTERACT WITH RDATAFRAME
  // ********************************************************
  // ********************************************************


void
FSHistogram::enableRDataFrame(bool executeImmediately, int numThreads){
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
  ROOT::EnableImplicitMT(numThreads);
  m_USEDATAFRAME = true;
  m_USEDATAFRAMENOW = executeImmediately;
#else
  cout << "enableRDataFrame(" << executeImmediately << "," << numThreads << ")" << endl;
  cout << "To use RDataFrame, the ROOT version should be greater than 6.18" << endl;
  exit(0);
#endif
}


void
FSHistogram::executeRDataFrame(){
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
    // first loop over the basic histograms
  for (map< TString, FSHistogramInfo* >::iterator mapItr = m_FSHistogramInfoCache.begin();
       mapItr != m_FSHistogramInfoCache.end(); mapItr++){
    FSHistogramInfo* histInfo = mapItr->second;
    if ((histInfo->m_waitingForEventLoop) && (histInfo->basic())){
      histInfo->m_waitingForEventLoop = false;
      if (histInfo->status().Contains("!!")) continue;
      cout << "    FILLING HISTOGRAM...  " << std::flush;
      map<TString,TString> indexMap = parseHistogramIndex(histInfo->m_index);
      double scale = 1.0;  
      if (indexMap.find("{-SC-}") != indexMap.end()) 
        scale = FSString::TString2double(indexMap["{-SC-}"]); 
      TString var = "";  
      if (indexMap.find("{-VA-}") != indexMap.end()) 
        var = indexMap["{-VA-}"]; 
      pair<TH1F*, TH2F*> histPair = histInfo->m_histPair;
      if (histPair.first){
        TH1F* hist = histPair.first;  TString hName = hist->GetName();
        ROOT::RDF::RResultPtr<TH1D> histRDF = histInfo->m_histPairRDF.first; 
        histRDF->Copy(*hist);  hist = getTH1F(hist);  hist->SetName(hName);  
        hist->Scale(scale); hist->SetTitle(var);
      }
      if (histPair.second){
        TH2F* hist = histPair.second;  TString hName = hist->GetName();
        ROOT::RDF::RResultPtr<TH2D> histRDF = histInfo->m_histPairRDF.second; 
        histRDF->Copy(*hist);  hist = getTH2F(hist);  hist->SetName(hName);  
        hist->Scale(scale); hist->SetTitle(var);
      }
      cout << histInfo->infoString() << endl;
    }
  }
    // then loop over the composite histograms
  for (map< TString, FSHistogramInfo* >::iterator mapItr = m_FSHistogramInfoCache.begin();
       mapItr != m_FSHistogramInfoCache.end(); mapItr++){
    FSHistogramInfo* histInfo = mapItr->second;
    if ((histInfo->m_waitingForEventLoop) && (!histInfo->basic())){
      cout << "    FILLING COMPOSITE...  " << std::flush;
      histInfo->m_waitingForEventLoop = false;
      pair<TH1F*,TH2F*> hComp = histInfo->m_histPair;
      for (unsigned int i = 0; i < histInfo->m_basicHistograms.size(); i++){
        pair<TH1F*,TH2F*> hBasic = histInfo->m_basicHistograms[i]->m_histPair;
        if (hComp.first  && hBasic.first)  hComp.first->Add(hBasic.first);
        if (hComp.second && hBasic.second) hComp.second->Add(hBasic.second);
      }
      if (hComp.first)  getTH1F(hComp.first);
      if (hComp.second) getTH2F(hComp.second);
      cout << histInfo->infoString() << endl;
    }
  }
#else
  cout << "To use RDataFrame, the ROOT version should be greater than 6.18" << endl;
  exit(0);
#endif
}


void
FSHistogram::disableRDataFrame(){
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
  ROOT::DisableImplicitMT();
  m_USEDATAFRAME = false;
  m_USEDATAFRAMENOW = false;
  for (map<TString, ROOT::RDataFrame*>::iterator mapItr = m_RDataFrameCache.begin();
           mapItr != m_RDataFrameCache.end(); mapItr++){
    if (mapItr->second) delete mapItr->second;
  }
  m_RDataFrameCache.clear();
  m_RDFVariableDefinitions.clear();
  m_RDFVariableCounter = 0;
  vector<int> rmHist;
  for (map<TString, FSHistogramInfo*>::iterator mapItr = m_FSHistogramInfoCache.begin();
           mapItr != m_FSHistogramInfoCache.end(); mapItr++){
    FSHistogramInfo* histInfo = mapItr->second;
    if (histInfo && histInfo->waitingForEventLoop()){
      if (histInfo->m_histPair.first) 
        rmHist.push_back(getFSRootHistNumber(histInfo->m_histPair.first->GetName()));
      if (histInfo->m_histPair.second) 
        rmHist.push_back(getFSRootHistNumber(histInfo->m_histPair.second->GetName()));
    }
  }
  for (unsigned int i = 0; i < rmHist.size(); i++){ clearHistogramCache(rmHist[i]); }
#else
  cout << "To use RDataFrame, the ROOT version should be greater than 6.18" << endl;
  exit(0);
#endif
}


  // *****************************************************
  // *****************************************************
  //          FSHISTOGRAMINFO CLASS (private)
  // *****************************************************
  // *****************************************************

FSHistogramInfo::FSHistogramInfo(TString index, vector<TString> expandedIndices){
  m_index = index;  m_histPair.first = NULL;  m_histPair.second = NULL;
  map<TString,TString> indexMap = FSHistogram::parseHistogramIndex(index);
  if (indexMap.find("{-VA-}") != indexMap.end()) m_title = indexMap["{-VA-}"];
  m_waitingForEventLoop = false;  m_status = "OKAY";
  for (unsigned int i = 0; i < expandedIndices.size(); i++){
    m_basicHistograms.push_back(FSHistogram::getFSHistogramInfo(expandedIndices[i]));
  }
}


pair<TH1F*,TH2F*>
FSHistogramInfo::getTHNF(){

    // case 1:  return a previously created histogram
  if (m_histPair.first || m_histPair.second){
    cout << "    FOUND HISTOGRAM...    " << std::flush;
  }

    // case 2:  set up histograms but don't fill them yet
  else if (FSHistogram::m_USEDATAFRAME && m_index.Contains("{-TP-}TREE")){
    m_histPair = FSHistogram::getTHNFBasicEmpty(m_index);
    if (m_histPair.first)  FSHistogram::getTH1F(m_histPair.first)->SetName(FSHistogram::makeFSRootHistName());
    if (m_histPair.second) FSHistogram::getTH2F(m_histPair.second)->SetName(FSHistogram::makeFSRootHistName());
    if (m_histPair.first)  m_histPair.first->SetTitle(m_title);
    if (m_histPair.second) m_histPair.second->SetTitle(m_title);
    m_waitingForEventLoop = true;

      // case 2a: set up a single histogram
    if (basic()){
      cout << "    SETTING HISTOGRAM...  " << std::flush;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
      m_histPairRDF = FSHistogram::getTHNFBasicTreeRDF(m_index,m_status);
#else
  cout << "To use RDataFrame, the ROOT version should be greater than 6.18" << endl;
  exit(0);
#endif
    }

      // case 2b: set up a composite histogram
    else{
      cout << "    SETTING COMPOSITE...  " << getHistName() 
           << "   (from " << m_basicHistograms.size() << " elements)" << endl;
      vector< pair<TH1F*,TH2F*> > vBasic;
      for (unsigned int i = 0; i < m_basicHistograms.size(); i++){
        vBasic.push_back(m_basicHistograms[i]->getTHNF());
      }
        // case 2c: if the basic histograms all exist, fill the composite
      if (!waitingForEventLoop()){
        for (unsigned int i = 0; i < vBasic.size(); i++){
          pair<TH1F*,TH2F*> hNew = vBasic[i];
          if (m_histPair.first  && hNew.first)  m_histPair.first->Add(hNew.first);
          if (m_histPair.second && hNew.second) m_histPair.second->Add(hNew.second);
        }
      }
      cout << "    FINISHED COMPOSITE... " << std::flush;
    }
  }

    // case 3: create a single histogram
  else if (basic()){
    cout << "    CREATING HISTOGRAM... " << std::flush;
    m_histPair = FSHistogram::getTHNFBasicIndex(m_index,m_status);
    if (m_histPair.first)  FSHistogram::getTH1F(m_histPair.first)->SetName(FSHistogram::makeFSRootHistName());
    if (m_histPair.second) FSHistogram::getTH2F(m_histPair.second)->SetName(FSHistogram::makeFSRootHistName());
  }

    // case 4: create a composite histogram
  else{
    TString compName = FSHistogram::makeFSRootHistName();
    cout << "    CREATING COMPOSITE... " << compName 
         << "   (from " << m_basicHistograms.size() << " elements)" << endl;
    pair<TH1F*,TH2F*> hP0 = m_basicHistograms[0]->getTHNF();
    if (hP0.first)  m_histPair.first  = FSHistogram::getTH1F((TH1F*)hP0.first->Clone(compName));
    if (hP0.second) m_histPair.second = FSHistogram::getTH2F((TH2F*)hP0.second->Clone(compName));
    for (unsigned int i = 1; i < m_basicHistograms.size(); i++){
      pair<TH1F*,TH2F*> hNew = m_basicHistograms[i]->getTHNF();
      if (m_histPair.first  && hNew.first)  m_histPair.first->Add(hNew.first);
      if (m_histPair.second && hNew.second) m_histPair.second->Add(hNew.second);
    }
    if (m_histPair.first)  m_histPair.first->SetTitle(m_title);
    if (m_histPair.second) m_histPair.second->SetTitle(m_title);
    cout << "    FINISHED COMPOSITE... " << std::flush;
  }

  cout << infoString() << endl;
  return m_histPair;
}


TTree* 
FSHistogramInfo::getTHNFContents(vector< pair<TString,TString> > extraTreeContents){
  TTree* histTree = NULL;
  if (basic()){
    cout << "    CREATING TREE FROM HISTOGRAM..." << std::flush;
    histTree = FSHistogram::getTHNFBasicContents(histTree,m_index,extraTreeContents,m_status);
  }
  else{
    cout << "    CREATING COMPOSITE TREE FROM HISTOGRAM... " << endl;
    for (unsigned int i = 0; i < m_basicHistograms.size(); i++){
      TString tmpStatus;
      histTree = FSHistogram::getTHNFBasicContents(histTree,
                                     m_basicHistograms[i]->m_index,extraTreeContents,tmpStatus);
    }
    cout << "    FINISHED COMPOSITE TREE FROM HISTOGRAM... " << std::flush;
  }
  cout << infoString() << "(entries = " << histTree->GetEntries() << ")" << endl;
  return histTree;
}

bool
FSHistogramInfo::waitingForEventLoop(){
  if (basic()) return m_waitingForEventLoop;
  m_waitingForEventLoop = false;
  for (unsigned int i = 0; i < m_basicHistograms.size(); i++){
    if (m_basicHistograms[i]->waitingForEventLoop()){
      m_waitingForEventLoop = true;  break;
    }
  }
  return m_waitingForEventLoop;
}

TString
FSHistogramInfo::status(){
  if (m_basicHistograms.size() == 0) return m_status;
  m_status = "OKAY";
  for (unsigned int i = 0; i < m_basicHistograms.size(); i++){
    if (m_basicHistograms[i]->status().Contains("!!")){
      m_status = "!!BAD_COMP!!";  break;
    }
  }
  return m_status;
}


void
FSHistogramInfo::show(bool showDetails){
  cout << "   NAME = " << infoString() << endl;
  cout << "  INDEX = " << m_index << endl; 
  if (m_basicHistograms.size() > 0) cout << "    COMPONENTS: " << endl;
  for (unsigned int i = 0; i < m_basicHistograms.size(); i++){
    cout << "    (" << i+1 << ")  " << m_basicHistograms[i]->infoString() << endl;
  }
  if (showDetails) FSHistogram::printIndexInfo(m_index);
}


TString
FSHistogramInfo::infoString(){
  TString info("");
  TString name = getHistName();  if (name == "") name = "NO NAME";
  info += FSString::padTString(name,20,"L");
  if (m_index.Contains("{-CA-}")){
    map<TString,TString> iMap = FSHistogram::parseHistogramIndex(m_index);
    info += FSString::padTString(iMap["{-CA-}"],18,"L");}
  info += FSString::padTString(status(),15,"L");
  if (m_histPair.first)  info += "(entries = " +
    FSString::double2TString(m_histPair.first->GetEntries(),0,false,true) + ")";
  if (m_histPair.second)  info += "(entries = " +
    FSString::double2TString(m_histPair.second->GetEntries(),0,false,true) + ")";
  if (waitingForEventLoop()) info += "  (WAITING) ";
  return info;
}

