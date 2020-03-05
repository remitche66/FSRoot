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
map< TString, ROOT::RDataFrame* > FSHistogram::m_RDataFrameCache;
map< TString, TString > FSHistogram::m_RDFVariableDefinitions;
unsigned int FSHistogram::m_RDFVariableCounter = 0;


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
  TString index = getHistogramIndexFile(1,fileName,histName);
  return getTH1F(getFSHistogramInfo(index)->getTHNF().first);
}

TH2F* 
FSHistogram::getTH2F(TString fileName, TString histName){
  TString index = getHistogramIndexFile(2,fileName,histName);
  return getTH2F(getFSHistogramInfo(index)->getTHNF().second);
}


pair<TH1F*,TH2F*> 
FSHistogram::getTHNFBasicFile(TString index){

    // initial checks
  index = FSString::removeWhiteSpace(index);
  if (checkIndex(index,"FILE").Contains("!!")) return getTHNFBasicEmpty(index); 

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
  TFile* tf = FSTree::getTFile(fileName); tf->cd();
  if (dimension == 1) hist1d = (TH1F*) gDirectory->FindObjectAny(histName);
  if (dimension == 2) hist2d = (TH2F*) gDirectory->FindObjectAny(histName);
  if (hist1d) getTH1F(hist1d)->SetName(makeFSRootTempName());
  if (hist2d) getTH2F(hist2d)->SetName(makeFSRootTempName());
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
  if (TESTONLY){ printIndexInfo(indices.first,indices.second); return NULL; }
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
  if (TESTONLY){ printIndexInfo(indices.first,indices.second); return NULL; }
  TH2F* hist = getFSHistogramInfo(indices.first,indices.second)->getTHNF().second;
  if (m_USEDATAFRAME && m_USEDATAFRAMENOW) executeRDataFrame();
  return getTH2F(hist);
} 


pair<TH1F*,TH2F*> 
FSHistogram::getTHNFBasicTree(TString index){
 
    // initial checks
  index = FSString::removeWhiteSpace(index);
  if (checkIndex(index,"TREE").Contains("!!")) return getTHNFBasicEmpty(index); 

    // adjust the input parameters
  map<TString,TString> mapIndex = parseHistogramIndex(index);
  int     dimension = FSString::TString2int(mapIndex["{-ND-}"]);
  TString fileName  = mapIndex["{-FN-}"];
  TString ntName    = mapIndex["{-NT-}"];
  TString variable  = mapIndex["{-VA-}"];  variable = FSTree::expandVariable(variable);
  TString bounds    = mapIndex["{-BO-}"];
  TString cuts      = mapIndex["{-CU-}"];  cuts = FSTree::expandVariable(cuts);
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
  TChain* chain = FSTree::getTChain(fileName,ntName);
  chain->Project(hbounds, variable, scaleTimesCuts);
  if (dimension == 1) hist1d = (TH1F*) gDirectory->FindObject(hname); 
  if (dimension == 2) hist2d = (TH2F*) gDirectory->FindObject(hname); 

    // return the created histogram
  if (hist1d){ getTH1F(hist1d)->SetName(hname); }
  if (hist2d){ getTH2F(hist2d)->SetName(hname); }
  if (FSControl::DEBUG){
    cout << "FSHistogram::getTHNFBasicTree DEBUG: finished making histogram from tree" << endl;
    printIndexInfo(index);
  }
  return pair<TH1F*,TH2F*>(hist1d,hist2d);
} 


pair< ROOT::RDF::RResultPtr<TH1D>, ROOT::RDF::RResultPtr<TH2D> >
FSHistogram::getTHNFBasicTreeRDF(TString index){
  pair< ROOT::RDF::RResultPtr<TH1D>, ROOT::RDF::RResultPtr<TH2D> > histPairRDF;

    // initial checks
  index = FSString::removeWhiteSpace(index);
  if (FSControl::DEBUG){
    cout << "FSHistogram::getTHNFBasicTreeRDF DEBUG: setting up histogram" << endl;
    printIndexInfo(index);
  }
  if (checkIndex(index,"TREE").Contains("!!")) return histPairRDF;

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
  string sVAR  = FSString::TString2string(variable);
  string sCUTS = FSString::TString2string(cuts);
  int nbinsx = FSString::parseBoundsNBinsX(bounds);
  double xlow = FSString::parseBoundsLowerX(bounds);
  double xhigh = FSString::parseBoundsUpperX(bounds);
  int nbinsy = FSString::parseBoundsNBinsY(bounds);
  double ylow = FSString::parseBoundsLowerY(bounds);
  double yhigh = FSString::parseBoundsUpperY(bounds);
  TString rdfIndex = "{-FN-}"+fileName + "{-NT-}"+ntName;
  TString hname = makeFSRootTempName();

    // find the right RDataFrame object
  ROOT::RDataFrame* RDF;
  if (m_RDataFrameCache.find(rdfIndex) != m_RDataFrameCache.end()){
    RDF = m_RDataFrameCache[rdfIndex]; 
  }
  else {
    vector<TString> allFiles = FSSystem::getAbsolutePaths(fileName);
    vector<string> rootFiles;
    for (unsigned int i = 0; i < allFiles.size(); i++){
      if (FSSystem::checkRootFormat(allFiles[i]))
        rootFiles.push_back(FSString::TString2string(allFiles[i]));
    }
    RDF = new ROOT::RDataFrame(sNT, rootFiles);
    m_RDataFrameCache[rdfIndex] = RDF;
  }

    // find the name of the variable
  TString varName(""); string sVarName("");
  if (m_RDFVariableDefinitions.find(variable) != m_RDFVariableDefinitions.end()){
    varName = m_RDFVariableDefinitions[variable];
  }
  else{
    varName = "VAR" + FSString::int2TString(m_RDFVariableCounter++);
    m_RDFVariableDefinitions[variable] = varName;
  }
  sVarName = FSString::TString2string(varName);

    // set up the histogram
  if (dimension == 1){
    histPairRDF.first = RDF->Filter(sCUTS)
                            .Define(sVarName,sVAR)
                            .Histo1D({hname,hname,nbinsx,xlow,xhigh},sVarName);
  }
  if (dimension == 2){
    histPairRDF.second = RDF->Filter(sCUTS)
                            .Define(sVarName,sVAR)
                            .Histo2D({hname,hname,nbinsx,xlow,xhigh,nbinsy,ylow,yhigh},sVarName);
  }

  return histPairRDF;

}



pair<TH1F*,TH2F*> 
FSHistogram::getTHNFBasicEmpty(TString index){

    // adjust the input parameters
  index = FSString::removeWhiteSpace(index);
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
                                vector< pair<TString,TString> > extraTreeContents){

    // initial checks
  index = FSString::removeWhiteSpace(index);
  if (checkIndex(index,"TREE").Contains("!!")) return histTree;

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
  vector<TString> vars = FSString::parseTString(variable,":");
  if ((int)vars.size() != dimension){
    cout << "FSHistogram::getTHNFBasicContents ERROR:  bad variable format" << endl;
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
    cout << "FSHistogram::getTHNFBasicContents ERROR: too many extra branches" << endl;
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

    // delete the TTreeFormula objects and return
  if (cutsF) delete cutsF;
  if (varXF) delete varXF;
  if (varYF) delete varYF;
  for (unsigned int i = 0; i < varEF.size(); i++){
    if (varEF[i]) delete varEF[i];
  }
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
  return getTH1FFormula(function->GetFormula()->GetExpFormula(),bounds,numRandomTrials);
}

TH2F*
FSHistogram::getTH2FFormula(TF2* function, TString bounds, int numRandomTrials){
  return getTH2FFormula(function->GetFormula()->GetExpFormula(),bounds,numRandomTrials);
}


pair<TH1F*,TH2F*>
FSHistogram::getTHNFBasicFormula(TString index){

    // initial checks
  index = FSString::removeWhiteSpace(index);
  if (checkIndex(index,"FORMULA").Contains("!!")) return getTHNFBasicEmpty(index); 

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


  // ********************************************
  //  MAKE PROJECTIONS, DO INTEGRALS, ETC.
  // ********************************************


TH1F*
FSHistogram::getTH1FRandom(TH1F* hist, int numRandomTrials){
  if (!hist) return NULL;
  TH1F* rhist = getTH1F((TH1F*)hist->Clone(makeFSRootTempName()));  rhist->Reset();
  TAxis* axisX = rhist->GetXaxis(); 
  int nbinsX = axisX->GetNbins(); 
  double fMax; double fMin;  hist->GetMinimumAndMaximum(fMin,fMax);
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
  TH2F* rhist = getTH2F((TH2F*)hist->Clone(makeFSRootTempName()));  rhist->Reset();
  TAxis* axisX = rhist->GetXaxis(); TAxis* axisY = rhist->GetYaxis();
  int nbinsX = axisX->GetNbins();   int nbinsY = axisX->GetNbins();
  double fMax; double fMin;  hist->GetMinimumAndMaximum(fMin,fMax);
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
FSHistogram::getTH1F(TH2F* hist2d, TString projectionAxis, bool function){
  TH1F* hist = NULL;
  if (projectionAxis == "x") projectionAxis = "X";
  if (projectionAxis == "y") projectionAxis = "Y";
  if ((projectionAxis != "X") && (projectionAxis != "Y")) return hist;
  int nbins1 = hist2d->GetNbinsX();
  int nbins2 = hist2d->GetNbinsY();
  double low1  = hist2d->GetXaxis()->GetXmin();
  double high1 = hist2d->GetXaxis()->GetXmax();
  double low2  = hist2d->GetYaxis()->GetXmin();
  double high2 = hist2d->GetYaxis()->GetXmax();
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
  double low  = hist->GetXaxis()->GetXmin();
  double high = hist->GetXaxis()->GetXmax();
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
  // SET MAXIMA/MINIMA SO HISTOGRAMS CAN BE SHOWN TOGETHER
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
  // DUMP THE HISTOGRAM CACHE
  // ********************************************************

void 
FSHistogram::dumpHistogramCache(string cacheName){

    // first sort (to make debugging easier)

  vector<FSHistogramInfo*> vecHistInfo;
  for (map<TString,FSHistogramInfo*>::iterator mpItr = m_FSHistogramInfoCache.begin();
       mpItr != m_FSHistogramInfoCache.end(); mpItr++){
    if ((mpItr->second) && (mpItr->second->getHistName() != ""))
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

  string textCacheName(cacheName);  textCacheName += ".cache.dat";
  string rootCacheName(cacheName);  rootCacheName += ".cache.root";
  TFile* rootCache = new TFile(rootCacheName.c_str(),"recreate");
  ofstream textCache(textCacheName.c_str());

    // write to files

  for (unsigned int i = 0; i < vecHistInfo.size(); i++){
    rootCache->cd();
    FSHistogramInfo* histInfo = vecHistInfo[i];
    TString histIndex = histInfo->m_index;
    if (histInfo->m_basicHistograms.size() > 0) continue;
    if (histInfo->m_waitingForEventLoop) continue;
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
FSHistogram::readHistogramCache(string cacheName){

    // open input files

  string textCacheName(cacheName);  textCacheName += ".cache.dat";
  string rootCacheName(cacheName);  rootCacheName += ".cache.root";
  TFile* rootCache = new TFile(rootCacheName.c_str());
  ifstream textCache(textCacheName.c_str());

    // read histograms

  TString histName;
  TString fileName(FSString::string2TString(rootCacheName));
  TString index;
  while (textCache >> index){
    textCache >> histName;
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
    FSTree::clearFileCache();  // (subtle: reusing the same cache name)
    pair<TH1F*,TH2F*> histPair = getTHNFBasicFile(getHistogramIndexFile(dim,fileName,histName));
    if (histPair.first)  histPair.first->SetName(makeFSRootHistName());
    if (histPair.second) histPair.second->SetName(makeFSRootHistName());
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
FSHistogram::clearHistogramCache(int histNumber){
  if (FSControl::DEBUG) 
    cout << "FSHistogram: clearing histogram cache " << histNumber << endl;
  for (map<TString,FSHistogramInfo*>::iterator rmItr = m_FSHistogramInfoCache.begin();
       rmItr != m_FSHistogramInfoCache.end(); ){
    bool eraseIt = false;
    FSHistogramInfo* hInfo = rmItr->second;
    if (!hInfo) eraseIt = true;  // shouldn't happen 
    if (hInfo){
      TString hName = hInfo->getHistName();
      if (histNumber < 0) eraseIt = true;
      if (histNumber > 0 && getFSRootHistNumber(hName) == histNumber) eraseIt = true;
    }
    if (eraseIt){
      if (hInfo){
        hInfo->m_basicHistograms.clear();
        if (hInfo->m_histPair.first)  delete hInfo->m_histPair.first;
        if (hInfo->m_histPair.second) delete hInfo->m_histPair.second;
        delete hInfo;
      }
      rmItr = m_FSHistogramInfoCache.erase(rmItr);
    }
    else{ rmItr++; }
  }
  if (FSControl::DEBUG) 
    cout << "FSHistogram: done clearing histogram cache" << endl;
}

void
FSHistogram::showHistogramCache(int histNumber, bool showDetails){
  cout << "-------------------------------------" << endl;
  cout << "-- CONTENTS OF THE HISTOGRAM CACHE --" << endl;
  cout << "-------------------------------------" << endl;
  vector<FSHistogramInfo*> vecHistInfo;
  for (map<TString,FSHistogramInfo*>::iterator mpItr = m_FSHistogramInfoCache.begin();
       mpItr != m_FSHistogramInfoCache.end(); mpItr++){
    if (histNumber < 0){ vecHistInfo.push_back(mpItr->second); continue; }
    TString hName(""); if (mpItr->second) hName = mpItr->second->getHistName();
    if (histNumber > 0 && getFSRootHistNumber(hName) == histNumber)
      vecHistInfo.push_back(mpItr->second);
  }
  for (unsigned int i = 0; i < vecHistInfo.size(); i++){
    if (!vecHistInfo[i]){
      cout << "FSHistogram ERROR: null FSHistogramInfo pointer in cache" << endl;
      exit(0);
    }
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
  for (unsigned int i = 0; i < vecHistInfo.size(); i++){
    vecHistInfo[i]->show(showDetails);
    if (!showDetails) cout << "-------------------------------------" << endl;
  }
}



  // ********************************************************
  // GENERALIZED BASIC FUNCTIONS CREATED FROM INDICES
  // ********************************************************


pair<TH1F*,TH2F*>
FSHistogram::getTHNFBasicIndex(TString index){
  index = FSString::removeWhiteSpace(index);
  if (index.Contains("{-TP-}FILE"))    return getTHNFBasicFile(index);
  if (index.Contains("{-TP-}TREE"))    return getTHNFBasicTree(index);
  if (index.Contains("{-TP-}EMPTY"))   return getTHNFBasicEmpty(index);
  if (index.Contains("{-TP-}FORMULA")) return getTHNFBasicFormula(index);
  return pair<TH1F*,TH2F*>(NULL,NULL);
}


  // *******************************************
  // INTERFACE TO THE FSHISTOGRAMINFO CLASS
  // *******************************************

vector<TString>
FSHistogram::expandHistogramIndexTree(TString index){
  vector<TString> expandedIndices;
    // expand "cuts" using FSCut and check for multidimensional sidebands
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
  fileName = FSString::removeWhiteSpace(fileName);
  histName = FSString::removeWhiteSpace(histName);
  index += "{-TP-}FILE";
  if (dimension == 1) index += "{-ND-}1D";
  if (dimension == 2) index += "{-ND-}2D";
  index += "{-FN-}";  index += fileName;
  index += "{-HN-}";  index += histName;
  return index;
}

TString
FSHistogram::getHistogramIndexFormula(int dimension, TString formula, TString bounds){
  TString index;
  formula  = FSString::removeWhiteSpace(formula);
  bounds   = FSString::removeWhiteSpace(bounds);
  index += "{-TP-}FORMULA";
  if (dimension == 1) index += "{-ND-}1D";
  if (dimension == 2) index += "{-ND-}2D";
  index += "{-FO-}";  index += formula;
  index += "{-BO-}";  index += bounds;
  return index;
}

TString
FSHistogram::getHistogramIndexEmpty(int dimension, TString bounds){
  TString index;
  bounds   = FSString::removeWhiteSpace(bounds);
  index += "{-TP-}EMPTY";
  if (dimension == 1) index += "{-ND-}1D";
  if (dimension == 2) index += "{-ND-}2D";
  index += "{-BO-}";  index += bounds;
  return index;
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
  if ((dimension != 1) && (dimension != 2)) return TString("!!BAD_DIMENSION!!"); 
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
  /*
    // checks on hisgotram (HN)
  if (type == "FILE"){
    TString fileName = iMap["{-FN-}"];
    TString histName = iMap["{-HN-}"];
    if (!FSSystem::checkRootFormat(fileName)) return TString("!!BAD_FILE!!");
    //if (!FSSystem::checkRootFormat(fileName,histName)) return TString("!!NO_HISTOGRAM!!");
  }
    // checks on tree (NT)
  if (type == "TREE"){
    TString fileName = iMap["{-FN-}"];
    TString ntName   = iMap["{-NT-}"];
    TChain* chain = FSTree::getTChain(fileName,ntName);
    if (!(chain) || (chain->GetEntries() == 0) || (chain->GetNbranches() == 0))
      return TString("!!NO_TREE!!");
  }
  */
    // checks on category (CA)
  if (type == "TREE"){
    TString category = iMap["{-CA-}"];
    if (category == "!!NO_MODES!!") return TString("!!NO_MODES!!");
  }
    // checks on variable (VA)
  if (type == "TREE"){
    TString variable = iMap["{-VA-}"];
    if (variable == "") return TString("!!NO_VARIABLE!!");
    if (!FSString::checkParentheses(variable)) return TString("!!BAD_VARIABLE!!");
  }
    // checks on cuts (CU)
  if (type == "TREE"){
    TString cuts = iMap["{-CU-}"];
    if (!FSString::checkParentheses(cuts)) return TString("!!BAD_CUTS!!");
  }
    // checks on formula (FO)
  if (type == "FORMULA"){
    TString formula = iMap["{-FO-}"];
    if (!FSString::checkParentheses(formula)) return TString("!!BAD_FORMULA!!");
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
FSHistogram::makeFSRootHistName(){
  TString hname("FSRootHist:");
  hname += FSString::int2TString(++m_indexFSRootHistName,6);
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
  ROOT::EnableImplicitMT(numThreads);
  m_USEDATAFRAME = true;
  m_USEDATAFRAMENOW = executeImmediately;
}


void
FSHistogram::executeRDataFrame(){
    // first loop over the basic histograms
  for (map< TString, FSHistogramInfo* >::iterator mapItr = m_FSHistogramInfoCache.begin();
       mapItr != m_FSHistogramInfoCache.end(); mapItr++){
    FSHistogramInfo* histInfo = mapItr->second;
    if ((histInfo->m_waitingForEventLoop) && (histInfo->m_basicHistograms.size() == 0)){
      histInfo->m_waitingForEventLoop = false;
      if (histInfo->m_status.Contains("!!")) continue;
      cout << "    FILLING HISTOGRAM...  " << std::flush;
      map<TString,TString> indexMap = parseHistogramIndex(histInfo->m_index);
      double scale = 1.0;  
      if (indexMap.find("{-SC-}") != indexMap.end()) 
        scale = FSString::TString2double(indexMap["{-SC-}"]); 
      pair<TH1F*, TH2F*> histPair = histInfo->m_histPair;
      if (histPair.first){
        TH1F* hist = histPair.first;  TString hName = hist->GetName();
        ROOT::RDF::RResultPtr<TH1D> histRDF = histInfo->m_histPairRDF.first; 
        histRDF->Copy(*hist);  hist = getTH1F(hist);  hist->SetName(hName);  hist->Scale(scale);
      }
      if (histPair.second){
        TH2F* hist = histPair.second;  TString hName = hist->GetName();
        ROOT::RDF::RResultPtr<TH2D> histRDF = histInfo->m_histPairRDF.second; 
        histRDF->Copy(*hist);  hist = getTH2F(hist);  hist->SetName(hName);  hist->Scale(scale);
      }
      cout << histInfo->infoString() << endl;
    }
  }
    // then loop over the composite histograms
  for (map< TString, FSHistogramInfo* >::iterator mapItr = m_FSHistogramInfoCache.begin();
       mapItr != m_FSHistogramInfoCache.end(); mapItr++){
    FSHistogramInfo* histInfo = mapItr->second;
    if ((histInfo->m_waitingForEventLoop) && (histInfo->m_basicHistograms.size() != 0)){
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
}


void
FSHistogram::disableRDataFrame(){
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
    if (histInfo && histInfo->m_waitingForEventLoop){
      if (histInfo->m_histPair.first) 
        rmHist.push_back(getFSRootHistNumber(histInfo->m_histPair.first->GetName()));
      if (histInfo->m_histPair.second) 
        rmHist.push_back(getFSRootHistNumber(histInfo->m_histPair.second->GetName()));
    }
  }
  for (unsigned int i = 0; i < rmHist.size(); i++){ clearHistogramCache(rmHist[i]); }
}


  // *****************************************************
  // *****************************************************
  //          FSHISTOGRAMINFO CLASS (private)
  // *****************************************************
  // *****************************************************

FSHistogramInfo::FSHistogramInfo(TString index, vector<TString> expandedIndices){
  m_index = index;  m_histPair.first = NULL;  m_histPair.second = NULL;
  m_waitingForEventLoop = false;  m_status = "OKAY";
  if (expandedIndices.size() == 0) m_status = FSHistogram::checkIndex(index);
  for (unsigned int i = 0; i < expandedIndices.size(); i++){
    m_basicHistograms.push_back(FSHistogram::getFSHistogramInfo(expandedIndices[i]));
    if (m_basicHistograms[i]->m_status.Contains("!!")) m_status = "!!BAD_COMPONENT!!";
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
    TString eIndex(m_index); eIndex.Replace(eIndex.Index("{-TP-}TREE"),10,"{-TP-}EMPTY");
    m_histPair = FSHistogram::getTHNFBasicIndex(eIndex);
    if (m_histPair.first)  FSHistogram::getTH1F(m_histPair.first)->SetName(FSHistogram::makeFSRootHistName());
    if (m_histPair.second) FSHistogram::getTH2F(m_histPair.second)->SetName(FSHistogram::makeFSRootHistName());
    m_waitingForEventLoop = true;

      // case 2a: set up a single histogram
    if (m_basicHistograms.size() == 0){
      cout << "    SETTING HISTOGRAM...  " << std::flush;
      m_histPairRDF = FSHistogram::getTHNFBasicTreeRDF(m_index);
    }

      // case 2b: set up a composite histogram
    else{
      cout << "    SETTING COMPOSITE...  " << getHistName() 
           << "   (from " << m_basicHistograms.size() << " elements)" << endl;
      m_waitingForEventLoop = false;
      vector< pair<TH1F*,TH2F*> > vBasic;
      for (unsigned int i = 0; i < m_basicHistograms.size(); i++){
        vBasic.push_back(m_basicHistograms[i]->getTHNF());
        if (m_basicHistograms[i]->m_waitingForEventLoop) m_waitingForEventLoop = true;
      }
        // case 2c: if the basic histograms all exist, fill the composite
      if (!m_waitingForEventLoop){
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
  else if (m_basicHistograms.size() == 0){
    cout << "    CREATING HISTOGRAM... " << std::flush;
    m_histPair = FSHistogram::getTHNFBasicIndex(m_index);
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
    cout << "    FINISHED COMPOSITE... " << std::flush;
  }

  cout << infoString() << endl;
  return m_histPair;
}


TTree* 
FSHistogramInfo::getTHNFContents(vector< pair<TString,TString> > extraTreeContents){
  TTree* histTree = NULL;
  if (m_basicHistograms.size() == 0){
    cout << "    CREATING TREE FROM HISTOGRAM..." << std::flush;
    histTree = FSHistogram::getTHNFBasicContents(histTree,m_index,extraTreeContents);
  }
  else{
    cout << "    CREATING COMPOSITE TREE FROM HISTOGRAM... " << endl;
    for (unsigned int i = 0; i < m_basicHistograms.size(); i++){
      histTree = FSHistogram::getTHNFBasicContents(histTree,
                                     m_basicHistograms[i]->m_index,extraTreeContents);
    }
    cout << "    FINISHED COMPOSITE TREE FROM HISTOGRAM... " << std::flush;
  }
  cout << " #entries = " << histTree->GetEntries() << endl;
  return histTree;
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
  info += FSString::padTString(m_status,20,"L");
  if (m_histPair.first)  info += "   (entries = " +
    FSString::double2TString(m_histPair.first->GetEntries(),0,false,true) + ")";
  if (m_histPair.second)  info += "   (entries = " +
    FSString::double2TString(m_histPair.second->GetEntries(),0,false,true) + ")";
  if (m_waitingForEventLoop) info += "  (WAITING) ";
  return info;
}

