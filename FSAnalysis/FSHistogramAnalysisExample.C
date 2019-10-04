#include <iostream>
#include <map>
#include "TChain.h"
#include "TH1F.h"
#include "TString.h"
#include "FSAnalysis/FSHistogramAnalysis.h"
#include "FSAnalysis/FSHistogramAnalysisExample.h"
#include "FSBasic/FSHistogram.h"


float enpa, pi0pullpa, chi2;

void 
FSHistogramAnalysisExample::setupBranches(TChain* nt){
  if (nt){
    nt->SetBranchAddress("EnPa",&enpa);
    nt->SetBranchAddress("Pi0PullPa",&pi0pullpa);
    nt->SetBranchAddress("Chi2",&chi2);
  }
}

void 
FSHistogramAnalysisExample::setNCuts(){
  n1cuts = 3;
  n2cuts = 3;
  n3cuts = 0;
  n4cuts = 0;
  n5cuts = 0;
  n6cuts = 0;
  n7cuts = 0;
  n8cuts = 0;
  n9cuts = 0;
}

void 
FSHistogramAnalysisExample::bookHistograms(TString key){
  h1map["h100"+key] = new TH1F("h100"+key,"Photon Energy",100,0.0,0.5);
  h1map["h200"+key] = new TH1F("h200"+key,"Chi2",100,0.0,50.0);
}


void 
FSHistogramAnalysisExample::setCuts(){

  l1[0] = true;
  if (pi0pullpa > 1.0) l1[1] = true;
  if (pi0pullpa > 2.0) l1[2] = true;
  if (pi0pullpa > 3.0) l1[3] = true;
  l2[0] = true;
  if (chi2 < 20.0) l2[1] = true;
  if (chi2 < 10.0) l2[2] = true;
  if (chi2 <  5.0) l2[3] = true;
  l3[0] = true;
  l4[0] = true;
  l5[0] = true;
  l6[0] = true;
  l7[0] = true;
  l8[0] = true;
  l9[0] = true;

}

void 
FSHistogramAnalysisExample::fillHistograms(TString key){
  h1map["h100"+key]->Fill(enpa);
  h1map["h200"+key]->Fill(chi2);
}



